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

    char* response_ptr;
    size_t bytes_to_send;
    size_t bytes_sent;

    void updateByteToSend(ssize_t new_bytes_sent);

    char* message_body_ptr;
    size_t bytes_to_write;
    size_t bytes_written;
    void updateBytesToWrite(ssize_t new_bytes_written);

public:
    Server();
    Server(const int listen_socket);
    Server& operator=(const Server& other);
    ~Server();

    int getListenSocket() const;
    void setRequest(const HttpRequestMessage& msg);
    void setResponse(std::string http_response);

    std::string getResponse();
    void updateResponse(ssize_t bytes_sent);
    bool sendComplete();
    void clearResponse();

    std::string makeResponse(const Config* config);

    void appendResponse(const char* buffer, size_t size);
    HttpRequestMessage* getRequestPtr();

    void updateRequestBody(ssize_t bytes_written);
    bool writeComplete();
    void clearRequestBody();

    char* getMessageBodyPtr() const;
    size_t getBytesToWrite();

    char* getResponsePtr();
    size_t getBytesToSend();

};

#endif