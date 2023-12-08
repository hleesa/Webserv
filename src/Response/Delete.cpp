#include "../../inc/Delete.hpp"
#include <unistd.h>

Delete::Delete(const HttpRequestMessage* request, const Config* config) {
	this->request = request;
	this->config = config;	
	this->location_key = findLocationKey();
}

HttpResponseMessage Delete::makeHttpResponseMessage() {
	std::string path = findPathByRoot(request->getURL());

	if (access(path.c_str(), F_OK) == -1) {
		throw 404;
	}
	if (remove(path.c_str()) == -1) {
		throw 500;
	}
	return HttpResponseMessage(204, makeHeaderFields(), "");
}

void Delete::handleError(const std::string path) const {
	if (!access(path.c_str(), F_OK)) {
		throw 404;
	}
}

std::string Delete::findPathByRoot(const std::string url) const{
	std::string root = findRoot();
	
	if (config->getLocations()[location_key].getRoot().empty()) {
		return root + "/" + url;
	}
	return root + "/" + url.substr(location_key.size());
}
