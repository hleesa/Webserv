#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {
	private:
		int connection_socket;
		int listen_socket;

	public:
		Server();
		Server(int connection, int listen);
};

#endif