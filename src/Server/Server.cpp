#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/ErrorPage.hpp"
#include "../../inc/Resource.hpp"

Server::Server() {}

Server::Server(const int listen_socket) {
    this->listen_socket = listen_socket;
    response.clear();
    bytes_to_send = 0;
	bytes_sent = 0;
    request_body.clear();
    bytes_to_write = 0;
    bytes_written = 0;
}

int Server::getListenSocket() const {
    return listen_socket;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
    request_body = request_message.getMessageBody();
    bytes_to_write = 0;
    bytes_written = request_message.getMessageBody().length();
}

void Server::setResponse(std::string http_response) {
    response = http_response;
    bytes_sent = 0;
    bytes_to_send = http_response.length();
    return;
}

std::string Server::makeResponse(const Config* config) {
    try {
		if (request.getStatusCode()) {
			throw (request.getStatusCode());
		}
		Method *method = Method::generate(request.getMethod(), &request, config);
		method->checkAllowed(request.getMethod());
        std::string response_message = method->makeHttpResponseMessage().toString();
        delete method;
        return response_message;
    }
    catch (const int status_code) {
        return ErrorPage::makeErrorPageResponse(status_code, config).toString();
    }
    return HttpResponseMessage().toString();
}

void Server::clearResponse() {
    response.clear();
    bytes_sent = 0;
    bytes_to_send = 0;
}

bool Server::isSendComplete() {
    return bytes_sent == bytes_to_send;
}

void Server::updateByteSend(ssize_t new_bytes_sent) {
    this->bytes_sent += static_cast<size_t>(new_bytes_sent);
}

std::string Server::getResponse() {
    return response;
}

void Server::updateResponse(ssize_t new_bytes_sent) {
    updateByteSend(new_bytes_sent);
    response = response.substr(new_bytes_sent);
}

void Server::appendResponse(const char* buffer, size_t size) {
    bytes_to_send += size;
	response.append(buffer, size);
}

HttpRequestMessage* Server::getRequestPtr() {
    return &this->request;
}

void Server::updateBytesWritten(ssize_t new_bytes_written) {
    this->bytes_written += static_cast<size_t>(new_bytes_written);
}

void Server::updateRequestBody(ssize_t new_bytes_written) {
    updateBytesWritten(new_bytes_written);
    request_body = request_body.substr(new_bytes_written);
}

bool Server::writeComplete() {
    return bytes_written == bytes_to_write;
}

void Server::clearRequestBody() {
    request_body.clear();
    bytes_to_write = 0;
    bytes_written = 0;
}