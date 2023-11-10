
#ifndef WEBSERV_CGIGET_HPP
#define WEBSERV_CGIGET_HPP

#include "Config.hpp"
#include "HttpResponseMessage.hpp"
#include <vector>
#include <string>

class CgiGet {

public:
    static bool isValidCgiGetUrl(const std::vector<std::string>& request_line, const std::map<int, Config>& configs,
                                 int con_socket);

    static HttpResponseMessage processCgiGet(const std::string url, CgiLocation cgi_location);

};

bool isVisibleString(const std::string& str);

#endif //WEBSERV_CGIGET_HPP
