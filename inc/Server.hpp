#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"

class Server {
private:
    int listen_socket;
    HttpRequestMessage request;
    std::string response;
    size_t bytes_to_send;
    size_t bytes_sent;

    void updateByteSend(ssize_t new_bytes_sent);

    size_t bytes_to_write;
    size_t bytes_written;
    char* message_body_ptr;
    void updateBytesToWrite(ssize_t new_bytes_written);

public:
    Server();
    Server(const int listen_socket);

    int getListenSocket() const;
    void setRequest(const HttpRequestMessage& msg);
    void setResponse(std::string http_response);

    std::string getResponse();
    void updateResponse(ssize_t bytes_sent);
    bool isSendComplete();
    void clearResponse();

    std::string makeResponse(const Config* config);

    void appendResponse(const char* buffer, size_t size);
    HttpRequestMessage* getRequestPtr();

    void updateRequestBody(ssize_t bytes_written);
    bool writeComplete();
    void clearRequestBody();

    char* getMessageBodyPtr() const;
    size_t getBytesToWrite();


};

#endif