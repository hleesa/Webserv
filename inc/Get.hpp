#ifndef GET_HPP
#define GET_HPP

#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class Get {
	private:
		HttpRequestMessage request;
		Config config;
		std::string location_key;
	
		std::string findLocationKey();
		std::string findRoot();
		std::string findResourcePath();
		bool isDirectoryList(const std::string path);
		Resource makeResource();

		HttpResponseMessage processReturnDirective();
		HttpResponseMessage makeRedirectionResponse(const std::pair<int, std::string> return_value);
		std::map<std::string, std::string> makeHeaderFileds(const std::string& body, const std::string path);
		std::string makeDate();

	public:
		Get(const HttpRequestMessage& request, const Config& config);

		HttpResponseMessage makeHttpResponseMessage();
};

bool checkFileExistence(const std::string file_name);

#endif