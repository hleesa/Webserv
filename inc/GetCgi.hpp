
#ifndef WEBSERV_GETCGI_HPP
#define WEBSERV_GETCGI_HPP

#include "Method.hpp"
#include "ToString.hpp"
#include <vector>
#include <string>

class GetCgi : public Method {

private:
    std::map<std::string, std::string> createHeaderFields(const std::string& body) const;

    bool isValidCgiGetUrl() const;

public:
    GetCgi(const HttpRequestMessage* request, const Config* config);

    HttpResponseMessage makeHttpResponseMessage();
    void checkAllowed(const std::string method) const;
};

bool isVisibleString(const std::string& str);

#endif //WEBSERV_GETCGI_HPP
