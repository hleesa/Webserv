#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "HttpRequestMessage.hpp"

class Server {
	private:
		int connection_socket;
		int listen_socket;
		HttpRequestMessage request;

    public:
		Server();
        Server(int connection, int listen);
        std::vector<std::string> getRequestLine() const;
        void setRequest(HttpRequestMessage request);
};

#endif