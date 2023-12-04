#include <iostream>
#include "../inc/ConfigParser.hpp"
#include "../inc/ServerManager.hpp"

#define DEFAULT_CONF "ConfigFile/default.conf"

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    try {
		std::string config_file(DEFAULT_CONF);
		if (argc > 1) {
			config_file = argv[1];
		}
        std::vector<Config> configs =  ConfigParser::parseConfigFile(config_file);
		ServerManager manager(&configs);
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