
#include "../../inc/HttpResponseMessage.hpp"

#include <sstream>

HttpResponseMessage::HttpResponseMessage(int status_code,
                                         std::map<std::string, std::string> header_fields,
                                         std::string message_body) {
    this->status_code = status_code;
    this->header_fields = header_fields;
    this->message_body = message_body;
}

std::string HttpResponseMessage::toString(const std::map<int, std::string>& codeToReason) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << status_code << " " << codeToReason.at(status_code) << "\r\n";
    for (std::map<std::string, std::string>::const_iterator header = header_fields.begin(); header != header_fields.begin(); ++header) {
        ss << header->first << ": " << header->second << "\r\n";
    }
    ss << "\r\n" << message_body;
    return ss.str();
}

int HttpResponseMessage::getBodySize() {
    return message_body.size();
}