#include "../../inc/Method.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/ToString.hpp"
#include <unistd.h>
#include <ctime>

Method::Method() {}

Method::Method(const HttpRequestMessage* request, const Config* config)
: request(request), config(config) {
	this->location_key = findLocationKey();
}

Method::~Method() {}

std::string Method::findLocationKey() {
	std::string url = request->getURL();
	std::map<std::string, Location> locations = config->getLocations();

	if (locations.find(url) != locations.end()) {
		return url;
	}
	if (url.size() > 1 && url.back() != '/') {
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

std::string Method::findRoot() {
	std::string root = config->getLocations()[location_key].getRoot();

	if (root.empty()) {
		root = config->getRoot();
	}
	return root;
}

std::map<std::string, std::string> Method::makeHeaderFileds(const std::string& body, const std::string path) {
	std::map<std::string, std::string> header;

	header["Date"] = makeDate();
	header["Connection"] = "keep-alive";
	header["Server"] = "Webserv";
	return header;
}

std::string Method::makeDate() {
	time_t timer = time(NULL);
	struct tm* t = localtime(&timer);
	std::string day_names[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	std::string months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	std::string date;

	date = day_names[t->tm_wday] + ", ";
	if (t->tm_mday < 10)
		date += "0";
	date += to_string(t->tm_mday) + " " + months[t->tm_mon] + " " + to_string(t->tm_year + 1900)
			+ " " + to_string(t->tm_hour) + ":" + to_string(t->tm_min) + ":" + to_string(t->tm_sec) + " GMT";
	return date;
}