#include "../../inc/Get.hpp"
#include "../../inc/MediaType.hpp"
#include <unistd.h>
#include <ctime>

Get::Get(const HttpRequestMessage& request, const Config& config) {
	this->request = request;
	this->config = config;
	this->location_key = findLocationKey();
}

std::string Get::findLocationKey() {
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

HttpResponseMessage Get::makeHttpResponseMessage() {
	Location location = config.getLocations()[location_key];

	if (location.hasReturnValue()) {
		return processReturnDirective();
	}
	Resource resource = makeResource();
	std::string body = resource.make();
	return HttpResponseMessage(200, makeHeaderFileds(body, resource.getPath()), body);
}

HttpResponseMessage Get::processReturnDirective() {
	std::pair<int, std::string> return_value = config.getLocations()[location_key].getReturnValue();
	int status_code = return_value.first;

	if (status_code / 100 == 3) { // redirection
		return makeRedirectionResponse(return_value);
	}
	std::string body = return_value.second;
	return HttpResponseMessage(status_code, makeHeaderFileds(body, ".txt"), body);
}

HttpResponseMessage Get::makeRedirectionResponse(const std::pair<int, std::string> return_value) {
	std::string host = request.getHeaderFields()["host"].at(0);
	std::string location = "http://" + host + return_value.second;
	std::string body = "Moved Permanently. Redirecting to " + location + ".";
	std::map<std::string, std::string> header = makeHeaderFileds(body, ".txt");

	header["Location"] = location;
	// header["Content-type"] = "text/plain";
	return HttpResponseMessage(return_value.first, header, body);
}

Resource Get::makeResource() {
	if (config.getLocations()[location_key].isNotAllowedMethod("GET")) {
		throw 405;
	}
	std::string resource_path = findResourcePath();
	if (isDirectoryList(resource_path)) {
		return Resource(findRoot() + location_key, true);
	}
	return Resource(resource_path, false);
}

std::string Get::findResourcePath() {
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

std::string Get::findRoot() {
	std::string root = config.getLocations()[location_key].getRoot();

	if (root.empty()) {
		root = config.getRoot();
	}
	return root;
}

bool checkFileExistence(const std::string file_name) {
	return !access(file_name.c_str(), R_OK);
}

bool Get::isDirectoryList(const std::string path) {
	if (isDirectory(path)) {
		throw 403;
	}
	if (path.size()) {
		return false;
	}
	if (!config.getLocations()[location_key].getAutoindex()) {
		throw 404;
	}
	return true;
}

std::map<std::string, std::string> Get::makeHeaderFileds(const std::string& body, const std::string path) {
	std::map<std::string, std::string> header;

	header["Content-length"] = std::to_string(body.length());
	if (!path.empty()) {
		header["Content-type"] = MediaType::getType(path);
	}
	header["Date"] = makeDate();
	header["Connection"] = "keep-alive";
	header["Server"] = "Webserv";
	return header;
}

std::string Get::makeDate() {
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