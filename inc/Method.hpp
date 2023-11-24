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
		
		std::string findLocationKey() const;
		std::string findRoot() const;
		
		std::map<std::string, std::string> makeHeaderFields() const;
		std::string makeDate() const;

	public:
		Method();
		virtual ~Method();

		bool isCgi() const;
		static Method* generate(const std::string method, const HttpRequestMessage* request, const Config* config);
		void checkAllowed(const std::string method) const;
		virtual HttpResponseMessage makeHttpResponseMessage() = 0;
};

#endif