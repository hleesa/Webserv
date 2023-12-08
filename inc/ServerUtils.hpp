#ifndef SERVERUTILS_HPP
#define SERVERUTILS_HPP

#include "Config.hpp"
#include "HttpRequestMessage.hpp"
#include <string>

std::string findLocationKey(const Config* config, const HttpRequestMessage* request);
bool isCgi(const Config* config, const HttpRequestMessage* request);

#endif