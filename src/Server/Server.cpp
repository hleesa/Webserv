#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/ErrorPage.hpp"
#include "../../inc/Resource.hpp"

Server::Server() : listen_socket(-1), response_ptr(NULL), bytes_to_send(0),
                   bytes_sent(0), message_body_ptr(NULL), bytes_to_write(0), bytes_written(0) {
}

Server::Server(const int listen_socket) : listen_socket(listen_socket), response_ptr(NULL), bytes_to_send(0),
                                          bytes_sent(0), message_body_ptr(NULL), bytes_to_write(0), bytes_written(0) {
//    response.clear();
}

Server& Server::operator=(const Server& other) {
    if (&other != this) {
        listen_socket = other.listen_socket;
        request = other.request;
        response = other.response;

        if (response_ptr != NULL) {
            delete[] response_ptr;
            response_ptr = NULL;
        }
        if (other.response_ptr != NULL) {
            response_ptr = new char[other.bytes_to_send + 1];
            strcpy(response_ptr, other.response_ptr);
        }

        bytes_to_send = other.bytes_to_send;
        bytes_sent = other.bytes_sent;

        if (message_body_ptr != NULL) {
            delete[] message_body_ptr;
            message_body_ptr = NULL;
        }
        if (other.message_body_ptr != NULL) {
            message_body_ptr = new char[other.bytes_to_write + 1];
            strcpy(message_body_ptr, other.message_body_ptr);
        }

        bytes_to_write = other.bytes_to_write;
        bytes_written = other.bytes_written;
    }
    return *this;
}

Server::~Server() {
    if (response_ptr != NULL) {
        delete[] response_ptr;
        response_ptr = NULL;
    }
    if (message_body_ptr != NULL) {
        delete[] message_body_ptr;
        message_body_ptr = NULL;
    }
}

int Server::getListenSocket() const {
    return listen_socket;
}
void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
    bytes_to_write = request_message.getBodySize();
    bytes_written = 0;
    message_body_ptr = new char[bytes_to_write + 1];
    strcpy(message_body_ptr, request_message.getMessageBodyPtr());
}

void Server::setResponse(std::string http_response) {
    response = http_response;
    bytes_sent = 0;
    if (!http_response.empty())
        bytes_to_send = http_response.length();
    response_ptr = new char[bytes_to_send + 1];
    std::strcpy(response_ptr, http_response.c_str());
//    response_ptr[bytes_to_send] = '\0';
    return;
}

std::string Server::makeResponse(const Config* config) {
    Method *method = Method::generate(request.getMethod(), &request, config);
    try {
		if (request.getStatusCode()) {
            delete method;
			throw (request.getStatusCode());
		}
		method->checkAllowed(request.getMethod());
        std::string response_message = method->makeHttpResponseMessage().toString();
        delete method;
        return response_message;
    }
    catch (const int status_code) {
        delete method;
        return ErrorPage::makeErrorPageResponse(status_code, config).toString();
    }
    return HttpResponseMessage().toString();
}

void Server::updateBytesSent(ssize_t new_bytes_sent) {
    bytes_to_send -= static_cast<size_t>(new_bytes_sent);
    bytes_sent += static_cast<size_t>(new_bytes_sent);
}

bool Server::sendComplete() {
    return bytes_to_send == 0;
}

void Server::clearResponse() {
    response.clear();
    if (response_ptr != NULL) {
        delete response_ptr;
    }
    response_ptr = NULL;
    if (message_body_ptr != NULL) {
        delete message_body_ptr;
    }
    message_body_ptr = NULL;
    bytes_sent = 0;
    bytes_to_send = 0;
}

char* Server::getResponsePtr() {
    return response_ptr + bytes_sent;
}

size_t Server::getBytesToSend() {
    return bytes_to_send;
}

void Server::appendResponse(const char* buffer, size_t size) {
    bytes_to_send += size;
	response.append(buffer, size);
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
    delete[] message_body_ptr;
    message_body_ptr = NULL;
    bytes_to_write = 0;
    bytes_written = 0;
}

char* Server::getMessageBodyPtr() const {
    return message_body_ptr + bytes_written;
}

size_t Server::getBytesToWrite() {
    return bytes_to_write;
}

std::string Server::getResponse() {
    return response;
}

