
#include "../../inc/Location.hpp"
#include <iostream>

bool isHttpMethod(const std::string& method);
bool isNumber(const std::string& string);
bool isValidRangeStatusCode(const int status_code);

Location::Location() {}

Location::Location(std::vector<std::vector<std::string> >& location_block)
: root(""), autoindex(false), limit_body_size(-1), cgi_path(""), cgi_ext("") {
	std::set<std::string> duplicated;
	std::vector<std::vector<std::string> >::iterator itr;

	for (itr = location_block.begin(); itr != location_block.end(); itr++) {
		parse(*itr, duplicated);
	}
	if (http_methods.empty()) {
		http_methods.insert("GET");
		http_methods.insert("POST");
		http_methods.insert("DELETE");
		http_methods.insert("HEAD");
	}
}

Location::Location(const Location &other) {
	*this = other;
}

Location &Location::operator=(const Location &other) {
	if (this != &other) {
		this->http_methods = other.http_methods;
		this->return_value = other.return_value;
		this->root = other.root;
		this->index = other.index;
		this->autoindex = other.autoindex;
		this->limit_body_size = other.limit_body_size;
		this->cgi_ext = other.cgi_ext;
		this->cgi_path = other.cgi_path;
	}
	return *this;
}

Location::~Location() {}

std::set<std::string> Location::getHttpMethods() const {
	return this->http_methods;
}

bool Location::isNotAllowedMethod(const std::string method) const {
	return this->http_methods.find(method) == this->http_methods.end();
}

std::pair<int, std::string> Location::getReturnValue() const {
	return this->return_value;
}

bool Location::hasReturnValue() const {
	return this->return_value.first != 0 && this->return_value.second.size();
}

std::string Location::getRoot() const {
	return this->root;
}

std::vector<std::string> Location::getIndex() const {
	return this->index;
}

bool Location::getAutoindex() const {
	return this->autoindex;
}

ssize_t Location::getLimitBodySize() const {
	return this->limit_body_size;
}

std::string Location::getCgiPath() const {
	return this->cgi_path;
}

std::string Location::getCgiExt() const {
	return this->cgi_ext;
}

void Location::parse(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string	directive;
	
	if (line.empty())
		return ;
	directive = *(line.begin());
	if (directive == "limit_except") {
		parseHttpMethod(line, duplicated);
		return ;
	}
	if (directive == "return") {
		parseReturnValue(line);
		return ;
	}
	if (directive == "root") {
		parseRoot(line, duplicated);
		return ;
	}
	if (directive == "index") {
		parseIndex(line, duplicated);
		return ;
	}
	if (directive == "autoindex") {
		parseAutoindex(line, duplicated);
		return ;
	}
	if (directive == "client_max_body_size") {
		parseLimitBodySize(line, duplicated);
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

void Location::checkDuplicated(const std::set<std::string>& duplicated, const std::string& directive) {
	if (duplicated.find(directive) != duplicated.end())
		throw (std::invalid_argument("Error: '" + directive + "' directive is duplicate"));
}

void Location::checkInvalidNumber(bool isValid, const std::string& directive) {
	if (isValid == false)
		throw (std::invalid_argument("Error: Invalid number of arguments in '" + directive + "' directive"));
}

void Location::parseHttpMethod(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string method;
	std::string directive("limit_except");
	std::vector<std::string>::iterator itr = line.begin() + 1;

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() > 1, directive);
	for (; itr != line.end(); itr++) {
		checkHttpMethod(*itr);
		http_methods.insert(*itr);
	}
	duplicated.insert(directive);
}

void Location::checkHttpMethod(const std::string& value) const {
	if (isHttpMethod(value) == false)
		throw (std::invalid_argument("Error: Invalid value '" + value + "' in 'limit_except' directive"));
}

void Location::parseReturnValue(std::vector<std::string>& line) {
	std::string directive("return");
	std::vector<std::string>::iterator itr = line.begin();

	checkInvalidNumber(line.size() == 2 || line.size() == 3, directive);
	setReturnCode(*(++itr));
	if (line.size() == 2)
		return ;
	setReturnString(*(++itr));
}

void Location::setReturnCode(const std::string& value) {
	if (!isNumber(value)) {
		throw (std::invalid_argument("Error: Invalid return code '" + value + "'"));
	}
	std::stringstream ss(value);
	ss >> return_value.first;
	if (!isValidRangeStatusCode(return_value.first)) {
		throw (std::invalid_argument("Error: Invalid return code '" + value + "'"));
	}
}

void Location::setReturnString(const std::string& value) {
	return_value.second = value;
}

void Location::parseRoot(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("root");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	root = line[1];
	duplicated.insert(directive);
}

void Location::parseIndex(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("index");
	std::vector<std::string>::iterator itr = line.begin() + 1;

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() > 1, directive);
	for (; itr != line.end(); itr++)
		index.push_back(*itr);
	duplicated.insert(directive);
}

void Location::parseAutoindex(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("autoindex");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	checkAutoindexFormat(line[1]);
	autoindex = line[1] == "on" ? true : false;
	duplicated.insert(directive);
}

void Location::checkAutoindexFormat(const std::string& value) const {
	if (!(value == "on" || value == "off"))
		throw (std::invalid_argument("Error: Invalid value '" + value + "' in 'autoindex' directive"));
}

void Location::parseLimitBodySize(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("client_max_body_size");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	std::string size(line[1]);
	if (!isNumber(size)) {
		throw (std::invalid_argument("Error: Invalid client max hody size '" + size + "'"));
	}	
	std::stringstream ss(size);
	ss >> limit_body_size;
	duplicated.insert(directive);
}

void Location::parseCgiPath(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("cgi_path");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	cgi_path = line[1];
	duplicated.insert(directive);
}

void Location::parseCgiExt(std::vector<std::string>& line, std::set<std::string>& duplicated) {
	std::string directive("cgi_ext");

	checkDuplicated(duplicated, directive);
	checkInvalidNumber(line.size() == 2, directive);
	cgi_ext = line[1];
	duplicated.insert(directive);
}

std::ostream& operator<<(std::ostream& out, Location& l) {
	out << "\n-----http methods-----\n";
	for (std::set<std::string>::iterator itr = l.http_methods.begin(); itr != l.http_methods.end(); itr++)
		out << *itr << " ";
	out << "\n-----return-----\n";
	out << "status code : " << l.return_value.first << "\nstring : " << l.return_value.second << "\n";
	out << "-----root-----\n" << l.root << "\n";
	out << "-----index-----\n";
	for (std::vector<std::string>::iterator itr = l.index.begin(); itr != l.index.end(); itr++)
		out << *itr << " ";
	out << "\n-----autoindex-----\n" << std::boolalpha << l.autoindex << std::endl;
	out << "\n-----autoindex-----\n" << l.limit_body_size << std::endl;
	out << "\n-----cgi_ext-----\n" << l.cgi_ext << std::endl;
	out << "\n-----cgi_path-----\n" << l.cgi_path << std::endl;
	return out;
}