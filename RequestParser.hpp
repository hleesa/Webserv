
#ifndef WEBSERV_REQUESTPARSER_HPP
#define WEBSERV_REQUESTPARSER_HPP

#include "Config.hpp"
#include <map>
#include <string>
#include <vector>
#include "HttpRequestMessage.hpp"

class RequestParser {

private:
    std::vector<std::string> parseRequestLine(const std::string& request_line);

    std::map<std::string, std::vector<std::string> > parseHeaderFields(const std::vector<std::string>& request);

    std::string parseMessageBody(const std::string& message_body);

public:
    HttpRequestMessage parseRequestMessage(std::vector<std::string> request);
};

#endif //WEBSERV_REQUESTPARSER_HPP
