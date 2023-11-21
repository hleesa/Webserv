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
    response = NULL;
    bytes_sent = 0;
    bytes_response = 0;
}

int Server::getListenSocket() const {
	return this->listen_socket;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
}

void Server::setResponse(std::string http_response) {
    this->httpResponse = http_response; // debug
    char *response_message = new char[http_response.length() + 1];
    strcpy(response_message, http_response.c_str());
    this->response = response_message;
    this->bytes_response = http_response.length() + 1;
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
        return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
    }
    return HttpResponseMessage().toString();
}

char* Server::getBuffer() {
    return response + bytes_sent;
}

size_t Server::getSendBytes() {
    return bytes_response - bytes_sent;
}

void Server::updateByteSend(ssize_t new_bytes_sent) {
    this->bytes_sent += static_cast<size_t>(new_bytes_sent);
}

bool Server::isSendComplete() {
    return bytes_sent == bytes_response;
}

void Server::clearResponse() {
    delete response;
    response = NULL;
    bytes_sent = 0;
    bytes_response = 0;
}
