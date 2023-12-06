
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
    unsigned char* getResponsePtr() const;
    std::string& getResponseRef();
    std::string getResponse();

    void* getSendBuffer();
    void updateBytesSent(ssize_t new_bytes_sent);
    bool sendComplete() const;
    void clear();
    size_t getBytesSent() const;
    size_t getBytesToSend() const;

};

#endif //WEBSERV_HTTPRESPONSEMESSAGE_HPP
