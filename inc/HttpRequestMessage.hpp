
#ifndef WEBSERV_HTTPREQUESTMESSAGE_HPP
#define WEBSERV_HTTPREQUESTMESSAGE_HPP

#include <map>
#include <vector>
#include <string>

class HttpRequestMessage {
private:
    std::vector<std::string> request_line;
    std::map<std::string, std::vector<std::string> > header_fields;
    int status_code;
    size_t body_size;
    unsigned char* request_body;
    size_t bytes_to_write;
    size_t bytes_written;

public:
	HttpRequestMessage();
    HttpRequestMessage(std::vector<std::string> request_line,
                       std::map<std::string, std::vector<std::string> > header_fields, std::string message_body,
                       int status_code);
    HttpRequestMessage &operator=(const HttpRequestMessage &other);
    ~HttpRequestMessage();

	std::string getMethod() const;
	std::string getURL() const;
	std::vector<std::string> getRequestLine() const;
	std::map<std::string, std::vector<std::string> > getHeaderFields() const;
	int getStatusCode() const;
    std::string getHost();

    unsigned char* getMessageBodyPtr() const;
    size_t getBodySize() const;
    void updateBytesWritten(ssize_t new_bytes_written);
    bool writeComplete();
    void* getWriteBuffer();
    size_t getBytesToWrite();
};


#endif //WEBSERV_HTTPREQUESTMESSAGE_HPP
