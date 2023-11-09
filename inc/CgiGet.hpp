
#ifndef WEBSERV_CGIGET_HPP
#define WEBSERV_CGIGET_HPP

#include "Config.hpp"

#include <vector>
#include <string>

class CgiGet {

private:

public:
    static bool isValidCgiGetUrl(const std::vector<std::string>& request_line, const std::map<int, Config>& configs,
                                 int con_socket);

};


#endif //WEBSERV_CGIGET_HPP
