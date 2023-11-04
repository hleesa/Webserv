#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <vector>
#include <map>
#include <sys/event.h>
#include "Config.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"

#define NUMBER_OF_EVENT 8
#define NUMBER_OF_BACKLOG 5
#define ERROR -1
#define BUFFER_SIZE 10000

class ServerManager {
	private:
		int kq;
		std::vector<struct kevent> change_list;
		struct kevent event_list[NUMBER_OF_EVENT];
		std::map<int, Config> configs;
		std::map<int, Server> servers;
		RequestParser parser;
		
		ServerManager();

		int openListenSocket(const int port) const;
		void addListenEvent();
		struct kevent makeEvent( 
			uintptr_t ident,
			int16_t filter,
			uint16_t flags,
			uint32_t fflags,
			intptr_t data,
			void* udata) const;
		void processEvents(const int events);
		void checkEventError(const struct kevent& event);
		void processListenEvent(const struct kevent& event);
		void processReadEvent(const struct kevent& event);
		void processWriteEvent(const struct kevent& event);
		void disconnectWithClient(const struct kevent& event);

	public:
		ServerManager(const std::vector<Config>& configs);
		// ServerManager(const ServerManager& other);
		// ServerManager operator=(const ServerManager& other);
		~ServerManager();

		void run();
};

#endif