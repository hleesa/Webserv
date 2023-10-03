
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"


class Config {

private:
    std::vector<Server> servers;

public:
    Config();

    Config(const std::string &config_file);

    Config(const Config &other);

    Config &operator=(const Config &ohter);

    ~Config();

};



#endif //CONFIG_HPP
