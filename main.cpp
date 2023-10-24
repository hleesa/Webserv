#include <iostream>
#include "ConfigParser.hpp"
// #include "Server.hpp"


int main() {
    try
	{
        std::vector<Config> configs =  ConfigParser::parseConfigFile("default.conf");
    } 
	catch (std::exception &exception)
	{
        std::cout << exception.what() << '\n';
    }
    return 0;
}