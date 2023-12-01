#include "../../inc/ServerUtils.hpp"
#include <map>

std::string findLocationKey(const Config* config, const HttpRequestMessage* request ){
    std::string url = request->getURL();
    std::map<std::string, Location> locations = config->getLocations();

    if (locations.find(url) != locations.end()) {
        return url;
    }
    unsigned long pos = url.rfind('/');
    while (pos && pos != std::string::npos) {
        std::string key = url.substr(0, pos + 1);
        if (locations.find(key) != locations.end()) {
            return key;
        }
        key = url.substr(0, pos);
        if (locations.find(key) != locations.end()) {
            return key;
        }
        url = key;
        pos = url.rfind('/');
    }
    return "/";
}

bool isCgi(const Config* config, const HttpRequestMessage* request) {
    std::string extension = config->getLocations()[findLocationKey(config, request)].getCgiExt();
    std::string url = request->getURL();

    if (extension.empty()) {
        return false;
    }
    if (request->getMethod() == "GET") {
        size_t queryPos = url.find("?");
        if (queryPos == std::string::npos) {
            return false;
        }
        url = queryPos != std::string::npos ? url.substr(0, queryPos) : url;
    }
    if (url.size() < extension.size()) {
        return false;
    }
    return url.substr(url.size() - extension.size()) == extension;
}