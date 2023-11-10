#include "../../inc/CgiLocation.hpp"

CgiLocation::CgiLocation() {}

CgiLocation::CgiLocation(std::vector<std::vector<std::string> >& location_block) {
	std::set<std::string> duplicated;
	std::vector<std::vector<std::string> >::iterator itr;

	for (itr = location_block.begin(); itr != location_block.end(); itr++) {
		parse(*itr, duplicated);
	}
}

CgiLocation::CgiLocation(const CgiLocation &other) {
	*this = other;
}

CgiLocation::~CgiLocation() {}

CgiLocation& CgiLocation::operator=(const CgiLocation &other) {
	if (this != &other) {
		this->root = other.root;
		this->index = other.index;
		this->cgi_path = other.cgi_path;
		this->cgi_ext = other.cgi_ext;
	}
	return *this;
}
	
std::string CgiLocation::getCgiPath() const {
	return this->cgi_path;
}

std::string CgiLocation::getCgiExt() const {
	return this->cgi_ext;
}

void CgiLocation::parse(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string	directive;

	if (line.empty())
		return ;
	directive = *(line.begin());
	if (directive == "root") {
		parseRoot(line, duplicated);
		return ;
	}
	if (directive == "index") {
		parseIndex(line, duplicated);
		return ;
	}
	if (directive == "cgi_path") {
		parseCgiPath(line, duplicated);
		return ;
	}
	if (directive == "cgi_ext") {
		parseCgiExt(line, duplicated);
		return ;
	}
	throw (std::invalid_argument("Error: unknown directive '" + directive + "'"));
}
		
void CgiLocation::parseCgiPath(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("cgi_path");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	cgi_path = line[1];
	duplicated.insert(directive);
}

void CgiLocation::parseCgiExt(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("cgi_ext");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	cgi_ext = line[1];
	duplicated.insert(directive);
}
