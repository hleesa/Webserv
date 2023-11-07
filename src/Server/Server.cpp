#include "../../inc/Server.hpp"
#include "../../inc/GetMethod.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
	this->request = request_message;
}

HttpResponseMessage Server::makeResponse(std::map<int, Config>& configs) {
	if (request.getMethod() == "GET")
		return GetMethod::run(request, configs[listen_socket]);
	return HttpResponseMessage();
}
