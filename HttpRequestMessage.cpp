
#include "HttpRequestMessage.hpp"

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
	
std::vector<std::string> HttpRequestMessage::getRequestLine() {
	return request_line;
}