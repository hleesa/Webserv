
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
    std::string response;
    size_t response_size;
    unsigned char* response_ptr;

public:
	HttpResponseMessage();
    HttpResponseMessage(int status_code, std::map<std::string, std::string> header_fields,
                        std::string message_body);

    ~HttpResponseMessage();

    HttpResponseMessage &operator=(const HttpResponseMessage &other);
    std::string toString();
    unsigned char* getResponsePtr() const;
    std::string& getResponseRef();
    std::string getResponse();
  
};

#endif //WEBSERV_HTTPRESPONSEMESSAGE_HPP
