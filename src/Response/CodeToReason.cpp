#include "../../inc/CodeToReason.hpp"

CodeToReason::CodeToReason() {
	init();
}

void CodeToReason::init() {
	codeToReason[200] = "OK";
	codeToReason[201] = "Created";
	codeToReason[202] = "Accepted";
	codeToReason[301] = "Moved Permanently";
	codeToReason[400] = "Bad Request";
	codeToReason[402] = "Payment Required";
	codeToReason[403] = "Forbidden";
	codeToReason[404] = "Not Found";
	codeToReason[405] = "Method Not Allowed";
	codeToReason[406] = "Not Acceptable";
	codeToReason[408] = "Request Timeout";
	codeToReason[409] = "Conflict";
	codeToReason[410] = "Gone";
	codeToReason[411] = "Length Required";
	codeToReason[500] = "Internal Server Error";
	codeToReason[501] = "Not Implemented";
	codeToReason[502] = "Bad Gateway";
	codeToReason[503] = "Service Unavailable";
	codeToReason[504] = "Gateway Timeout";
	codeToReason[505] = "HTTP Version Not Supported";

}

std::string CodeToReason::getReasonPharse(const int status_code) {
	try {
		return codeToReason.at(status_code);
	}
	catch (const std::exception& e)
	{}
	return codeToReason[500];
}