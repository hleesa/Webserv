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
    HttpResponseMessage response;

    size_t bytes_to_write;
    size_t bytes_written;

public:
    Server();
    Server(const int listen_socket);
    Server& operator=(const Server& other);
    ~Server();

    int getListenSocket() const;
    void setRequest(const HttpRequestMessage& msg);
    void setResponse(HttpResponseMessage http_response);
    std::string getResponseStr();
//    HttpResponseMessage getResponse();
    void updateBytesSent(ssize_t bytes_sent);
    bool sendComplete();
    void clearResponse();

    HttpResponseMessage makeResponse(const Config* config);

    void appendResponse(const char* buffer, size_t size);
    HttpRequestMessage* getRequestPtr();

    void updateBytesWritten(ssize_t bytes_written);
    bool writeComplete();
    void clearRequestBodyPtr();

    unsigned char* getMessageBodyPtr() const;
    size_t getBytesToWrite();

    HttpResponseMessage* getResponsePtr();
    size_t getBytesToSend();

};

#endif