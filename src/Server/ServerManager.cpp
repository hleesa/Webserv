#include "ServerManager.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

ServerManager::ServerManager(const std::vector<Config>& configs) {
    for (int idx = 0; idx < configs.size(); idx++) {
        this->configs[openListenSocket(configs[idx].getPort())] = configs[idx];
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
    while (true) {
        int events = kevent(kq, &(change_list[0]), change_list.size(), event_list, NUMBER_OF_EVENT, 0);

        if (events == ERROR)
            throw (strerror(errno));
        change_list.clear();
        processEvents(events);
    }
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
        }
        else if (event->filter == EVFILT_WRITE && servers.find(event->ident) != servers.end()) { // status 확인
            // status에 대한 처리
            // status == 요청 다 읽음 -> Request parser -> Request
            //servers[event.ident].clearContent();
			if (parser.getReadingStatus(event->ident) == END) {
				HttpRequestMessage msg = parser.getHttpRequestMessage(event->ident);
				
				std::cout << msg.getStatusCode() << std::endl;
				parser.clear(event->ident);
			}
            processWriteEvent(*event);
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

    if (connection_socket == ERROR)
        throw (strerror(errno));
    change_list.push_back(makeEvent(connection_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL));
    change_list.push_back(makeEvent(connection_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL));
	servers[connection_socket] = Server(connection_socket, event.ident);
}

void ServerManager::processReadEvent(const struct kevent& event) {
    int n;
    char buff[BUFFER_SIZE + 1];

    memset(buff, 0, sizeof(buff));
    n = read(event.ident, &buff, BUFFER_SIZE);
    if (n < 1) {
        disconnectWithClient(event);
        return;
    }
    buff[n] = 0;
	parser.run(event.ident, buff);
    // servers[event.ident].appendContent(buff);
    // servers[event.ident].checkContent();
}

void ServerManager::processWriteEvent(const struct kevent& event) {
    int n;
    std::string response_content;

    // Request -> Response -> String
    //write(event.ident, response_content, response_content.size());
    if (n == ERROR) {
        disconnectWithClient(event);
        return;
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
