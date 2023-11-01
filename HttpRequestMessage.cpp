
#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body) {
    request_line = request_line;
    header_fields = header_fields;
    message_body = message_body;
}