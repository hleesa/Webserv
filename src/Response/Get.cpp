#include "../../inc/Get.hpp"
//#include "../../inc/GetCgi.hpp"
#include "../../inc/MediaType.hpp"
#include <unistd.h>
#include <ctime>

Get::Get(const HttpRequestMessage* request, const Config* config) : Method(request, config) {
}

HttpResponseMessage Get::makeHttpResponseMessage(){
	if (isCgi()) {
		//GetCgi cgi(request, config);
		//return cgi.makeHttpResponseMessage();
	}
	Location location = config->getLocations()[location_key];
	if (location.hasReturnValue()) {
		return processReturnDirective();
	}
	Resource resource = makeResource();
	std::string body = resource.make();
	return HttpResponseMessage(200, makeHeaderFields(body, resource.getPath()), body);
}

HttpResponseMessage Get::processReturnDirective() const {
	std::pair<int, std::string> return_value = config->getLocations()[location_key].getReturnValue();
	int status_code = return_value.first;

	if (status_code / 100 == 3) { // redirection
		return makeRedirectionResponse(return_value);
	}
	std::string body = return_value.second;
	return HttpResponseMessage(status_code, makeHeaderFields(body, ".txt"), body);
}

HttpResponseMessage Get::makeRedirectionResponse(const std::pair<int, std::string> return_value) const {
	std::string host = request->getHeaderFields()["host"].at(0);
	std::string location = "http://" + host + return_value.second;
	std::string body = "Moved Permanently. Redirecting to " + location + ".";
	std::map<std::string, std::string> header = makeHeaderFields(body, ".txt");

	header["Location"] = location;
	header["Content-type"] = "text/plain";
	return HttpResponseMessage(return_value.first, header, body);
}

Resource Get::makeResource() const {
	std::string resource_path = findResourcePath();
	if (isDirectoryList(resource_path)) {
		return Resource(findRoot() + location_key, true);
	}
	return Resource(resource_path, false);
}

std::string Get::findPathByRoot(const std::string url) const{
	std::string root = findRoot();
	
	if (config->getLocations()[location_key].getRoot().empty()) {
		return root + "/" + url;
	}
	return root + "/" + url.substr(location_key.size());
}

std::string Get::findResourcePath() const {
	std::vector<std::string> index = config->getLocations()[location_key].getIndex();
	std::string url = request->getURL();

	if (index.empty()) {
		index = config->getIndex();
	}
	if (location_key != url) { 
		std::string path = findPathByRoot(url);
		if (!isDirectory(path)) {
			return checkFileExistence(path) ? path : "";
		}
		std::vector<std::string>::iterator itr = index.begin();
		path = findPathByRoot(url + "/" + *itr);
		while (!checkFileExistence(path) && ++itr != index.end()) {
			path = findPathByRoot(url + "/" + *itr);
		}
		return itr != index.end() ? path : "";
	}
	std::vector<std::string>::iterator itr = index.begin();
	std::string path = findPathByRoot(location_key + "/" + *itr);
	while (!checkFileExistence(path) && ++itr != index.end()) {
		path = findPathByRoot(location_key + "/" + *itr);
	}
	return itr != index.end() ? path : "";
}

bool Get::isDirectoryList(const std::string path) const {
	// if (isDirectory(path)) {
	// 	throw 403;
	// }
	if (path.size()) {
		return false;
	}
	if (!config->getLocations()[location_key].getAutoindex()) {
		throw 404;
	}
	return true;
}

std::map<std::string, std::string> Get::makeHeaderFields(const std::string& body, const std::string path) const {
	std::map<std::string, std::string> header = Method::makeHeaderFields();

	header["Content-length"] = std::to_string(body.length());
	if (!path.empty()) {
		header["Content-type"] = MediaType::getType(path);
	}
	return header;
}

bool checkFileExistence(const std::string file_name) {
	return !access(file_name.c_str(), R_OK);
}
