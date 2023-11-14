#ifndef DELETE_HPP
#define DELETE_HPP

#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class Delete {
	private:
		HttpRequestMessage request;
		Config config;
		int status_code;
		std::string location_key;
		
		std::string findLocationKey();
		void handleError(const std::string path);
		std::string findRoot();
		std::string findPath();
		
		std::map<std::string, std::string> makeHeaderFileds();
		std::string makeDate();

	public:
		Delete(const HttpRequestMessage& request, const Config& config);

		HttpResponseMessage makeHttpResponseMessage();
};

#endif