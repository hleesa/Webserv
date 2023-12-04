
#include "../../inc/HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage() : message_body(NULL) {
}


HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body, int status_code){
    this->request_line = request_line;
    this->header_fields = header_fields;
    this->status_code = status_code;
    this->body_size = message_body.length();
    this->message_body = new char[this->body_size + 1];
    std::strcpy(this->message_body, message_body.c_str());
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
        if (other.message_body) {
            message_body = new char[other.getBodySize()];
            std::strcpy(message_body, other.getMessageBodyPtr());
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

char* HttpRequestMessage::getMessageBodyPtr() const {
    return this->message_body;
}

size_t HttpRequestMessage::getBodySize() const {
    return this->body_size;
}