#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <vector>
#include <map>
#include <sys/event.h>
#include "Config.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "EventType.hpp"

#define NUMBER_OF_EVENT 100
#define NUMBER_OF_BACKLOG 5
#define BUFFER_SIZE 10000
#define TIMEOUT_SEC 30

class ServerManager {
	private:
		int kq;
		std::vector<struct kevent> change_list;
		struct kevent event_list[NUMBER_OF_EVENT];

		std::set<int> listen_sockets;
		const Config* default_config;
		std::map<std::string, std::vector<const Config*> > server_name_to_config;
		std::map<int, Server> servers;
		RequestParser parser;
		
		ServerManager();

		void setConfigByServerName(const std::vector<Config>* configs);
		int openListenSocket(const int port) const;
		void addListenEvent(void);
		struct kevent makeEvent( 
			uintptr_t ident,
			int16_t filter,
			uint16_t flags,
			uint32_t fflags,
			intptr_t data,
			void* udata) const;
		void processEvents(const int events);
		void processEvent(const struct kevent* event);
		const Config* findConfig(const std::string host, const std::string url);
		void checkEventError(const struct kevent& event);
		void processListenEvent(const struct kevent& event);
		void processReceiveEvent(const struct kevent& event);
		void processSendEvent(const struct kevent& event);
		void disconnectWithClient(const struct kevent& event);
		void handleError(const int return_value, const int listen_socket) const;

        void processPipeWriteEvent(const struct kevent &event);
        void processPipeReadEvent(const struct kevent& event);

        EventType getEventType(const struct kevent* event);
        void assignParsedRequest(const struct kevent* event);
        void processCgiOrMakeResponse(const struct kevent* event);

	public:
		ServerManager(const std::vector<Config>* configs);
		// ServerManager(const ServerManager& other);
		// ServerManager operator=(const ServerManager& other);
		~ServerManager();

		void run();
};

#endif