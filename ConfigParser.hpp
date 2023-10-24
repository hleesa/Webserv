
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Config.hpp"

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

class ConfigParser {

private:
    ConfigParser();

    ConfigParser(const ConfigParser& other);

    ConfigParser& operator=(const ConfigParser& other);

    ~ConfigParser();

public:
    static std::vector<Config> parseConfigFile(const std::string& file_name);
};

#endif //CONFIG_HPP
