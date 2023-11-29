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

void ServerManager::processEvent(const struct kevent* event) {
//    if (event->flags & EV_ERROR) {
//        std::cout << "error\n";
//        checkEventError(*event);
//    }

    if (event->filter == EVFILT_READ) {
        if (listen_sockets.find(event->ident) != listen_sockets.end()) {
            processListenEvent(*event);
        }
        else if (servers.find(event->ident) != servers.end()) {
            processReadEvent(*event);
            if (parser.getReadingStatus(event->ident) == END) {
                HttpRequestMessage request = parser.getHttpRequestMessage(event->ident);
                if (request.getStatusCode() != 0) {
                    servers[event->ident].setResponse(ErrorPage::makeErrorPageResponse(request.getStatusCode(), default_config).toString());
                    return;
                }
                const Config* config = findConfig(request.getHost(), request.getURL());
                servers[event->ident].setRequest(request);
                parser.clear(event->ident);


                if (isCgi(config, &request)) {
                    PostCgi post_cgi(&request, config);
                    PostCgiPipePid* cgi_pipe_pid = post_cgi.cgipost();
                    int* conn_sock = new int;
                    *conn_sock = event->ident;
                    change_list.push_back(makeEvent(cgi_pipe_pid->getWritePipeFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(conn_sock)));
                    change_list.push_back(makeEvent(cgi_pipe_pid->getReadPipeFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, reinterpret_cast<void*>(conn_sock)));
                    change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT_MSEC, reinterpret_cast<void*>(cgi_pipe_pid)));
                }
                else {
                    servers[event->ident].setResponse(servers[event->ident].makeResponse(config));
                    change_list.push_back(makeEvent(event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
                }
            }
        }
        else if (event->udata != NULL) {
            processPipeReadEvent(*event);
        }
    }
    else if (event->filter == EVFILT_WRITE) {
        if (servers.find(event->ident) != servers.end() ) {
            processWriteEvent(*event);
            change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT_MSEC, NULL));
        }
        else if (event->udata != NULL) {
            processPipeWriteEvent(*event);
        }
    }
    else if (event->filter == EVFILT_TIMER) {
        if (event->udata == NULL) {
            std::cout << "time out\n";
            change_list.push_back(makeEvent(event->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL));
        }
        else {
            // cgi post time out
            PostCgiPipePid* pipe_pid = reinterpret_cast<PostCgiPipePid*>(event->udata);
            kill(pipe_pid->getChildPid(), SIGTERM);
            delete pipe_pid;
            change_list.push_back(makeEvent(event->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL));
        }
        disconnectWithClient(*event);
    }
}

void ServerManager::processPipeReadEvent(const struct kevent& event) {
    char buff[BUFFER_SIZE];
    memset(buff, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(event.ident, &buff, BUFFER_SIZE);
	int* server_idx = reinterpret_cast<int*>(event.udata);
	Server *server = &servers[*server_idx];

    
	
    if (bytes_read == ERROR) {
        std::cout << errno << " " <<  strerror(errno) << '\n';
        return;
    }
    else if (bytes_read > 0) {
		server->appendResponse(buff, bytes_read);
    }
	else { // EOF
		//server->setResponse(PostCgi::makeResponse(server->getResponse()));
		change_list.push_back(makeEvent(*server_idx, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
		close(event.ident);
	}
}

void ServerManager::processPipeWriteEvent(const struct kevent& event) {
    int* server_idx = reinterpret_cast<int*>(event.udata);
    Server *server = &servers[*server_idx];

    std::string requestBody = server->getRequestPtr()->getMessageBody();
    ssize_t bytes_write = write(event.ident, requestBody.c_str(), requestBody.length());
    if (bytes_write == ERROR) {
        std::cout << errno << " " <<  strerror(errno) << '\n';
        return;
    }
    if (requestBody.length() == static_cast<size_t>(bytes_write)) {
        change_list.push_back(makeEvent(event.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL));
        close(event.ident);
	}



//    std::string http_message_body = request->getMessageBody();
//    size_t body_size = http_message_body.length();
//    ssize_t bytes_written = 0;
//    while (body_size > 0) {
//        ssize_t bytes_write = write(pipe_parent_to_child[WRITE], http_message_body.c_str() + bytes_written, body_size);
//        if (bytes_write == ERROR) {
//            throw 500;
//        }
//        bytes_written += bytes_written;
//        body_size -= bytes_write;
//    }

//    ssize_t bytes_write = write(pipe_pid->getWritePipeFd(),)
//
//    ssize_t bytes_sent = send(event.ident, server->getResponse().c_str(), server->getResponse().length(), 0);
//    if (bytes_sent != ERROR) {
//        server->updateResponse(bytes_sent);
//        if (server->isSendComplete()) {
//            server->clearResponse();
//            change_list.push_back(makeEvent(event.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL));
//        }
//    }

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

void ServerManager::checkEventError(const struct kevent& event) {
    // if (configs.find(event.ident) != configs.end())
    if (servers.find(event.ident) != servers.end())
        disconnectWithClient(event);
}

void ServerManager::processListenEvent(const struct kevent& event) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);
    int connection_socket = accept(event.ident, (struct sockaddr*) &client_address, &client_address_len);

    if (connection_socket == ERROR) {
        throw (strerror(errno));
	}
	if (fcntl(connection_socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
        throw (strerror(errno));
	}
    servers[connection_socket] = Server(event.ident);
    change_list.push_back(makeEvent(connection_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
    change_list.push_back(makeEvent(connection_socket, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT_MSEC, NULL));
}

void ServerManager::processReadEvent(const struct kevent& event) {
    char buff[BUFFER_SIZE + 1];
    ssize_t bytes_recv = recv(event.ident, &buff, BUFFER_SIZE, 0);
    
	if (bytes_recv == ERROR) {
       return;
    }
    buff[bytes_recv] = '\0';
	parser.run(event.ident, buff);
}

void ServerManager::processWriteEvent(const struct kevent& event) {
    Server *server = &servers[event.ident];
    ssize_t bytes_sent = send(event.ident, server->getResponse().c_str(), server->getResponse().length(), 0);
    if (bytes_sent != ERROR) {
        server->updateResponse(bytes_sent);
        if (server->isSendComplete()) {
            server->clearResponse();
            change_list.push_back(makeEvent(event.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL));
        }
    }
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

void ServerManager::disconnectWithClient(const struct kevent& event) {
	// struct linger linger;
	
	// linger.l_onoff = 1;
	// linger.l_linger = 0;
	// setsockopt(event.ident, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger)); // Linger option
    close(event.ident);
    servers.erase(event.ident);
}
