
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

public:
    HttpRequestMessage(std::vector<std::string> request_line,
                       std::map<std::string, std::vector<std::string> > header_fields, std::string message_body);

};


#endif //WEBSERV_HTTPREQUESTMESSAGE_HPP
