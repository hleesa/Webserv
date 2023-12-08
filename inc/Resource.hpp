#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include "Location.hpp"

class Resource {
	private:
		std::string path;
		bool isDirectoryList;

		std::string read() const;
		std::string makeDirectoryList();

	public:
		Resource();
		Resource(const std::string& resource_path, const bool isDirectoryList);
		Resource(const Resource& other);
		~Resource();
		Resource& operator=(const Resource& other);
		
		std::string getPath() const;

		std::string make();
};

#endif
