#include "../../inc/GetMethod.hpp"
#include <unistd.h>
#include <ctime>

GetMethod::GetMethod(const HttpRequestMessage& request, const Config& config) {
	this->request = request;
	this->config = config;
	this->location_key = findLocationKey();
	this->resource = makeResource();
	std::cout << "url : " << request.getURL() << "\n\n";
}

std::string GetMethod::findLocationKey() {
	std::string url = request.getURL();
	std::map<std::string, Location> locations = config.getLocations();

	if (locations.find(url) != locations.end()) {
		return url;
	}
	if (url.size() > 1 && *--url.end() != '/') {
		url += "/";
	}
	unsigned long pos = url.rfind('/');
	while (pos != std::string::npos) {
		std::string key = url.substr(0, pos);
		if (locations.find(key) != locations.end()) {
			return key;
		}
		url = key;
		pos = url.rfind('/');
	}
	return "/";
}

std::string GetMethod::findRoot() {
	std::string root = config.getLocations()[location_key].getRoot();

	if (root.empty()) {
		root = config.getRoot();
	}
	return root;
}

std::string GetMethod::findResourcePath() {
	std::string root = findRoot();
	std::vector<std::string> index = config.getLocations()[location_key].getIndex();
	std::string url = request.getURL();

	if (url != "/" && *url.rbegin() == '/') {
		url.resize(url.size() - 1);
	}
	if (location_key != "/" && location_key != url) {
		std::string path = root + "/" + url;
		return checkFileExistence(path) ? path : "";
	}

	std::vector<std::string>::iterator itr = index.begin();
	std::string path = root + location_key + "/" + *itr;
	std::cout << "path : " << path << std::endl;
	while (!checkFileExistence(path) && ++itr != index.end()) {
		path = root + location_key + "/" + *itr;
	}
	return itr != index.end() ? path : "";
}

bool checkFileExistence(const std::string file_name) {
	return !access(file_name.c_str(), R_OK);
}

ResourceStatus GetMethod::getResourceStatus(const std::string path) {
	if (path.size()) {
		return FOUND;
	}
	return config.getLocations()[location_key].getAutoindex() ? DirectoryList : NotFound;
}

std::string GetMethod::findErrorPageFilePath() {
	std::map<int, std::string> error_page = config.getErrorpage();

	if (error_page.find(404) != error_page.end()) {
		return config.getRoot() + "/" + error_page[404];
	}
	return "";
}

Resource GetMethod::makeResource() {
	std::string resource_path = findResourcePath();
	ResourceStatus status = getResourceStatus(resource_path);
	if (status == DirectoryList) {
		resource_path = findRoot() + location_key;
	}
	if (status == NotFound) {
		resource_path = findErrorPageFilePath();
	}
	// std::cout << "resource_path : " << resource_path << std::endl;
	return Resource(resource_path, status);
}

HttpResponseMessage GetMethod::makeHttpResponseMessage() {
	int status_code = 200;

	std::string body = resource.makeResource();
	if (resource.getStatus() == NotFound)
		status_code = 404;
	return HttpResponseMessage(status_code, makeHeaderFileds(body), body);
}

std::map<std::string, std::string> GetMethod::makeHeaderFileds(const std::string& body) {
	std::map<std::string, std::string> header;

	header["Content-length"] = std::to_string(body.length());
	header["content-type"] = "text/html";
	// MIME을 어떻게 찾지 ..? 
	header["Date"] = makeDate(); 
	return header;
}

std::string GetMethod::makeDate() {
	time_t timer = time(NULL);
	struct tm* t = localtime(&timer);
	std::string day_names[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	std::string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::string date;

	date = day_names[t->tm_wday] + ", ";
	if (t->tm_mday < 10)
		date += "0";
	date += std::to_string(t->tm_mday) + " " + months[t->tm_mon] + " " + std::to_string(t->tm_year + 1900)
			+ " " + std::to_string(t->tm_hour) + ":" + std::to_string(t->tm_min) + ":" + std::to_string(t->tm_sec) + " GMT";
	return date;
}