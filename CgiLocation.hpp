
#ifndef WEBSERV_CGILOCATION_HPP
#define WEBSERV_CGILOCATION_HPP

#include "Location.hpp"

class CgiLocation : public Location {
private:
    std::vector<std::string> cgi_path;
    std::vector<std::string> cgi_ext;
};


#endif //WEBSERV_CGILOCATION_HPP
