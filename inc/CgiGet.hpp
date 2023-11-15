
#ifndef WEBSERV_CGIGET_HPP
#define WEBSERV_CGIGET_HPP

#include "Config.hpp"
#include "HttpResponseMessage.hpp"
#include "HttpRequestMessage.hpp"
#include <vector>
#include <string>

class CgiGet {

public:
    static bool isValidCgiGetUrl(const std::vector<std::string>& request_line, const std::map<int, Config>& configs,
                                 int listen_socket);

    static HttpResponseMessage
    processCgiGet(HttpRequestMessage request, const std::map<int, Config>& configs,
                  int listen_socket);

};

bool isVisibleString(const std::string& str);

#endif //WEBSERV_CGIGET_HPP
