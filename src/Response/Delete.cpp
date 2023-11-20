#include "../../inc/Delete.hpp"
#include <unistd.h>

Delete::Delete(const HttpRequestMessage* request, const Config* config) {
	this->request = request;
	this->config = config;	
	this->location_key = findLocationKey();
}

HttpResponseMessage Delete::makeHttpResponseMessage() {
	std::string path = findPath();

	if (remove(path.c_str()) == -1) {
		throw 500;
	}
	return HttpResponseMessage(204, makeHeaderFileds(), "");
}

void Delete::handleError(const std::string path) const {
	if (!access(path.c_str(), F_OK)) {
		throw 404;
	}
}

std::string Delete::findPath() const {
	std::string root = findRoot();
	std::string url = request->getURL();

	return root + url;
}
