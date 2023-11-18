#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
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

int Server::getListenSocket() const {
	return this->listen_socket;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
	Method *method = Method::generate(request.getMethod(), &request, &configs[listen_socket]);
    try {
		if (request.getStatusCode()) {
			throw(request.getStatusCode());
		}
        // if (request.getMethod() == "POST") {
        //     Post method;
        //     return method.run(request, configs[listen_socket]).toString();
        // }
        if (request.getMethod() == "GET" && request.getURL().find("cgi") == std::string::npos) {
            return method->makeHttpResponseMessage().toString();
        }
        // if (request.getMethod() == "DELETE") {
        //     Delete method(request, configs[listen_socket]);
        //     return method.makeHttpResponseMessage().toString();
        // }
        // if (request.getMethod() == "GET" && request.getURL().find("cgi") != std::string::npos) {
        //     return CgiGet::processCgiGet(request, configs[listen_socket]).toString();
        // }
    }
    catch (const int status_code) {
        return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
    }
    return HttpResponseMessage().toString();
}
