
#include "Location.hpp"
#include "type.hpp"

#include <iostream>

bool isHttpMethod(const std::string& method);

Location::Location() {}

Location::Location(std::istringstream& location_block) {

	std::string line;

	return_value.first = -1;
	autoindex = false;
	while (location_block.eof() == false)
	{
		std::getline(location_block, line);
		parse(line);
	}
	std::cout << *this;
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
	throw (std::invalid_argument("Error: unknown directive '" + directive + "'"));
}

void Location::checkValueFormat(const std::string& value) const {

	int pos = value.find(";");
	bool isInSemicolon = pos != std::string::npos;
	int size = static_cast<int>(value.size());

	if (isInSemicolon && (pos < size - 1 || size == 1))
		throw (std::invalid_argument("Error: invalid value format '" + value + "'"));
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
	redefineLastValue(value) ;
	root = value;
}

void Location::parseIndex(std::stringstream& ss) {

	std::string value;

	while (ss.eof() == false)
	{
		ss >> value;
		checkValueFormat(value);
		redefineLastValue(value);
		index.push_back(value);
	}
}

void Location::parseAutoindex(std::stringstream& ss) {

	static bool duplicated;
	std::string value;

	if (duplicated == true)
		throw (std::invalid_argument("Error: 'autoindex' directive is duplicate"));
	ss >> value;
	if (ss.eof() == false)
		throw (std::invalid_argument("Error: Invalid number of arguments in 'autoindex' directive"));
	checkAutoindexFormat(value);
	autoindex = value == "on" ? true : false;
	duplicated = true;
}

void Location::checkAutoindexFormat(std::string& value) const
{
	checkValueFormat(value);
	redefineLastValue(value);
	if (!(value == "on" || value == "off"))
		throw (std::invalid_argument("Error: Invalid value '" + value + "' in 'autoindex' directive"));
}

std::ostream& operator<<(std::ostream& out, Location& l)
{
	out << "-----http method-----\n";
	for (std::set<std::string>::iterator itr = l.http_methods.begin(); itr != l.http_methods.end(); itr++)
		out << *itr << " ";
	out << "\n-----return-----\n";
	out << "status code : " << l.return_value.first << "\nstring : " << l.return_value.second << "\n";
	out << "-----root-----\n" << l.root << "\n";
	out << "-----index-----\n";
	for (std::vector<std::string>::iterator itr = l.index.begin(); itr != l.index.end(); itr++)
		out << *itr << " ";
	out << "\n-----autoindex-----\n" << std::boolalpha << l.autoindex << std::endl;
	return out;
}