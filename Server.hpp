
#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP


#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "Location.hpp"
#include "CgiLocation.hpp"

class Server {

private:
    int port;
    std::string host;
    std::string name;
    std::map<int, std::string> error_page;
    std::string root;
    std::vector<std::string> index;
    long limit_body_size;
    std::map<std::string, Location> locations;
    CgiLocation cgi_location;

public:
    Server();

    Server(std::istringstream& server_block);

    Server(const Server& other);

    Server& operator=(const Server& other);

    ~Server();

};


#endif //WEBSERV_SERVER_HPP
