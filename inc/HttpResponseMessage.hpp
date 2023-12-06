
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
    size_t bytes_to_send;
    size_t bytes_sent;

public:
	HttpResponseMessage();
    HttpResponseMessage(int status_code, std::map<std::string, std::string> header_fields,
                        std::string message_body);
    HttpResponseMessage &operator=(const HttpResponseMessage &other);
    ~HttpResponseMessage();

    std::string toString();
    std::string& getResponseRef();

    void* getSendBuffer();
    void updateBytesSent(ssize_t new_bytes_sent);
    bool sendComplete() const;
    void clear();
    size_t getBytesToSend() const;

    void append(const char* buffer, size_t size);
};

#endif //WEBSERV_HTTPRESPONSEMESSAGE_HPP
