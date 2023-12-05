#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include <vector>
#include <map>
#include <sys/event.h>
#include "Config.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "EventType.hpp"
#include "CgiData.hpp"

#define NUMBER_OF_EVENT 200
#define NUMBER_OF_BACKLOG 2000
#define BUFFER_SIZE 25000
#define TIMEOUT_SEC 30
#define ERROR -1

typedef struct kevent k_event;

class ServerManager {
	private:
		int kq;

		std::vector<k_event> change_list;
		k_event event_list[NUMBER_OF_EVENT];

		std::set<int> listen_sockets;
		const Config* default_config;
		std::map<std::string, std::vector<const Config*> > server_name_to_config;
		std::map<int, Server> servers;
		RequestParser parser;
		
		ServerManager();

		void setConfigByServerName(const std::vector<Config>* configs);
		int openListenSocket(const int port) const;
		void addListenEvent(void);
		k_event makeEvent(
			uintptr_t ident,
			int16_t filter,
			uint16_t flags,
			uint32_t fflags,
			intptr_t data,
			void* udata) const;

		const Config* findConfig(const std::string host, const std::string url);
        void handleError(const int return_value, const int listen_socket) const;

        void processEvents(const int events);
        void processEvent(const k_event* event);
        EventType getEventType(const k_event* event);
		void checkEventError(const k_event* event);
		void processListenEvent(const k_event* event);
		void processReceiveEvent(const k_event* event);
        void assignParsedRequest(const k_event* event);
        void processCgiOrMakeResponse(const k_event* event);
		void processSendEvent(const k_event* event);
        void processReadPipeEvent(const k_event* event);
        void processWritePipeEvent(const k_event* event);
		void disconnectWithClient(const k_event* event);
        void processTimeoutCgiEvent(const k_event* event);
        void processCgiEnd(const k_event* event);
        void processCgiTermination(CgiData* cgi_data);

	public:
		ServerManager(const std::vector<Config>* configs);
		// ServerManager(const ServerManager& other);
		// ServerManager operator=(const ServerManager& other);
		~ServerManager();

		void run();
};

#endif