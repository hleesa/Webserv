#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/ErrorPage.hpp"
#include "../../inc/Resource.hpp"

Server::Server() : listen_socket(0) {
}

Server::Server(const int listen_socket) : listen_socket(listen_socket) {
}

Server& Server::operator=(const Server& other) {
    if (&other != this) {
        listen_socket = other.listen_socket;
        request = other.request;
        response = other.response;
    }
    return *this;
}

Server::~Server() {
}

int Server::getListenSocket() const {
    return listen_socket;
}
void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
}

void Server::setResponse(HttpResponseMessage http_response) {
    response = http_response;
    return;
}

HttpResponseMessage Server::makeResponse(const Config* config) {
    Method *method = Method::generate(request.getMethod(), &request, config);
    try {
		if (request.getStatusCode()) {
            delete method;
			throw (request.getStatusCode());
		}
		method->checkAllowed(request.getMethod());
        HttpResponseMessage response_message = method->makeHttpResponseMessage();
        delete method;
        return response_message;
    }
    catch (const int status_code) {
        delete method;
        return ErrorPage::makeErrorPageResponse(status_code, config);
    }
    return HttpResponseMessage();
}

HttpResponseMessage* Server::getResponsePtr() {
    return &response;
}

void Server::appendResponse(const char* buffer, size_t size) {
    response.getResponseRef().append(buffer, size);
}

HttpRequestMessage* Server::getRequestPtr() {
    return &this->request;
}

std::string Server::getResponseStr() {
    return response.getResponseRef();
}
