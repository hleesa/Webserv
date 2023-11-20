#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"

#include "Config.hpp"

class Server {
	private:
		int connection_socket;
		int listen_socket;
		HttpRequestMessage request;
        char* response;
        size_t response_length;
        ssize_t bytes_send;

    public:
		Server();
        Server(int connection, int listen);

		int getListenSocket() const;
		void setRequest(const HttpRequestMessage& msg);
        void setResponse(std::string response_str);
        ssize_t getBytesSend();
        size_t getResponseLength();
        char *getResponse();
        void setBytesSend(ssize_t bytes_send);
//        void GResponseLength(size_t response_size);
		std::string makeResponse(std::map<int, Config>& configs);
};

#endif