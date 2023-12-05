
#include "../../inc/HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage() : message_body(NULL), body_size(0) {
}


HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body, int status_code) : body_size(0){
    this->request_line = request_line;
    this->header_fields = header_fields;
    this->status_code = status_code;
    if (!message_body.empty())
        this->body_size = message_body.length();
    this->message_body = new unsigned char[this->body_size];
    std::memmove(this->message_body, message_body.c_str(), body_size);
}

HttpRequestMessage::~HttpRequestMessage() {
    if (message_body != NULL)
        delete[] message_body;
}

HttpRequestMessage &HttpRequestMessage::operator=(const HttpRequestMessage &other) {
    if (&other != this) {
        request_line = other.request_line;
        header_fields = other.header_fields;
        status_code = other.status_code;
        body_size = other.body_size;
        if (message_body != NULL) {
            delete[] message_body;
            message_body = NULL;
        }
        if (other.message_body != NULL) {
            message_body = new unsigned char[other.getBodySize()];
            std::memmove(message_body, other.getMessageBodyPtr(), other.getBodySize());
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
    return this->message_body;
}

size_t HttpRequestMessage::getBodySize() const {
    return this->body_size;
}