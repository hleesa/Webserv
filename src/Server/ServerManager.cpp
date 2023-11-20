#include "../../inc/ServerManager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

ServerManager::ServerManager(const std::vector<Config>& configs) {
    for (unsigned long idx = 0; idx < configs.size(); idx++) {
		int socket = openListenSocket(configs[idx].getPort());
		if (fcntl(socket, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
			throw (strerror(errno));
		}
        this->configs[socket] = configs[idx];
	}
    memset((void*) event_list, 0, sizeof(struct kevent) * NUMBER_OF_EVENT);
    kq = kqueue();
    if (kq == ERROR)
        throw (strerror(errno));
}

ServerManager::~ServerManager() {}

int ServerManager::openListenSocket(const int port) const {
    int listen_socket;
    struct sockaddr_in server_address;

    listen_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == ERROR)
        throw (strerror(errno));
    memset((void*) &server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_socket, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
        throw (strerror(errno));
    if (listen(listen_socket, NUMBER_OF_BACKLOG) == ERROR)
        throw (strerror(errno));
    return listen_socket;
}

void ServerManager::run() {
    addListenEvent();
    try {
        while (true) {
            int events = kevent(kq, &(change_list[0]), change_list.size(), event_list, NUMBER_OF_EVENT, 0);

            if (events == ERROR)
                throw (strerror(errno));
            change_list.clear();
            processEvents(events);
        }
    } catch (std::exception &e) {
        std::cerr << "exception: " << e.what() << '\n';    }

}

void ServerManager::processEvents(const int events) {
    for (int idx = 0; idx < events; idx++) {
        struct kevent* event = event_list + idx;

        if (event->flags & EV_ERROR) {
            checkEventError(*event);
        }
        else if (event->filter == EVFILT_READ && configs.find(event->ident) != configs.end()) {
            processListenEvent(*event);
        }
        else if (event->filter == EVFILT_READ && servers.find(event->ident) != servers.end()) {
            processReadEvent(*event);
			if (parser.getReadingStatus(event->ident) == END) {
				int limit_body_size = configs[servers[event->ident].getListenSocket()].getLimitBodySize();
				servers[event->ident].setRequest(parser.getHttpRequestMessage(event->ident, limit_body_size));
				parser.clear(event->ident);
    			change_list.push_back(makeEvent(event->ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL));
                servers[event->ident].setResponse(servers[event->ident].makeResponse(configs));

			}
        }
        else if (event->filter == EVFILT_WRITE && servers.find(event->ident) != servers.end()) {
            processWriteEvent(*event);
            change_list.push_back(makeEvent(event->ident, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT_MSEC, NULL));
        }
        else if (event->filter == EVFILT_TIMER) {
            std::cout << "time out\n";
            change_list.push_back(makeEvent(event->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL));
            disconnectWithClient(*event);
        }
    }
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
    servers[connection_socket] = Server(connection_socket, event.ident);
    change_list.push_back(makeEvent(connection_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
    change_list.push_back(makeEvent(connection_socket, EVFILT_TIMER, EV_ADD | EV_ONESHOT, 0, TIMEOUT_MSEC, NULL));
}

void ServerManager::processReadEvent(const struct kevent& event) {
    char buff[BUFFER_SIZE + 1];
    ssize_t bytes_recv = recv(event.ident, &buff, BUFFER_SIZE, 0);
    
	if (bytes_recv == ERROR) {
        if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
            disconnectWithClient(event);
        }
        return;
    }
    buff[bytes_recv] = '\0';
	parser.run(event.ident, buff);
}

void ServerManager::processWriteEvent(const struct kevent& event) {
//	std::string response = servers[event.ident].makeResponse(configs);
    Server *server = &servers[event.ident];
    int bytes_send = send(event.ident, &server->getResponse()[server->getBytesSend()], server->getResponseLength() - server->getBytesSend(), 0);
    if (bytes_send == ERROR) {
        if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
            disconnectWithClient(event);
        }
    }
    else {
        server->setBytesSend(server->getBytesSend() + bytes_send);
    }
}

void ServerManager::addListenEvent() {
    std::map<int, Config>::iterator itr = configs.begin();

    for (; itr != configs.end(); itr++) {
        change_list.push_back(makeEvent(itr->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
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
    close(event.ident);
    servers.erase(event.ident);
}
