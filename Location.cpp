
#include "Location.hpp"

#include <iostream>

bool isNumber(const std::string& string);

Location::Location() {}

Location::Location(std::vector<std::vector<std::string>>& location_block) {

	std::vector<std::vector<std::string>>::iterator itr;

	autoindex = false;
	for (itr = location_block.begin(); itr != location_block.end(); itr++)
	{
		parse(*itr);
	}
	// std::cout << *this;
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

void Location::parse(std::vector<std::string>& line) {

	std::string	directive;
	
	if (line.empty())
		return ;
	directive = *(line.begin());
	// std::cout << "directive : " << directive << std::endl;
	if (directive == "return") {
		parseReturnValue(line);
		return ;
	}
	if (directive == "root") {
		parseRoot(line);
		return ;
	}
	if (directive == "index") {
		parseIndex(line);
		return ;
	}
	if (directive == "autoindex") {
		parseAutoindex(line);
		return ;
	}
	throw (std::invalid_argument("Error: unknown directive '" + directive + "'"));
}

void Location::checkDuplicated(const bool& duplicated, const std::string& directive) {
	if (duplicated == true)
		throw (std::invalid_argument("Error: '" + directive + "' directive is duplicate"));
}

void Location::checkInvalidNumber(unsigned int size, unsigned int expected, const std::string& directive) {
	if (size > expected)
		throw (std::invalid_argument("Error: Invalid number of arguments in '" + directive + "' directive"));
}

void Location::parseReturnValue(std::vector<std::string>& line) {

	std::vector<std::string>::iterator itr = line.begin();

	checkInvalidNumber(line.size(), 3, "return");
	setReturnCode(*(++itr));
	if (line.size() == 2)
		return ;
	setReturnString(*(++itr));
}

void Location::setReturnCode(const std::string& value) {
	
	if (isNumber(value) == false)
		throw (std::invalid_argument("Error: Invalid return code '" + value + "'"));
	std::stringstream ss(value);
	ss >> return_value.first;
}

void Location::setReturnString(const std::string& value) {

	return_value.second = value;
}

void Location::parseRoot(std::vector<std::string>& line) {

	static bool duplicated = false;

	checkDuplicated(duplicated, "root");
	checkInvalidNumber(line.size(), 2, "root");
	root = line[1];
}

void Location::parseIndex(std::vector<std::string>& line) {

	std::vector<std::string>::iterator itr;

	for (itr = line.begin(); itr != line.end(); itr++)
		index.push_back(*(++itr));
}

void Location::parseAutoindex(std::vector<std::string>& line) {

	static bool duplicated = false;

	checkDuplicated(duplicated, "autoindex");
	checkInvalidNumber(line.size(), 2, "autoindex");
	checkAutoindexFormat(line[1]);
	autoindex = line[1] == "on" ? true : false;
	duplicated = true;
}

void Location::checkAutoindexFormat(const std::string& value) const {
	if (!(value == "on" || value == "off"))
		throw (std::invalid_argument("Error: Invalid value '" + value + "' in 'autoindex' directive"));
}

std::ostream& operator<<(std::ostream& out, Location& l) {
	out << "\n-----return-----\n";
	out << "status code : " << l.return_value.first << "\nstring : " << l.return_value.second << "\n";
	out << "-----root-----\n" << l.root << "\n";
	out << "-----index-----\n";
	for (std::vector<std::string>::iterator itr = l.index.begin(); itr != l.index.end(); itr++)
		out << *itr << " ";
	out << "\n-----autoindex-----\n" << std::boolalpha << l.autoindex << std::endl;
	out << "-----client max body size-----\n" << l.limit_body_size << "\n";
	return out;
}