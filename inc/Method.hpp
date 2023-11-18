#ifndef METHOD_HPP
#define METHOD_HPP

#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include <string>

class Method {
	protected:
		const HttpRequestMessage* request;
		const Config* config;
		std::string location_key;
		
		std::string findLocationKey();
		std::string findRoot();
		
		std::map<std::string, std::string> makeHeaderFileds(const std::string& body, const std::string path);
		std::string makeDate();

	public:
		Method();
		Method(const HttpRequestMessage* request, const Config* config);
		virtual ~Method();

		virtual HttpResponseMessage makeHttpResponseMessage() = 0;
};

#endif