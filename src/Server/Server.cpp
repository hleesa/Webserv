#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/ErrorPage.hpp"
#include "../../inc/Resource.hpp"

Server::Server() : listen_socket(-1), bytes_to_send(0),
                   bytes_sent(0), bytes_to_write(0), bytes_written(0) {
}

Server::Server(const int listen_socket) : listen_socket(listen_socket), bytes_to_send(0),
                                          bytes_sent(0), bytes_to_write(0), bytes_written(0) {
//    response.clear();
}

Server& Server::operator=(const Server& other) {
    if (&other != this) {
        listen_socket = other.listen_socket;
        request = other.request;
        response = other.response;

        bytes_to_send = other.bytes_to_send;
        bytes_sent = other.bytes_sent;

        bytes_to_write = other.bytes_to_write;
        bytes_written = other.bytes_written;
    }
    return *this;
}

Server::~Server() {
    std::cout << "~Server()\n";
}

int Server::getListenSocket() const {
    return listen_socket;
}
void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
    bytes_to_write = request_message.getBodySize();
    bytes_written = 0;
}

void Server::setResponse(HttpResponseMessage http_response) {
    response = http_response;
    bytes_sent = 0;
    bytes_to_send = http_response.getResponseRef().length();
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

void Server::updateBytesSent(ssize_t new_bytes_sent) {
    bytes_to_send -= static_cast<size_t>(new_bytes_sent);
    bytes_sent += static_cast<size_t>(new_bytes_sent);
}

bool Server::sendComplete() {
    return bytes_to_send == 0;
}

void Server::clearResponse() {
    bytes_sent = 0;
    bytes_to_send = 0;
}

unsigned char* Server::getResponsePtr() const{
    return response.getResponsePtr() + bytes_sent;
}

size_t Server::getBytesToSend() {
    return bytes_to_send;
}

void Server::appendResponse(const char* buffer, size_t size) {
    bytes_to_send += size;
    response.getResponseRef().append(buffer, size);
}

HttpRequestMessage* Server::getRequestPtr() {
    return &this->request;
}

void Server::updateBytesWritten(ssize_t new_bytes_written) {
    bytes_to_write -= static_cast<size_t>(new_bytes_written);
    bytes_written  += static_cast<size_t>(new_bytes_written);
}

bool Server::writeComplete() {
    return bytes_to_write == 0;
}

void Server::clearRequestBodyPtr() {
    bytes_to_write = 0;
    bytes_written = 0;
}

unsigned char* Server::getMessageBodyPtr() const {
    return request.getMessageBodyPtr() + bytes_written;
}

size_t Server::getBytesToWrite() {
    return bytes_to_write;
}

std::string Server::getResponse() {
    return response.getResponseRef();
}
