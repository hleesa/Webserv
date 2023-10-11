
#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP


#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "Location.hpp"
// #include "CgiLocation.hpp"

class Server {

	private:
		int port;
		std::string host;
		std::vector<std::string> name;
		std::map<int, std::string> error_page;
		std::string root;
		std::vector<std::string> index;
		long limit_body_size;
		std::map<std::string, Location> locations;
		// CgiLocation cgi_location;

	public:
		Server();

		Server(std::vector< std::vector< std::string> >& server_block);

		Server(const Server& other);

		Server& operator=(const Server& other);

		~Server();

		void server_token_parser(std::vector<std::string> one_line, std::set<std::string>& duplicated);
		// void server_token_parser(std::string key, std::stringstream& one_line, std::set<std::string>& duplicated);

//인자 확인 함수
		// void print_checker(void);
		// template<typename K, typename V>
		// void print_map(std::map<K, V> &m);
};


#endif //WEBSERV_SERVER_HPP
