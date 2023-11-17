
#ifndef WEBSERV_HTTPRESPONSEMESSAGE_HPP
#define WEBSERV_HTTPRESPONSEMESSAGE_HPP

#include <string>
#include <vector>
#include <map>

class HttpResponseMessage {

private:
    int status_code;
    std::map<std::string, std::string > header_fields;
    std::string message_body;

public:
	HttpResponseMessage();
    HttpResponseMessage(int status_code, std::map<std::string, std::string> header_fields,
                        std::string message_body);

    std::string toString();
  
};

#endif //WEBSERV_HTTPRESPONSEMESSAGE_HPP
