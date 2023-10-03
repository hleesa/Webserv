
#include "Location.hpp"

#include <iostream>
#include <cctype>

Location::Location() {}

Location::Location(std::istringstream& location_block) : root("")
{
	std::string line;

	while (location_block.eof() == false)
	{
		std::getline(location_block, line, ';');
		parse(line);
	}
}

Location::Location(const Location &other)
{
	*this = other;
}

Location &Location::operator=(const Location &other)
{
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

void Location::parse(std::string &line) {
	std::stringstream ss(line);
	std::string	directive;
	
	ss >> directive;
	if (directive == "allow_methods") {
		parseMethod();
		return ;
	}
	if (directive == "return") {
		parseReturnValue();
		return ;
	}
	if (directive == "root") {
		parseRoot();
		return ;
	}
	if (directive == "index") {
		parseIndex();
		return ;
	}
	if (directive == "autoindex") {
		parseAutoindex();
		return ;
	}
	throw (std::invalid_argument("Error: Invalid directive '" + directive + "'"));
}

void Location::parseMethod() {

}

void Location::parseReturnValue() {

}

void Location::parseRoot() {

}

void Location::parseIndex() {

}

void Location::parseAutoindex() {

}