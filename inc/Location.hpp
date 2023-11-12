
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
	
	virtual void parse(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseHttpMethod(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseReturnValue(std::vector<std::string>& line);
	void parseRoot(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseIndex(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseAutoindex(std::vector<std::string>& line, std::set<std::string>& duplicated);

	void checkDuplicated(const std::set<std::string>& duplicated, const std::string& directive);
	void checkInvalidNumber(bool isValid, const std::string& directive);
	void checkHttpMethod(const std::string& value) const;
	void setReturnCode(const std::string& value);
	void setReturnString(const std::string& value);
	void checkAutoindexFormat(const std::string& value) const;

public:
    Location();
    Location(std::vector<std::vector<std::string> >& location_block);
    Location(const Location& other);
	Location& operator=(const Location& other);
    virtual ~Location();

	std::set<std::string> getHttpMethods() const;
    std::pair<int, std::string> getReturnValue() const;
    std::string getRoot() const;
    std::vector<std::string> getIndex() const;
    bool getAutoindex() const;
	
	bool hasReturnValue() const;
	bool isNotAllowedMethod(const std::string method) const;

	friend std::ostream& operator<<(std::ostream& out, Location& location);
};

std::ostream& operator<<(std::ostream& out, Location& location);


#endif //LOCATION_HPP
