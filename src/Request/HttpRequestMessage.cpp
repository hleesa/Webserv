
#include "../../inc/HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage() : status_code(0), body_size(0), request_body(NULL) {
}

HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body, int status_code) :
        request_line(request_line), header_fields(header_fields), status_code(status_code), body_size(0) {

    if (!message_body.empty())
        this->body_size = message_body.length();
    this->request_body = new unsigned char[this->body_size];
    std::memmove(this->request_body, message_body.c_str(), body_size);
}

#include <iostream>
HttpRequestMessage::~HttpRequestMessage() {
    std::cout << "~HttpRequestMessage()\n";
    if (request_body != NULL)
        delete[] request_body;
}

HttpRequestMessage &HttpRequestMessage::operator=(const HttpRequestMessage &other) {
    if (&other != this) {
        request_line = other.request_line;
        header_fields = other.header_fields;
        status_code = other.status_code;
        body_size = other.body_size;
        if (request_body != NULL) {
            delete[] request_body;
            request_body = NULL;
        }
        if (other.request_body != NULL) {
            request_body = new unsigned char[other.getBodySize()];
            std::memmove(request_body, other.getMessageBodyPtr(), other.getBodySize());
        }
    }
    return *this;
}
	
std::string HttpRequestMessage::getMethod() const {
	return this->request_line[0];
}

std::string HttpRequestMessage::getURL() const {
	return this->request_line[1];
}

std::vector<std::string> HttpRequestMessage::getRequestLine() const {
	return this->request_line;
}

std::map<std::string, std::vector<std::string> > HttpRequestMessage::getHeaderFields() const {
	return this->header_fields;
}

int HttpRequestMessage::getStatusCode() const {
	return status_code;
}

std::string HttpRequestMessage::getHost() {
    return *this->header_fields["host"].begin();
}

unsigned char* HttpRequestMessage::getMessageBodyPtr() const {
    return this->request_body;
}

size_t HttpRequestMessage::getBodySize() const {
    return this->body_size;
}