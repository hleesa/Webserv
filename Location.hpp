
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
	
	void parse(std::vector<std::string>& line);
	void parseReturnValue(std::vector<std::string>& line);
	void parseRoot(std::vector<std::string>& line);
	void parseIndex(std::vector<std::string>& line);
	void parseAutoindex(std::vector<std::string>& line);

	void checkDuplicated(const bool& duplicated, const std::string& directive);
	void checkInvalidNumber(unsigned int size, unsigned int expected, const std::string& directive);
	void setReturnCode(const std::string& value);
	void setReturnString(const std::string& value);
	void checkAutoindexFormat(const std::string& value) const;

public:
    Location();
    Location(std::vector<std::vector<std::string>>& location_block);
    Location(const Location& other);
	Location& operator=(const Location& other);
    ~Location();

	friend std::ostream& operator<<(std::ostream& out, Location& location);
};

std::ostream& operator<<(std::ostream& out, Location& location);


#endif //LOCATION_HPP
