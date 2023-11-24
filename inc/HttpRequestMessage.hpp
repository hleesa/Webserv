
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
	HttpRequestMessage();
    HttpRequestMessage(std::vector<std::string> request_line,
                       std::map<std::string, std::vector<std::string> > header_fields, std::string message_body,
                       int status_code);

    HttpRequestMessage &operator=(const HttpRequestMessage &other);

	std::string getMethod() const;
	std::string getURL() const;
	std::vector<std::string> getRequestLine() const;
	std::map<std::string, std::vector<std::string> > getHeaderFields() const;
	std::string getMessageBody() const;
	int getStatusCode() const;
    std::string getHost(); 
};


#endif //WEBSERV_HTTPREQUESTMESSAGE_HPP
