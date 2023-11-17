
#ifndef WEBSERV_CGIGET_HPP
#define WEBSERV_CGIGET_HPP

#include "Config.hpp"
#include "HttpResponseMessage.hpp"
#include "HttpRequestMessage.hpp"
#include <vector>
#include <string>

class CgiGet {

public:
    static bool isValidCgiGetUrl(const std::vector<std::string>& request_line, const Config& config);

    static HttpResponseMessage
    processCgiGet(HttpRequestMessage request, const Config& config);
};

bool isVisibleString(const std::string& str);

template <typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif //WEBSERV_CGIGET_HPP
