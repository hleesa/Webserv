
#include "HttpRequestMessage.hpp"

HttpRequestMessage::HttpRequestMessage(std::vector<std::string> request_line,
                                       std::map<std::string, std::vector<std::string> > header_fields,
                                       std::string message_body, int status_code){
    request_line = request_line;
    header_fields = header_fields;
    message_body = message_body;
    status_code = status_code;
}
	
std::vector<std::string> HttpRequestMessage::getRequestLine() {
	return request_line;
}