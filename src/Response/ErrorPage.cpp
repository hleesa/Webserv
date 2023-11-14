#include "../../inc/ErrorPage.hpp"
#include "../../inc/HttpRequestMessage.hpp"
#include "../../inc/Resource.hpp"

HttpResponseMessage ErrorPage::makeErrorPageResponse(const int status_code, const Config& config) {
	Resource resource(findErrorPageFilePath(status_code, config), false);
	std::string body = resource.make();
	
	return HttpResponseMessage(status_code, makeHeaderFileds(body), body);
}

std::string ErrorPage::findErrorPageFilePath(const int status_code, const Config& config) {
	std::map<int, std::string> error_page = config.getErrorpage();

	return config.getRoot() + "/" + error_page[status_code];
}

std::map<std::string, std::string> ErrorPage::makeHeaderFileds(const std::string& body) {
	std::map<std::string, std::string> header;

	header["Content-length"] = std::to_string(body.length());
	header["Content-type"] = "text/html";
	header["Date"] = makeDate();
	header["Connection"] = "keep-alive";
	header["Server"] = "Webserv";
	return header;
}

std::string ErrorPage::makeDate() {
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