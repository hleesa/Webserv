#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/GetCgi.hpp"
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

void Server::setResponse(std::string http_response) {
    response = http_response;
    bytes_sent = 0;
    bytes_response = http_response.length();
    return;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
    try {
		if (request.getStatusCode()) {
			throw(request.getStatusCode());
		}
        
		Method *method = Method::generate(request.getMethod(), &request, &configs[listen_socket]);
		method->checkAllowed(request.getMethod());
        std::string response_message = method->makeHttpResponseMessage().toString();
        delete method;
        return response_message;
    }
    catch (const int status_code) {
        return ErrorPage::makeErrorPageResponse(status_code, &configs[listen_socket]).toString();
    }
    return HttpResponseMessage().toString();
}

void Server::clearResponse() {
    bytes_sent = 0;
    bytes_response = 0;
}

bool Server::isSendComplete() {
    return bytes_sent == bytes_response;
}

void Server::updateByteSend(ssize_t new_bytes_sent) {
    this->bytes_sent += static_cast<size_t>(new_bytes_sent);
}

std::string Server::getResponse() {
    return response;
}

void Server::updateResponse(ssize_t new_bytes_sent) {
    updateByteSend(new_bytes_sent);
    response.substr(new_bytes_sent);
}
