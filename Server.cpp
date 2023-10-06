
#include "Server.hpp"

// Server::Server() {}

    /**
     * 1. server 멤버 값 넗기
     * 2. location block 생성
     *
     */

Server::Server(std::istringstream& server_block) {
	std::string line;

	//server block item read
	while (std::getline(server_block, line)) {
		std::stringstream one_line(line);
		std::string key;
		one_line >> key;
		// std::cout << key << " | " << one_line << std::endl;
		if (key == "location")
			break;
		server_token_parser(key, one_line);
	}
	//location block read
	while (std::getline(server_block, line)) {
		std::stringstream one_line(line);
		std::string checker;
		one_line >> checker;
		if (checker == "location") {
			std::string loc_val;
			std::string loc_key;
			one_line >> loc_key;
			while (std::getline(server_block, line)) {
				if (line == "}")
					break;
				loc_val.append(line + "\n");
			}
			std::stringstream loc_line;
			// Location unit_loc(loc_val);
			// locations.insert(make_pair(loc_key, unit_loc));
			std::cout << loc_val << std::endl;
		}

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

// Server::~Server() {}


void Server::server_token_parser(std::string key, std::stringstream& one_line) {
	// std::cout << key << " | " << one_line << std::endl;
	if (key == "listen") {
		int value;
		one_line >> value;
		if (!value)
			return ;
		port = value;
	} else if (key == "host") {
		std::string value;
		one_line >> value;
		if (value == "")
			return ;
		host = value;
	} else if (key == "server_name") {
		std::string value;
		one_line >> value;
		if (value == "")
			return ;
		name = value;
	} else if (key == "error_page") {
		int value1;
		std::string value2;
		one_line >> value1;
		if (value1 || one_line.peek())
			return;
		one_line >> value2;
		if (value2 == "")
			return;
		error_page.insert(std::pair<int, std::string>(value1, value2));
	} else if (key == "root") {
		std::string value;
		one_line >> value;
		root = value;
	} else if (key == "index") {
		std::string value;
		one_line >> value;
		index.push_back(value);
	} else if (key == "limit_body_size") {
		long value;
		one_line >> value;
		limit_body_size = value;
	} else {
		throw std::invalid_argument("Error: invalid server key\n");
	}
}

