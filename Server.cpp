
#include "Server.hpp"
#include "Location.hpp"

// Server::Server() {}

    /**
     * 1. server 멤버 값 넗기
     * 2. location block 생성
     *
     */

Server::Server(std::vector< std::vector< std::string> >& server_block)
	:	port(80), host(""), root("html"), limit_body_size(1e6)
{
	std::set<std::string> duplicated;
	int i = 0;

	for (;i < server_block.size(); i++) {
		if (server_block[i][0] == "location") {
			break;
		} else if (duplicated.find(server_block[i][0]) == duplicated.end()) {
			throw std::invalid_argument("Error: duplicated argument\n");
		} else {
			server_token_parser(server_block[i], duplicated);
		}
	}
	for (; i < server_block.size(); i++) {
		std::string key;
		std::vector< std::vector<std::string> > loc_block;
		if (server_block[i][0] == "location") {
			key = server_block[i][1];
			for (; server_block[i][0] != "}"; ++i) {
				loc_block.push_back(server_block[i]);
			}
			// Location unit_loc(loc_block);
			// locations.insert(make_pair(key, unit_loc));

		}
	}
}


//keep
	// std::string line;
	// std::string d_loc_key;
	// std::set<std::string> duplicated;

	// //server block item read
	// while (std::getline(server_block, line)) {
	// 	std::stringstream one_line(line);
	// 	std::string key;
	// 	one_line >> key;
	// 	// std::cout << key << " | " << one_line << std::endl;
	// 	if (key == "location") {
	// 		one_line >> d_loc_key;
	// 		break;
	// 	} else if (duplicated.find(key) == duplicated.end()) {
	// 		throw std::invalid_argument("Error: duplicated arguments\n");
	// 	}
	// 	server_token_parser(key, one_line, duplicated);
	// }
	// //first location block read
	// std::string d_loc_val;
	// while (std::getline(server_block, line)) {
	// 	if (line == "}")
	// 		break;
	// 	d_loc_val.append(line + "\n");
	// }
	// std::istringstream d_ss(d_loc_val);
	// Location d_unit_loc(d_ss);
	// locations.insert(make_pair(d_loc_key, d_unit_loc));
	// std::cout << "Location : " << d_loc_key << "\n" << d_unit_loc << std::endl;

	// //location block read
	// while (std::getline(server_block, line)) {
	// 	std::stringstream one_line(line);
	// 	std::string checker;
	// 	one_line >> checker;
	// 	if (checker == "location") {
	// 		std::string loc_val;
	// 		std::string loc_key;
	// 		one_line >> loc_key;
	// 		while (std::getline(server_block, line)) {
	// 			if (line == "}")
	// 				break;
	// 			loc_val.append(line + "\n");
	// 		}
	// 		std::istringstream ss(loc_val);
	// 		Location unit_loc(ss);
	// 		std::cout << "Location : " << loc_key << "\n" << unit_loc << std::endl;
	// 		locations.insert(make_pair(loc_key, unit_loc));
	// 	}




Server::Server(const Server& other) {
    if (this != &other) {
        port = other.port;
        host = other.host;
        name = other.name;
        error_page = other.error_page;
        root = other.root;
        index = other.index;
        limit_body_size = other.limit_body_size;
        locations = other.locations;
        // cgi_location = other.cgi_location;
    }
	return ;
}

// Server::~Server() {}


void Server::server_token_parser(std::vector<std::string> one_line, std::set<std::string>& duplicated) {
	// std::cout << key << " | " << one_line << std::endl;
	if (one_line.empty())
		throw std::invalid_argument("Error: invalid arguments\n");

	if (one_line[0] == "listen" || one_line.size() == 2) {
		std::stringstream ss(one_line[1]);
		int value;
		ss >> value;
		if (!value)
			return ;
		port = value;
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "host" || one_line.size() == 2) {
		std::string value;
		host = one_line[1];
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "server_name" || one_line.size() >= 2) {
		for (int i = 1; i < one_line.size(); i++) {
			name.push_back(one_line[i]);
		}

	} else if (one_line[0] == "error_page" || one_line.size() == 3) {
		std::stringstream ss(one_line[1]);
		int err_no;
		ss >> err_no;
		error_page.insert(std::pair<int, std::string>(err_no, one_line[2]));
	
	} else if (one_line[0] == "root" || one_line.size() == 2) {
		root = one_line[1];
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "index" || one_line.size() >= 2) {
		for (int i = 1; i < one_line.size(); i++) {
			index.push_back(one_line[i]);
		}

	} else if (one_line[0] == "limit_body_size" || one_line.size() == 2) {
		std::stringstream ss(one_line[1]);
		long value;
		ss >> value;
		limit_body_size = value;
		duplicated.insert(one_line[0]);

	} else {
		throw std::invalid_argument("Error: invalid server key\n");
	}
}

