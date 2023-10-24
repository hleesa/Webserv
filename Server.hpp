#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {
	private:
		int connection_socket;
		int listen_socket;
		std::string request_content;
		// enum status; 
		// Request request;
		// Response response;
	public:
		// void appendContent(const std::string content);
		// void checkContent();
		// void clearContent();
};

#endif