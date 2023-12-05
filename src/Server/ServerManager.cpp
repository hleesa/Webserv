#include "../../inc/ServerManager.hpp"
#include "../../inc/ToString.hpp"
#include "../../inc/ErrorPage.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/PostCgi.hpp"
#include "../../inc/ServerUtils.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>

ServerManager::ServerManager(const std::vector<Config>* configs) : default_config(&configs->front()){
	setConfigByServerName(configs);
    addListenEvent();
    memset((void*)event_list, 0, sizeof(struct kevent) * NUMBER_OF_EVENT);
    kq = kqueue();
    if (kq == ERROR) {
        throw (strerror(errno));
	}
}

ServerManager::~ServerManager() {}

void ServerManager::setConfigByServerName(const std::vector<Config>* configs) {
	std::vector<Config>::const_iterator itr = (*configs).begin();
    std::map<int, bool> is_used_ports;

    for (;itr != (*configs).end(); itr++) {
		int port = itr->getPort();
		if (!is_used_ports[port]) {
            is_used_ports[port] = true;
			int socket = openListenSocket(port);
            listen_sockets.insert(socket);
			if (fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
				throw (strerror(errno));
			}
		}
		std::vector<std::string> server_names = itr->getName();
		std::vector<std::string>::iterator name = server_names.begin();
		for (; name != server_names.end(); name++) {
			const Config* config = &*itr;
            std::string server_name = port == 80 ? *name : *name + ":" + to_string(port);
			server_name_to_config[server_name].push_back(config);
		}
	}
}

void ServerManager::handleError(const int return_value, const int listen_socket) const {
    if (return_value == ERROR) {
        close(listen_socket);
        throw (strerror(errno));
    }
}

int ServerManager::openListenSocket(const int port) const {
    int listen_socket;
    struct sockaddr_in server_address;

    listen_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == ERROR) {
        throw (strerror(errno));
	}
	int option_value = 1;
	handleError(setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)), listen_socket);
	memset((void*) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	handleError(bind(listen_socket, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)), listen_socket);
	handleError(listen(listen_socket, NUMBER_OF_BACKLOG), listen_socket);
    return listen_socket;
}

void ServerManager::addListenEvent() {
    std::set<int>::iterator itr = listen_sockets.begin();

    for (; itr != listen_sockets.end(); itr++) {
        change_list.push_back(makeEvent(*itr, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
    }
}

void ServerManager::run() {
    while (true) {
		try {
			int events = kevent(kq, &(change_list[0]), change_list.size(), event_list, NUMBER_OF_EVENT, 0);

			if (events == ERROR)
				throw (strerror(errno));
			change_list.clear();
			processEvents(events);
		}
		catch (std::exception e) {
			std::cerr << e.what() << std::endl;
		}
		catch (int e) {
			std::cerr << e << std::endl;
		}
    }
}

void ServerManager::processEvents(const int events) {
    for (int idx = 0; idx < events; idx++) {
        struct kevent* event = event_list + idx;
		try {
			processEvent(event);
		}
		catch (int status_code) {
			std::map<std::string, std::string> header = Method::makeHeaderFields();
			header["Connection"] = "close";
			header["Content-length"] = "0";
            servers[event->ident].setResponse(HttpResponseMessage(status_code, header, "").toString());
		}
	}
}

EventType ServerManager::getEventType(const struct kevent* event){
    if (event->flags & EV_ERROR) {
        return EVENT_ERROR;
    }
    EventType event_type = EVENT_ERROR;
    switch (event->filter) {
        case EVFILT_READ:
            if (listen_sockets.find(event->ident) != listen_sockets.end()) {
                event_type = LISTEN;
            }
            else if (servers.find(event->ident) != servers.end()) {
                event_type = PARSE_REQUEST;
            }
            else if (event->udata != NULL) {
                event_type = READ_PIPE;
            }
            break;
        case EVFILT_WRITE:
            if (servers.find(event->ident) != servers.end()) {
                event_type = SEND_RESPONSE;
            }
            else if (event->udata != NULL) {
                event_type = WRITE_PIPE;
            }
            break;
        case EVFILT_TIMER:
            if (event->udata == NULL) {
                event_type = TIMEOUT;
            }
            else {
                event_type = TIMEOUT_CGI;
            }
            break;
        case EVFILT_PROC:
            event_type = CGI_END;
            break;
    }
    return event_type;
}

void ServerManager::assignParsedRequest(const struct kevent* event) {
    HttpRequestMessage request = parser.getHttpRequestMessage(event->ident);
    servers[event->ident].setRequest(request);
    parser.clear(event->ident);
}

void ServerManager::processCgiOrMakeResponse(const struct kevent* event) {
    HttpRequestMessage* request = servers[event->ident].getRequestPtr();
    if (request->getStatusCode() != 0) {
        change_list.push_back(makeEvent(event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
        servers[event->ident].setResponse(ErrorPage::makeErrorPageResponse(request->getStatusCode(), default_config).toString());
        change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, NULL));
        return;
    } 
	const Config* config = findConfig(request->getHost(), request->getURL());
    if (isCgi(config, request)) {
        PostCgi post_cgi(request, config);
        CgiData* cgi_data = post_cgi.cgipost();
        cgi_data->setConnSocket(event->ident);
        change_list.push_back(makeEvent(cgi_data->getWritePipeFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(cgi_data)));
        change_list.push_back(makeEvent(cgi_data->getReadPipeFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(cgi_data)));
        change_list.push_back(makeEvent(cgi_data->getChildPid(), EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, 0, reinterpret_cast<void*>(cgi_data)));
        change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, reinterpret_cast<void*>(cgi_data)));
    }
    else {
        change_list.push_back(makeEvent(event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
        servers[event->ident].setResponse(servers[event->ident].makeResponse(config));
        change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, NULL));
    }
}

