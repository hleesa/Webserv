
#include "Location.hpp"

#include <iostream>

bool isInteger(const std::string& string);

Location::Location() {}

Location::Location(std::istringstream& location_block) {

	std::string line;

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
	// std::cout << "directive : " << directive << std::endl;
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

void Location::checkDuplicated(const bool& duplicated, const std::string& directive)
{
	if (duplicated == true)
		throw (std::invalid_argument("Error: '" + directive + "' directive is duplicate"));
}

void Location::checkInvalidNumber(const std::stringstream& ss, const std::string& directive)
{
	if (ss.eof() == false)
		throw (std::invalid_argument("Error: Invalid number of arguments in '" + directive + "' directive"));
}

void Location::parseReturnValue(std::stringstream& ss) {

	std::string value;

	ss >> value;
	setReturnCode(value);
	if (ss.eof())
		return ;
	ss >> value;
	checkInvalidNumber(ss, "return");
	setReturnString(value);
}

void Location::setReturnCode(std::string& value) {
	
	if (isInteger(value) == false)
		throw (std::invalid_argument("Error: Invalid return code '" + value + "'"));
	std::stringstream ss(value);
	ss >> return_value.first;
}

void Location::setReturnString(std::string& value) {
	
	return_value.second = value;
}

void Location::parseRoot(std::stringstream& ss) {

	static bool duplicated = false;
	std::string value;

	checkDuplicated(duplicated, "root");
	ss >> value;
	checkInvalidNumber(ss, "root");
	root = value;
}

void Location::parseIndex(std::stringstream& ss) {

	std::string value;

	while (ss.eof() == false)
	{
		ss >> value;
		index.push_back(value);
	}
}

void Location::parseAutoindex(std::stringstream& ss) {

	static bool duplicated = false;
	std::string value;

	checkDuplicated(duplicated, "autoindex");
	ss >> value;
	checkInvalidNumber(ss, "autoindex");
	checkAutoindexFormat(value);
	autoindex = value == "on" ? true : false;
	duplicated = true;
}

void Location::checkAutoindexFormat(std::string& value) const
{
	if (!(value == "on" || value == "off"))
		throw (std::invalid_argument("Error: Invalid value '" + value + "' in 'autoindex' directive"));
}

std::ostream& operator<<(std::ostream& out, Location& l)
{
	out << "\n-----return-----\n";
	out << "status code : " << l.return_value.first << "\nstring : " << l.return_value.second << "\n";
	out << "-----root-----\n" << l.root << "\n";
	out << "-----index-----\n";
	for (std::vector<std::string>::iterator itr = l.index.begin(); itr != l.index.end(); itr++)
		out << *itr << " ";
	out << "\n-----autoindex-----\n" << std::boolalpha << l.autoindex << std::endl;
	return out;
}