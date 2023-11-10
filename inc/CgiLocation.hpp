
#ifndef WEBSERV_CGILOCATION_HPP
#define WEBSERV_CGILOCATION_HPP

#include "Location.hpp"

class CgiLocation : public Location {
private:
    std::string cgi_path;
    std::string cgi_ext;
public:
    std::string getCgiPath() const;
    std::string getCgiExt() const;
};

#endif //WEBSERV_CGILOCATION_HPP
