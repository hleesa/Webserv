#include <iostream>
#include "ConfigParser.hpp"
#include "ServerManager.hpp"

int main() {
    try {
        std::vector<Config> configs =  ConfigParser::parseConfigFile("default.conf");
		ServerManager manager(configs);

		manager.run();
    } 
	catch (std::exception &exception) {
        std::cout << exception.what() << '\n';
    }
	catch (const char* error) {
        std::cout << error << '\n';
    }
    return 0;
}