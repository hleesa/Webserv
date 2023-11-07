#include "../../inc/GetMethod.hpp"
#include <unistd.h>
#include <ctime>
// EXIST, AUTOINDEX, NOTFOUND

HttpResponseMessage GetMethod::run(const HttpRequestMessage& request, const Config& config) {
	// resource의 경로 찾기
	std::string url = request.getURL();
	std::string location_key = findLocation(url, config);
	std::string resource_path = getFilePath(url, config, location_key);
	std::cout << "resource_path : " << resource_path << std::endl;
	// 존재하는지 찾기 -> 못 찾은 경우, autoindex 확인
	Resource resouce = makeResource(resource_path, config, location_key);
	// readFile
	// header-filed & body 만들기
	return makeHttpResponseMessage(resouce);
}

std::string GetMethod::findLocation(std::string url, const Config& config) {
	std::map<std::string, Location> locations = config.getLocations();
	Location location;
	unsigned long pos = url.rfind("/");
	
	while (pos != std::string::npos) {
		std::string path = url.substr(0, pos);
		std::cout << "path : " << path << std::endl;
		if (locations.find(path) != locations.end()) {
			std::cout << "found ! path is " << path << std::endl;
			return path;
		}
		url = path;
		pos = url.rfind("/");
	}
	return "/";
}

std::string GetMethod::getFilePath(std::string url, const Config& config, const std::string location_key) {
	std::map<std::string, Location> locations = config.getLocations();
	Location location = locations[location_key];
	std::string root = config.getRoot();

	if (location.getRoot().size()) {
		root = location.getRoot();
	}
	if (root[0] != '/') {
		root = "/" + root;
	}
	root = "." + root;
	std::string path;
	if (url != location_key) {
		path = searchValidPath(location.getIndex(), root + location_key + "/");
	}
	if (path.size())
		return path;
	return searchValidPath(config.getIndex(), root + "/");
}

std::string GetMethod::searchValidPath(std::vector<std::string> index, const std::string& pre_path) {
	std::vector<std::string>::iterator itr = index.begin();
	std::string file;
	
	for (itr = index.begin(); index.size() && itr != index.end(); itr++) {
		file = pre_path + *itr;
		if (existFile(file)) {
			return file;
		}
	}
	return "";
}

Resource GetMethod::makeResource(const std::string& resource_path, const Config& config, const std::string location_key) {
	std::map<std::string, Location> locations = config.getLocations();
	Location location = locations[location_key];
	ResourceStatus status = getResourceStaus(resource_path, location.getAutoindex());
	if (status == FOUND) {
		return Resource(resource_path, status);
	}
	// if (status == DirectoryList) {

	// }
	std::map<int, std::string> error_pages = config.getErrorpage();
	if (error_pages.find(404) != error_pages.end()) {
		std::string path = config.getRoot() + error_pages[404];
		return Resource(path, status);
	}
	return Resource();
}

ResourceStatus GetMethod::getResourceStaus(const std::string resource_path, const bool autoindex) {
	if (resource_path.size()) {
		return FOUND;
	}
	return autoindex ? DirectoryList : NotFound;
}

bool GetMethod::existFile(const std::string resource_path) {
	return !access(resource_path.c_str(), F_OK | R_OK);
}

HttpResponseMessage GetMethod::makeHttpResponseMessage(const Resource& resource) {
	int status_code = 200;
	std::string body = resource.read();
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