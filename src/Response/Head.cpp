#include "../../inc/Head.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/ErrorPage.hpp"
#include <unistd.h>
#include <ctime>

Head::Head(const HttpRequestMessage* request, const Config* config) : Get(request, config) {}

HttpResponseMessage Head::makeHttpResponseMessage(){
	Location location = config->getLocations()[location_key];
	Resource resource;

	if (location.hasReturnValue()) {
		return processReturnDirective();
	}
	try {
		resource = makeResource();
		std::string body = resource.make();
	}
    catch (const int status_code) {
		Resource resource(ErrorPage::findErrorPageFilePath(status_code, config), false);
		std::string body = resource.make();
		return HttpResponseMessage(status_code, ErrorPage::makeHeaderFileds(body), "");
    }
	return HttpResponseMessage(200, makeHeaderFields(resource.getPath()), "");
}

HttpResponseMessage Head::processReturnDirective() const {
	std::pair<int, std::string> return_value = config->getLocations()[location_key].getReturnValue();
	int status_code = return_value.first;

	if (status_code / 100 == 3) { 
		return makeRedirectionResponse(return_value);
	}
	return HttpResponseMessage(status_code, makeHeaderFields(".txt"), "");
}

HttpResponseMessage Head::makeRedirectionResponse(const std::pair<int, std::string> return_value) const {
	std::string host = request->getHeaderFields()["host"].at(0);
	std::string location = "http://" + host + return_value.second;
	std::map<std::string, std::string> header = makeHeaderFields(".txt");

	header["Location"] = location;
	header["Content-type"] = "text/plain";
	return HttpResponseMessage(return_value.first, header, "");
}

std::map<std::string, std::string> Head::makeHeaderFields(const std::string path) const {
	std::map<std::string, std::string> header = Method::makeHeaderFields();

	if (!path.empty()) {
		header["Content-type"] = MediaType::getType(path);
	}
	return header;
}