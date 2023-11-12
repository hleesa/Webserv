#include "../../inc/GetMethod.hpp"
#include "../../inc/MediaType.hpp"
#include <unistd.h>
#include <ctime>

GetMethod::GetMethod(const HttpRequestMessage& request, const Config& config) {
	this->request = request;
	this->config = config;
	this->status_code = 200;
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

bool checkFileExistence(const std::string file_name) {
	return !access(file_name.c_str(), R_OK);
}

std::string GetMethod::findResourcePath() {
	std::string root = findRoot();
	std::vector<std::string> index = config.getLocations()[location_key].getIndex();
	std::string url = request.getURL();

	if (index.empty()) {
		index = config.getIndex();
	}
	// / 이 아닌 url에 대해
	if (url != "/" && *url.rbegin() == '/') { // url 끝에 있는 / 제거
		url.resize(url.size() - 1);
	}
	if (location_key != url) { // url 자체를 찾음 
		std::string path = root + "/" + url;
		return checkFileExistence(path) ? path : "";
	}

	std::vector<std::string>::iterator itr = index.begin();
	std::string path = root + location_key + "/" + *itr;
	while (!checkFileExistence(path) && ++itr != index.end()) {
		path = root + location_key + "/" + *itr;
	}
	return itr != index.end() ? path : "";
}

ResourceStatus GetMethod::getResourceStatus(const std::string path) {
	if (isDirectory(path)) {
		status_code = 403;
		return ERROR;	
	}
	if (path.size()) {
		return FOUND;
	}
	return config.getLocations()[location_key].getAutoindex() ? DirectoryList : NotFound;
}

std::string GetMethod::findErrorPageFilePath() {
	std::map<int, std::string> error_page = config.getErrorpage();

	return config.getRoot() + "/" + error_page[status_code];
}

std::string GetMethod::redefineResourcePath(const ResourceStatus status) {
	if (status == DirectoryList) {
		return findRoot() + location_key;
	}
	if (status == NotFound) {
		status_code = 404;
	}
	return findErrorPageFilePath();
}

Resource GetMethod::makeResource() {
	if (config.getLocations()[location_key].isNotAllowedMethod("GET")) {
		status_code = 405;
		return  Resource(findErrorPageFilePath(), ERROR);	
	}

	std::string resource_path = findResourcePath();
	ResourceStatus status = getResourceStatus(resource_path);
	if (status != FOUND) {
		resource_path = redefineResourcePath(status);
	}
	return Resource(resource_path, status);
}

HttpResponseMessage GetMethod::makeHttpResponseMessage() {
	std::string body = resource.make();
	Location location = config.getLocations()[location_key];

	if (location.hasReturnValue()) {
		return processReturnDirective();
	}
	return HttpResponseMessage(status_code, makeHeaderFileds(body), body);
}

HttpResponseMessage GetMethod::processReturnDirective() {
	std::pair<int, std::string> return_value = config.getLocations()[location_key].getReturnValue();
	int status_code = return_value.first;

	if (status_code / 100 == 3) { // redirection
		return makeRedirectionResponse(return_value);
	}
	std::string body = return_value.second;
	return HttpResponseMessage(status_code, makeHeaderFileds(body), body);
}

HttpResponseMessage GetMethod::makeRedirectionResponse(const std::pair<int, std::string> return_value) {
	std::string host = request.getHeaderFields()["host"].at(0);
	std::string location = "http://" + host + return_value.second;
	std::string body = "Moved Permanently. Redirecting to " + location + ".";
	std::map<std::string, std::string> header = makeHeaderFileds(body);

	header["Location"] = location;
	header["Content-type"] = "text/plain";
	return HttpResponseMessage(return_value.first, header, body);
}

std::map<std::string, std::string> GetMethod::makeHeaderFileds(const std::string& body) {
	std::map<std::string, std::string> header;

	header["Content-length"] = std::to_string(body.length());
	header["Content-type"] = MediaType::getType(resource.getPath());
	header["Date"] = makeDate();
	header["Connection"] = "keep-alive";
	header["Server"] = "Webserv";
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