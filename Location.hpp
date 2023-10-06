
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <stdexcept>

class Location {

protected:
    std::pair<int, std::string> return_value;
    std::string root;
    std::vector<std::string> index;
    bool autoindex;
    long limit_body_size;
	
	void parse(const std::string& line);
	void parseReturnValue(std::stringstream& ss);
	void parseRoot(std::stringstream& ss);
	void parseIndex(std::stringstream& ss);
	void parseAutoindex(std::stringstream& ss);

	void checkDuplicated(const bool& duplicated, const std::string& directive);
	void checkInvalidNumber(const std::stringstream& ss, const std::string& directive);
	void checkMethodFormat(std::string& method) const;
	void setReturnCode(std::string& value);
	void setReturnString(std::string& value);
	void checkAutoindexFormat(std::string& value) const;

public:
    Location();
    Location(std::istringstream& location_block);
    Location(const Location& other);
	Location& operator=(const Location& other);
    ~Location();

	friend std::ostream& operator<<(std::ostream& out, Location& location);
};

std::ostream& operator<<(std::ostream& out, Location& location);


#endif //LOCATION_HPP