void ServerManager::processEvent(const struct kevent* event) {
    EventType event_type = getEventType(event);

    switch (event_type) {
        case EVENT_ERROR:
            std::cout << "error " << strerror(event->data) << '\n';
//            checkEventError(*event);
            break;
        case LISTEN:
            processListenEvent(event);
            break;
        case PARSE_REQUEST:
            processReceiveEvent(event);
            if (parser.getReadingStatus(event->ident) == END) {
                assignParsedRequest(event);
                processCgiOrMakeResponse(event);
            }
            break;
        case SEND_RESPONSE:
            processSendEvent(event);
            break;
        case READ_PIPE:
            processReadPipeEvent(event);
            break;
        case WRITE_PIPE:
            processWritePipeEvent(event);
            break;
        case TIMEOUT:
            std::cout << "time out\n";
            disconnectWithClient(event);
            break;
        case TIMEOUT_CGI:
            processTimeoutCgiEvent(event);
            break;
        case CGI_END:
            processCgiEnd(event);
            break;
    }
}

void ServerManager::processTimeoutCgiEvent(const struct kevent* event) {
    CgiData* cgi_data = reinterpret_cast<CgiData*>(event->udata);
    close(cgi_data->getReadPipeFd());
    close(cgi_data->getWritePipeFd());
    processCgiTermination(cgi_data);
    disconnectWithClient(event);
}

void ServerManager::processCgiEnd(const struct kevent* event) {
    CgiData* cgi_data = reinterpret_cast<CgiData*>(event->udata);
    if (cgi_data->cgiDied()) {
        processCgiTermination(cgi_data);
    }
    else {
        cgi_data->setCgiDie(true);
        change_list.push_back(makeEvent(cgi_data->getReadPipeFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(cgi_data)));
    }
}

void ServerManager::processCgiTermination(CgiData* cgi_data) {
    int status;
    if (waitpid(cgi_data->getChildPid(), &status, 0) == ERROR) {
        delete cgi_data;
        throw 500;
    }
    if (WIFEXITED(status)) { // 자식 종료
        int exit_status = WEXITSTATUS(status);
        if (exit_status == EXIT_FAILURE) { // 비정상 종료
            delete cgi_data;
            throw 500;
        }
    }
    else {
        kill(cgi_data->getChildPid(), SIGTERM);
        delete cgi_data;
        throw 500;
    }
    delete cgi_data;
}

