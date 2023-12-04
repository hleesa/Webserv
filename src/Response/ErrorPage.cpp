#include "../../inc/ErrorPage.hpp"
#include "../../inc/HttpRequestMessage.hpp"
#include "../../inc/Resource.hpp"
#include "../../inc/Method.hpp"

HttpResponseMessage ErrorPage::makeErrorPageResponse(const int status_code, const Config* config) {
	Resource resource(findErrorPageFilePath(status_code, config), false);
	std::string body = resource.make();
	
	return HttpResponseMessage(status_code, makeHeaderFields(body), body);
}

std::string ErrorPage::findErrorPageFilePath(const int status_code, const Config* config) {
	std::map<int, std::string> error_page = config->getErrorpage();

	std::string path = error_page[status_code];
	return path;
}

std::map<std::string, std::string> ErrorPage::makeHeaderFields(const std::string& body) {
	std::map<std::string, std::string> header;

	header["Content-length"] = std::to_string(body.length());
	header["Content-type"] = "text/html";
	header["Date"] = Method::makeDate();
	header["Connection"] = "close";
	header["Server"] = "Webserv";
	return header;
}
