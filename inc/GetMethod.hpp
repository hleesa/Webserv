#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class GetMethod {
	private:
		static std::string findLocation(std::string url, const Config& config);
		static std::string getFilePath(std::string url, const Config& config, const std::string location_key);
		
		static Resource makeResource(const std::string& resource_path, const Config& config, const std::string location_key);
		static std::string searchValidPath(std::vector<std::string> index, const std::string& pre_path);
		static bool existFile(const std::string resource_path);
		static ResourceStatus getResourceStaus(const std::string resource_path, const bool autoindex);
		static HttpResponseMessage makeHttpResponseMessage(const Resource& resource);
		static std::map<std::string, std::string> makeHeaderFileds(const std::string& body);
		static std::string makeDate();

	public:
		static HttpResponseMessage run(const HttpRequestMessage& request, const Config& config);
};

#endif