
#ifndef WEBSERV_CONFIG_HPP
#define WEBSERV_CONFIG_HPP


#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "Location.hpp"

#define CLIENT_ERROR_PAGE "./default/4xx.html"
#define SERVER_ERROR_PAGE "./default/5xx.html"

class Config {

	private:
		int port;
		std::string host;
		std::vector<std::string> name;
		std::map<int, std::string> error_page;
		std::string root;
		std::vector<std::string> index;
		long limit_body_size;
		std::map<std::string, Location> locations;

		// void setLocation(std::vector< std::vector<std::string> >& loc_block, const std::string key, bool& hasCgi);

		void setDefaultErrorPage();

	public:
		Config();
		Config(std::vector< std::vector< std::string> >& server_block);
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		void server_token_parser(std::vector<std::string> one_line, std::set<std::string>& duplicated);

		int getPort() const;
		std::string getHost() const;
		std::vector<std::string> getName() const;
		std::map<int, std::string> getErrorpage() const;
		std::string getRoot() const;
		std::vector<std::string> getIndex() const;
		size_t getLimitBodySize() const;
		std::map<std::string, Location> getLocations() const;
		bool hasLocationOf(const std::string url) const;

//인자 확인 함수
		// void print_checker(void);
		// template<typename K, typename V>
		// void print_map(std::map<K, V> &m);
};


#endif //WEBSERV_CONFIG_HPP
