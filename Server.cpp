
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

	while (std::getline(server_block, line)) {
		std::stringstream one_line(line);
		one_line >> key;
		server_token_parser(key, one_line);
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
        // this->cgi_location = other.cgi_location;
    }
	return ;
}

Server::~Server() {}


void Server::server_token_parser(std::string key, std::stringstream one_line) {
	
	if (key == "listen") {
		int value;
		one_line >> value;
		if (!value)
			return ;
		this->port = value;
	} else if (key == "host") {
		std::string value;
		one_line >> value;
		if (value == "")
			return ;
		this->host = value;
	} else if (key == "name") {
		std::string value;
		one_line >> value;
		if (value == "")
			return ;
		this->name = value;
	} else if (key == "error_page") {
		int value1;
		std::string value2;
		one_line >> value1;
		if (value1 || one_line.peek())
			return;
		one_line >> value2;
		if (value2 == "")
			return;
		this->error_page.first = value1;
		this->error_page.second = value2;
	} else if (key == "root") {
		std::string value;
		one_line >> value;

	} else if (key == "index") {

	} else if (key == "limit_body_size") {

	}

}

