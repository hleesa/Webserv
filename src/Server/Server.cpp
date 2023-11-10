#include "../../inc/Server.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

std::vector<std::string> Server::getRequestLine() const {
    return request.getRequestLine();
}

void Server::setRequest(HttpRequestMessage request) {
    this->request = request;
}
