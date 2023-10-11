
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"

enum lineType {
    SPACE,
    COMMENT,
    SERVER_OPEN,
    SERVER_CLOSE,
    SEMICOLON,
    LOCATION_OPEN,
    LOCATION_CLOSE,
    INVALID
};

class Config {

private:
    std::vector<Server> servers;

    Config();

public:

    Config(const std::string& file_name);

    Config(const Config& other);

    Config& operator=(const Config& other);

    ~Config();

    std::vector<Server> getServers() const;
};

std::ostream& operator<<(std::ostream& os, const Config& cfg);

#endif //CONFIG_HPP
