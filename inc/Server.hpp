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

public:
    Server();
    Server(const int listen_socket);
    Server& operator=(const Server& other);
    ~Server();

    void setRequest(const HttpRequestMessage& msg);
    void setResponse(const HttpResponseMessage& new_response);
    std::string getResponseStr();

    HttpResponseMessage makeResponse(const Config* config);

    int getListenSocket();
    HttpRequestMessage* getRequestPtr();
    HttpResponseMessage* getResponsePtr();

};

#endif