#include "../../inc/Server.hpp"
#include "../../inc/CgiGet.hpp"
#include "../../inc/Get.hpp"
#include "../../inc/Post.hpp"
#include "../../inc/Delete.hpp"
#include "../../inc/ErrorPage.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
	this->request = request_message;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
	try {
		if (request.getMethod() == "POST") {
			Post method;
			return method.run(request, configs[listen_socket]).toString();
		}
		if (request.getMethod() == "GET" && request.getURL().find("cgi") == std::string::npos) {
			Get method(request, configs[listen_socket]);
			return method.makeHttpResponseMessage().toString();
		}
		if (request.getMethod() == "DELETE") {
			Delete method(request, configs[listen_socket]);
			return method.makeHttpResponseMessage().toString();
		}
	}
	catch (const int status_code) {
		return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
	}
    try {
        return CgiGet::processCgiGet(request, configs, listen_socket).toString();
    }catch (int status_code) {
        return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
    }
	return HttpResponseMessage().toString();
}
