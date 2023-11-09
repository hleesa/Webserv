#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class GetMethod {
	private:
		HttpRequestMessage request;
		Config config;
		std::string location_key;
		Resource resource;
	
		std::string findLocationKey();
		std::string findRoot();
		std::string findResourcePath();
		ResourceStatus getResourceStatus(const std::string path);
		std::string findErrorPageFilePath();
		Resource makeResource();

		std::map<std::string, std::string> makeHeaderFileds(const std::string& body);
		std::string makeDate();

	public:
		GetMethod(const HttpRequestMessage& request, const Config& config);

		HttpResponseMessage makeHttpResponseMessage();
};

bool checkFileExistence(const std::string file_name);

#endif