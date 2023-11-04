#ifndef PARSINGDATA_HPP
#define PARSINGDATA_HPP

#include <vector>
#include <string>
#include <map>
#include "ReadingStatus.hpp"
#include "Body.hpp"

struct ParsingData {
	std::vector<std::string> request_line;
    std::map<std::string, std::vector<std::string> > header_fields;
	int status_code;

	std::string buffer;
	ReadingStatus status;
	std::string method;
	Body body;

	ParsingData() {
		request_line.clear();
		header_fields.clear();
		status_code = 0;
		buffer.clear();
		status = StartLine;
	}
};

#endif