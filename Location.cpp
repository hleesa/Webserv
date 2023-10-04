
#include "Location.hpp"

#include <iostream>

bool isHttpMethod(const std::string& method);

Location::Location() {}

Location::Location(std::istringstream& location_block) : root("") {

	std::string line;

	while (location_block.eof() == false)
	{
		std::getline(location_block, line);
		parse(line);
	}
}

Location::Location(const Location &other) {

	*this = other;
}

Location &Location::operator=(const Location &other) {

	if (this != &other)
	{
		this->http_methods = other.http_methods;
		this->return_value = other.return_value;
		this->root = other.root;
		this->index = other.index;
		this->autoindex = other.autoindex;
	}
	return *this;
}

Location::~Location() {}

void Location::parse(const std::string &line) {

	std::stringstream ss(line);
	std::string	directive;
	
	if (line.empty())
		return ;
	ss >> directive;
	std::cout << "directive : " << directive << std::endl;
	if (directive == "allow_methods") {
		parseMethod(ss);
		return ;
	}
	if (directive == "return") {
		parseReturnValue(ss);
		return ;
	}
	if (directive == "root") {
		parseRoot(ss);
		return ;
	}
	if (directive == "index") {
		parseIndex(ss);
		return ;
	}
	if (directive == "autoindex") {
		parseAutoindex(ss);
		return ;
	}
	throw (std::invalid_argument("Error: Invalid directive '" + directive + "'"));
}

void Location::checkValueFormat(const std::string& value) const {

	int pos = value.find(";");
	bool isInSemicolon = pos != std::string::npos;
	int size = static_cast<int>(value.size());

	if (isInSemicolon && (pos < size - 1 || size == 1))
		throw (std::invalid_argument("Error: Invalid value format"));
}

void Location::parseMethod(std::stringstream& ss) {

	std::string method;

	if (http_methods.size() > 0)
		throw (std::invalid_argument("Error: Too many allow_methods directive"));
	while (ss.eof() == false)
	{
		ss >> method;
		checkValueFormat(method);
		checkMethodFormat(method);
		http_methods.insert(method);
	}
	// for (std::set<std::string>::iterator itr = http_methods.begin(); itr != http_methods.end(); itr++)
	// 	std::cout << *itr << std::endl;
}

void Location::checkMethodFormat(std::string& method) const {

	if (method[method.size() - 1] == ';')
		method.resize(method.size() - 1);
	if (method == ";" || isHttpMethod(method) == false)
		throw (std::invalid_argument("Error: Invalid http methods '" + method + "'"));
}

void Location::parseReturnValue(std::stringstream& ss) {

}

void Location::parseRoot(std::stringstream& ss) {

}

void Location::parseIndex(std::stringstream& ss) {

}

void Location::parseAutoindex(std::stringstream& ss) {

}