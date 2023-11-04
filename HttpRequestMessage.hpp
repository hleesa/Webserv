
#ifndef WEBSERV_HTTPREQUESTMESSAGE_HPP
#define WEBSERV_HTTPREQUESTMESSAGE_HPP

#include <map>
#include <vector>
#include <string>

class HttpRequestMessage {

private:
    std::vector<std::string> request_line;
    std::map<std::string, std::vector<std::string> > header_fields;
    std::string message_body;
    int status_code;

public:
    HttpRequestMessage(std::vector<std::string> request_line,
                       std::map<std::string, std::vector<std::string> > header_fields, std::string message_body,
                       int status_code);

    HttpRequestMessage &operator=(const HttpRequestMessage &other);

	std::vector<std::string> getRequestLine();
};


#endif //WEBSERV_HTTPREQUESTMESSAGE_HPP
