
#include "../../inc/HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage() {}

HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body, int status_code){
    this->request_line = request_line;
    this->header_fields = header_fields;
    this->message_body = message_body;
    this->status_code = status_code;
}

HttpRequestMessage &HttpRequestMessage::operator=(const HttpRequestMessage &other) {
    if (&other != this) {
        request_line = other.request_line;
        header_fields = other.header_fields;
        message_body = other.message_body;
        status_code = other.status_code;
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

std::string HttpRequestMessage::getMessageBody() const {
	return this->message_body;
}

int HttpRequestMessage::getStatusCode() const {
	return status_code;
}

std::string HttpRequestMessage::getHost() {
    return *this->header_fields["host"].begin();
}