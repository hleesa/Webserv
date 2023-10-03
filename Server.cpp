
#include "Server.hpp"

Server::Server() {}

    /**
     * 1. server 멤버 값 넗기
     * 2. location block 생성
     *
     */

Server::Server(std::istringstream& server_block) {
	std::string line;
	std::string key;
	std::string value;

	while (std::getline(server_block, line)) {
		std::stringstream one_line(line);
		one_line >> key;
		if (key == "location")
			break;

	}

}

Server::Server(const Server& other) {
    if (this != &other) {
        this->port = other.port;
        this->host = other.host;
        this->name = other.name;
        this->error_page = other.error_page;
        this->root = other.root;
        this->index = other.index;
        this->limit_body_size = other.limit_body_size;
        this->locations = other.locations;
        this->cgi_location = other.cgi_location;
    }
	return ;
}

Server::~Server() {}


