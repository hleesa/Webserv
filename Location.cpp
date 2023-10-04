
#include "Location.hpp"
#include "type.hpp"

#include <iostream>

bool isHttpMethod(const std::string& method);

Location::Location() {}

Location::Location(std::istringstream& location_block) {

	std::string line;

	return_value.first = -1;
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
		throw (std::invalid_argument("Error: Invalid value format '" + value + "'"));
}

void Location::redefineLastValue(std::string& value) const {

	if (value[value.size() - 1] == ';')
		value.resize(value.size() - 1);
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

	redefineLastValue(method);
	if (isHttpMethod(method) == false)
		throw (std::invalid_argument("Error: Invalid http methods '" + method + "'"));
}

void Location::parseReturnValue(std::stringstream& ss) {

	std::string value;

	if (return_value.first != -1)
		throw (std::invalid_argument("Error: Too many return directive"));
	ss >> value;
	setReturnStatusCode(value);
	if (ss.eof())
		return ;
	ss >> value;
	if (ss.eof() == false)
		throw (std::invalid_argument("Error: Invalid return directive line"));
	setReturnString(value);
	// std::cout << "status code : " << return_value.first << ", string : " << return_value.second << std::endl;
}

void Location::setReturnStatusCode(std::string& value) {
	
	checkValueFormat(value);
	redefineLastValue(value);
	if (isInteger(value) == false)
		throw (std::invalid_argument("Error: Invalid status code '" + value + "'"));
	std::stringstream ss(value);
	ss >> return_value.first;
}

void Location::setReturnString(std::string& value) {
	
	checkValueFormat(value);
	redefineLastValue(value);
	return_value.second = value;
}

void Location::parseRoot(std::stringstream& ss) {

	std::string value;

	if (root.empty() == false)
		throw (std::invalid_argument("Error: Too many root directive"));
	ss >> value;
	if (ss.eof() == false)
		throw (std::invalid_argument("Error: Invalid root directive line"));
	checkValueFormat(value);
	redefineLastValue(value);
	root = value;
	// std::cout << "root : " << root << std::endl;
}

void Location::parseIndex(std::stringstream& ss) {

}

void Location::parseAutoindex(std::stringstream& ss) {

}