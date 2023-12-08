#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"

class Server {
private:
    int port;
    HttpRequestMessage request;
    HttpResponseMessage response;

public:
    Server();
    Server(const int port);
    Server& operator=(const Server& other);
    ~Server();

    void setRequest(const HttpRequestMessage& msg);
    void setResponse(const HttpResponseMessage& new_response);
    std::string getResponseStr();

    HttpResponseMessage makeResponse(const Config* config);
    HttpRequestMessage* getRequestPtr();
    HttpResponseMessage* getResponsePtr();
    int getPort() const;

};

#endif