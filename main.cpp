#include "Server.hpp"

int main(int ac, char *av[]) {
	std::istringstream serv_block("listen 8002");
	Server test(serv_block);
	

	return 0;
}