void ServerManager::processReadPipeEvent(const struct kevent* event) {
    char buff[BUFFER_SIZE];
    memset(buff, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(event->ident, &buff, BUFFER_SIZE);
    CgiData* cgi_data = reinterpret_cast<CgiData*>(event->udata);
	Server *server = &servers[cgi_data->getConnSocket()];

    if (bytes_read == ERROR) {
//        std::cout << errno << " " <<  strerror(errno) << '\n';
        return;
    }
    else if (bytes_read > 0) {
		server->appendResponse(buff, bytes_read);
    }
    else { // EOF
        server->setResponse(PostCgi::makeResponse(server->getResponse()).toString());
        close(cgi_data->getReadPipeFd());
        change_list.push_back(makeEvent(cgi_data->getConnSocket(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
        if (cgi_data->cgiDied()) {
            processCgiTermination(cgi_data);
        }
        else {
            cgi_data->setCgiDie(true);
            change_list.push_back(makeEvent(cgi_data->getChildPid(), EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, 0, reinterpret_cast<void*>(cgi_data)));
        }
    }
}

void ServerManager::processWritePipeEvent(const struct kevent* event) {
    CgiData* cgi_data = reinterpret_cast<CgiData*>(event->udata);
    Server *server = &servers[cgi_data->getConnSocket()];

    ssize_t bytes_written = write(event->ident, server->getMessageBodyPtr(), server->getBytesToWrite());
    if (bytes_written == ERROR) {
        return;
    }
    server->updateBytesWritten(bytes_written);
    if (server->writeComplete()) {
        server->clearRequestBodyPtr();
        close(cgi_data->getWritePipeFd());
    }
    change_list.push_back(makeEvent(cgi_data->getConnSocket(), EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, reinterpret_cast<void*>(cgi_data)));
}

void ServerManager::processReceiveEvent(const struct kevent* event) {
    char buff[BUFFER_SIZE + 1];
    ssize_t bytes_recv = recv(event->ident, &buff, BUFFER_SIZE, 0);
    
	if (bytes_recv == ERROR) {
       return;
    }
    buff[bytes_recv] = '\0';
	parser.run(event->ident, buff);
}

void ServerManager::processSendEvent(const struct kevent* event) {
    Server *server = &servers[event->ident];
    ssize_t bytes_sent = send(event->ident, server->getResponsePtr(), server->getBytesToSend(), 0);
    if (bytes_sent == ERROR) {
        return;
    }
    server->updateBytesSent(bytes_sent);
    if (server->sendComplete()) {
        server->clearResponse();
        change_list.push_back(makeEvent(event->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL));
    }
    change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, NULL));
}

void ServerManager::processListenEvent(const struct kevent* event) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int connection_socket = accept(event->ident, (struct sockaddr*) &client_address, &client_address_len);

    if (connection_socket == ERROR) {
        throw (strerror(errno));
    }
    if (fcntl(connection_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
        throw (strerror(errno));
    }
    servers[connection_socket] = Server(event->ident);
    change_list.push_back(makeEvent(connection_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
    change_list.push_back(makeEvent(connection_socket, EVFILT_TIMER, EV_ADD | EV_ONESHOT, NOTE_SECONDS, TIMEOUT_SEC, NULL));
}

const Config* ServerManager::findConfig(const std::string host, const std::string url) {
    if (server_name_to_config.find(host) == server_name_to_config.end()) {
        return default_config;
    }
    std::vector<const Config*>::iterator itr = server_name_to_config[host].begin();
    for (;itr != server_name_to_config[host].end(); itr++) {
        const Config* config = *itr;
        if (config->hasLocationOf(url)) {
            return config;
        }
    }
    const Config* config = *server_name_to_config[host].begin();
    return config;
}

void ServerManager::checkEventError(const struct kevent* event) {
    // if (configs.find(event.ident) != configs.end())
    if (servers.find(event->ident) != servers.end())
        disconnectWithClient(event);
}

struct kevent ServerManager::makeEvent(
        uintptr_t ident,
        int16_t filter,
        uint16_t flags,
        uint32_t fflags,
        intptr_t data,
        void* udata) const {
    struct kevent event;

    EV_SET(&event, ident, filter, flags, fflags, data, udata);
    return event;
}

void ServerManager::disconnectWithClient(const struct kevent* event) {
//	 struct linger linger;
	
//	 linger.l_onoff = 1;
//	 linger.l_linger = 0;
//	 setsockopt(event.ident, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)); // Linger option
    close(event->ident);
    servers.erase(event->ident);
}
