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

    public:
		Server();
        Server(int connection, int listen);

		void setRequest(const HttpRequestMessage& msg);
		std::string makeResponse(std::map<int, Config>& configs);
};

#endif