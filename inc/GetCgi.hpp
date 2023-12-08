
#ifndef WEBSERV_GETCGI_HPP
#define WEBSERV_GETCGI_HPP

#include "Method.hpp"
#include "ToString.hpp"
#include "CgiData.hpp"
#include <vector>
#include <string>

class GetCgi{

private:
    const HttpRequestMessage* request;
    const Config* config;
    std::string location_key;

    std::map<std::string, std::string> createHeaderFields(const std::string& body) const;
    bool isValidCgiGetUrl() const;

public:
    GetCgi(const HttpRequestMessage* request, const Config* config);

    CgiData* execveCgi();
    void checkAllowed(const std::string method) const;

    static HttpResponseMessage makeResponse(std::string cgiResponse);

};

bool isVisibleString(const std::string& str);

#endif //WEBSERV_GETCGI_HPP
