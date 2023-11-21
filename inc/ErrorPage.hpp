#ifndef ERRORPAGE_HPP
#define ERRORPAGE_HPP

#include "Config.hpp"
#include "HttpResponseMessage.hpp"
#include <string>
#include <map>

class ErrorPage {
	private:
		static std::string makeDate();
	public:
		static HttpResponseMessage makeErrorPageResponse(const int status_code, const Config* config);
		static std::string findErrorPageFilePath(const int status_code, const Config* config);
		static std::map<std::string, std::string> makeHeaderFileds(const std::string& body);
};

#endif