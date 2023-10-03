
#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

#include <string>
#include <set>
#include <vector>

class Location {

protected:
    std::set<std::string> http_methods;
    std::set<std::string> http_redirection;
    std::string root;
    std::vector<std::string> index;
    bool autoindex;

public:

    Location();

    Location(const std::string &location_block);

};


#endif //WEBSERV_LOCATION_HPP
