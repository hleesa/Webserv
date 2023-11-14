#include "../../inc/Delete.hpp"
#include <unistd.h>

Delete::Delete(const HttpRequestMessage& request, const Config& config) {
	this->request = request;
	this->config = config;	
	this->location_key = findLocationKey();
}

std::string Delete::findLocationKey() {
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
		
HttpResponseMessage Delete::makeHttpResponseMessage() {
	std::string path = findPath();

	if (remove(path.c_str()) == -1) {
		throw 500;
	}
	return HttpResponseMessage(204, makeHeaderFileds(), "");
}

void Delete::handleError(const std::string path) {
	if (config.getLocations()[location_key].isNotAllowedMethod("DELETE")) {
		throw 405;
	}
	if (!access(path.c_str(), F_OK)) {
		throw 404;
	}
}

std::string Delete::findRoot() {
	std::string root = config.getLocations()[location_key].getRoot();

	if (root.empty()) {
		root = config.getRoot();
	}
	return root;
}

std::string Delete::findPath() {
	std::string root = findRoot();
	std::string url = request.getURL();

	return root + url;
}

std::map<std::string, std::string> Delete::makeHeaderFileds() {
	std::map<std::string, std::string> header;

	header["Date"] = makeDate();
	header["Server"] = "Webserv";
	return header;
}

std::string Delete::makeDate() {
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