#include "../../inc/Server.hpp"
#include "../../inc/Get.hpp"
#include "../../inc/Post.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
	this->request = request_message;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
	if (request.getMethod() == "POST") {
		Post method;
		std::cout << method.run(request, configs[listen_socket]).toString() << std::endl;
		return method.run(request, configs[listen_socket]).toString();
	}
	if (request.getMethod() == "GET") {
		Get method(request, configs[listen_socket]);
		return method.makeHttpResponseMessage().toString();
	}
	std::cout << "come here222" << std::endl;
	return HttpResponseMessage().toString();
}
