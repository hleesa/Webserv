
#include "../../inc/Config.hpp"
#include "../../inc/Location.hpp"

Config::Config() {}

Config::Config(std::vector< std::vector< std::string> >& server_block)
: port(80), host(""), root("./"), limit_body_size(100000000)
{
	std::set<std::string> duplicated;
	unsigned long i = 0;

	for (;i < server_block.size(); i++) {
		if (server_block[i][0] == "location") {
			break;
		} else if (duplicated.find(server_block[i][0]) != duplicated.end()) {
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
			i++;
			for (; server_block[i][0] != "}"; ++i) {
				loc_block.push_back(server_block[i]);
			}
			Location unit_loc(loc_block);
			locations.insert(make_pair(key, unit_loc));
		}
	}
	if (index.empty()) {
		index.push_back("index.html");
	}
	setDefaultErrorPage();
	// print_checker();
}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		port = other.port;
		host = other.host;
		name = other.name;
		error_page = other.error_page;
		root = other.root;
		index = other.index;
		limit_body_size = other.limit_body_size;
		locations = other.locations;
	}
	return (*this);
}

Config::~Config() {}

int Config::getPort() const {
	return this->port;
}

std::string Config::getHost() const {
	return this->host;	
}

std::vector<std::string> Config::getName() const {
	return this->name;
}

std::map<int, std::string> Config::getErrorpage() const {
	return this->error_page;
}

std::string Config::getRoot() const {
	return this->root;
}

std::vector<std::string> Config::getIndex() const {
	return this->index;
}

long Config::getLimitBodySize() const {
	return this->limit_body_size;
}

std::map<std::string, Location> Config::getLocations() const {
	return this->locations;
}

bool Config::hasLocationOf(const std::string url) const {
	if (locations.find(url) != locations.end()) {
		return true;
	}
	size_t pos = url.rfind('/');
	while (pos && pos != std::string::npos) {
		std::string key = url.substr(0, pos + 1);
		if (locations.find(key) != locations.end()) {
			return true;
		}
		key = url.substr(0, pos);
		if (locations.find(key) != locations.end()) {
			return true;
		}
		pos = key.rfind('/');
	}
	return false;
}

void Config::setDefaultErrorPage() {
	int status_code;

	for (status_code = 400; status_code <= 417; status_code++) {
		if (error_page.find(status_code) == error_page.end()) {
			error_page[status_code] = CLIENT_ERROR_PAGE;
		}
		else {
			error_page[status_code] = root + "/" + error_page[status_code];
		}
	}
	for (status_code = 500; status_code <= 505; status_code++) {
		if (error_page.find(status_code) == error_page.end()) {
			error_page[status_code] = SERVER_ERROR_PAGE;
		}
		else {
			error_page[status_code] = root + "/" + error_page[status_code];
		}
	}
}

void Config::server_token_parser(std::vector<std::string> one_line, std::set<std::string>& duplicated) {
	if (one_line.empty())
		throw std::invalid_argument("Error: invalid arguments\n");

	if (one_line[0] == "listen" && one_line.size() == 2) {
		std::stringstream ss(one_line[1]);
		int value;
		ss >> value;
		if (!value)
			return ;
		port = value;
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "host" && one_line.size() == 2) {
		std::string value;
		host = one_line[1];
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "server_name" && one_line.size() >= 2) {
		for (unsigned int i = 1; i < one_line.size(); i++) {
			name.push_back(one_line[i]);
		}

	} else if (one_line[0] == "error_page" && one_line.size() == 3) {
		std::stringstream ss(one_line[1]);
		int err_no;
		ss >> err_no;
		error_page.insert(std::pair<int, std::string>(err_no, one_line[2]));
	
	} else if (one_line[0] == "root" && one_line.size() == 2) {
		root = one_line[1];
		duplicated.insert(one_line[0]);

	} else if (one_line[0] == "index" && one_line.size() >= 2) {
		for (unsigned long i = 1; i < one_line.size(); i++) {
			index.push_back(one_line[i]);
		}

	} else if (one_line[0] == "client_max_body_size" && one_line.size() == 2) {
		std::stringstream ss(one_line[1]);
		long value;
		ss >> value;
		limit_body_size = value;
		duplicated.insert(one_line[0]);
	} else {
		throw std::invalid_argument("Error: invalid server key\n");
	}
}

// 인자 확인하는 함수

// void Config::print_checker(void) {
// 	std::cout << "============server block=============\n";
// 	std::cout << "port : " << port << std::endl;
// 	std::cout << std::endl;
// 	std::cout << "host : " << host << std::endl;
// 	std::cout << std::endl;
// 	for (int i = 0; i < name.size(); i++) {
// 		std::cout << "name : " << name[i]<< std::endl;
// 	}
// 	std::cout << std::endl;
// 	std::cout << "error_page : " << std::endl;
// 	print_map(error_page);
// 	std::cout << std::endl;
// 	std::cout << "root : " << root << std::endl;
// 	std::cout << std::endl;
// 	for (int i = 0; i < index.size(); i++) {
// 		std::cout << "index : " << index[i]<< std::endl;
// 	}
// 	std::cout << std::endl;
// 	std::cout << "limit_body_size : " << limit_body_size << std::endl;
// 	std::cout << std::endl;
// 	std::cout << "locations : " << std::endl;
// 	print_map(locations);
// 	std::cout << std::endl;
// }

// template<typename K, typename V>
// void Config::print_map(std::map<K, V> &m) {
// 	for (auto &pair: m) {
// 		std::cout << "location key : " << pair.first << "\ncontent\n" << pair.second << "\n";
// 	}
// }