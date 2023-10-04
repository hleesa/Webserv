
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <stdexcept>

class Location {

protected:
    std::set<std::string> http_methods;
    std::pair<int, std::string> return_value;
    std::string root;
    std::vector<std::string> index;
    bool autoindex;
	
	void parse(const std::string& line);
	void parseMethod(std::stringstream& ss);
	void parseReturnValue(std::stringstream& ss);
	void parseRoot(std::stringstream& ss);
	void parseIndex(std::stringstream& ss);
	void parseAutoindex(std::stringstream& ss);
	void checkValueFormat(const std::string& value) const;

	void checkMethodFormat(std::string& method) const;

public:
    Location();
    Location(std::istringstream& location_block);
    Location(const Location& other);
	Location& operator=(const Location& other);
    ~Location();
};


#endif //LOCATION_HPP
