
#include "CgiGet.hpp"
#include <map>

//중복 RequestParser.cpp
bool isVisibleString(const std::string& str) {
    for (std::string::const_iterator ch = str.begin(); ch != str.end(); ++ch) {
        if (!isprint(*ch))
            return false;
    }
    return true;
}

// http://example.com/cgi-bin/cgi_script.py?input=Hello
std::vector<std::string> tokenizeUrl(const std::string& url){
    std::string new_url = url.substr(1);
    std::string delim = "/?=";
    for (std::string::iterator ch = new_url.begin(); ch != new_url.end(); ++ch) {
        if (delim.find(*ch)) {
            *ch = ' ';
        }
    }
    std::stringstream ss(new_url);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void validateComponent(const std::vector<std::string>& url_vec){
    if (url_vec.size() != 4) {
        throw 400;
    }
    for (std::vector<std::string>::const_iterator component = url_vec.begin();
         component != url_vec.end(); ++component) {
        if (!isVisibleString(*component)) {
            throw 400;
        }
    }
}

std::vector<std::string> parseUrl(const std::string url){
    std::vector<std::string> url_vec = tokenizeUrl(url);
    validateComponent(url_vec);
    return url_vec;
}

bool CgiGet::isValidCgiGetUrl(const std::vector<std::string>& request_line, const std::map<int, Config>& configs, int con_socket) {
    const std::vector<std::string> url_vec = parseUrl(request_line[1]);
    if (request_line.front() != "GET" || url_vec.front().find("cgi") != std::string::npos) {
        return false;
    }
    std::map<int, Config>::const_iterator found_config = configs.find(con_socket);
    if (found_config == configs.end()) {
        return false;
    }
    const Config& config = found_config->second;
    std::map<std::string, Location>::iterator location = config.getLocations().find(url_vec.front());
    if (location == config.getLocations().end() || url_vec.front() != location->first.substr(1)) {
        return false;
    }
    if (location->second.getRoot().substr(1) != url_vec[1]) // root or path
        return false;
    return true;
}