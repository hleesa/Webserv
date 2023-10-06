#include <iostream>
#include "Config.hpp"
// #include "Server.hpp"


int main() {
    try {
//        Config("default.conf");
        Config("default2.conf");
    } catch (std::exception &exception) {
        std::cout << exception.what() << '\n';
        //끝
    }
//    std::cout << "Hello, World!" << std::endl;
    return 0;
}





