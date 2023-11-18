#include "../../inc/CodeToReason.hpp"

CodeToReason::CodeToReason() {
	init();
}

void CodeToReason::init() {
	codeToReason[100] = "Continue";
	codeToReason[101] = "Switching Protocols";
	codeToReason[200] = "OK";
	codeToReason[201] = "Created";
	codeToReason[202] = "Accepted";
	codeToReason[203] = "Non-Authoritative Information";
	codeToReason[204] = "No Content";
	codeToReason[205] = "Reset Content";
	codeToReason[300] = "Multiple Choices";
	codeToReason[301] = "Moved Permanently";
	codeToReason[302] = "Found";
	codeToReason[303] = "See Other";
	codeToReason[305] = "Use Proxy";
	codeToReason[307] = "Temporary Redirect";
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
	codeToReason[413] = "Payload Too Large";
	codeToReason[414] = "URI Too Long";
	codeToReason[415] = "Unsupported Media Type";
	codeToReason[417] = "Expectation Failed";
	codeToReason[426] = "Upgrade Required";
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