
#ifndef WEBSERV_CGILOCATION_HPP
#define WEBSERV_CGILOCATION_HPP

#include "Location.hpp"

class CgiLocation : public Location {
private:
    std::string cgi_path;
    std::string cgi_ext;

	virtual void parse(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseCgiPath(std::vector<std::string>& line, std::set<std::string>& duplicated);
	void parseCgiExt(std::vector<std::string>& line, std::set<std::string>& duplicated);

public:
	CgiLocation();
	CgiLocation(std::vector<std::vector<std::string> >& location_block);
	CgiLocation(const CgiLocation& other);
	virtual ~CgiLocation();
	CgiLocation& operator=(const CgiLocation& other);

	std::string getCgiPath() const;
    std::string getCgiExt() const;
};


#endif //WEBSERV_CGILOCATION_HPP
