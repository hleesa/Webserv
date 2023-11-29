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
    size_t bytes_response;
    size_t bytes_sent;

    std::string response_body;


    void updateByteSend(ssize_t new_bytes_sent);

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

    void updateResponseBody(const char* buffer, size_t size);
    HttpRequestMessage* getRequestPtr();
    size_t bytes_write;

};;

#endif