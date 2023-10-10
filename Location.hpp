
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
	void parseLimitBodySize(std::stringstream& ss);

	void checkDuplicated(const bool& duplicated, const std::string& directive);
	void checkInvalidNumber(const std::stringstream& ss, const std::string& directive);
	void setReturnCode(const std::string& value);
	void setReturnString(const std::string& value);
	void checkAutoindexFormat(const std::string& value) const;
	void setLimitBodySize(const std::string& value);

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
