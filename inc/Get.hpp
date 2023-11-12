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
		int status_code;
		std::string location_key;
		Resource resource;
	
		std::string findLocationKey();
		std::string findRoot();
		std::string findResourcePath();
		ResourceStatus getResourceStatus(const std::string path);
		std::string findErrorPageFilePath();
		Resource makeResource();
		std::string redefineResourcePath(const ResourceStatus status);

		HttpResponseMessage processReturnDirective();
		HttpResponseMessage makeRedirectionResponse(const std::pair<int, std::string> return_value);
		std::map<std::string, std::string> makeHeaderFileds(const std::string& body);
		std::string makeDate();

	public:
		Get(const HttpRequestMessage& request, const Config& config);

		HttpResponseMessage makeHttpResponseMessage();
};

bool checkFileExistence(const std::string file_name);

#endif