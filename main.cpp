#include <iostream>
#include "Config.hpp"
// #include "Server.hpp"


int main() {
    try
	{
        Config("default.conf");
    } 
	catch (std::exception &exception)
	{
        std::cout << exception.what() << '\n';
    }
    return 0;
}