#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include <string>
#include "Location.hpp"
#include "ResourceStatus.hpp"

class Resource {
	private:
		std::string path;
		ResourceStatus status;
		//MIME
		
		std::string read() const;
		std::string makeDirectoryList() const;

	public:
		Resource();
		Resource(const std::string& resource_path, const ResourceStatus& status);
		Resource(const Resource& other);
		~Resource();
		Resource& operator=(const Resource& other);
		
		ResourceStatus getStatus() const;
		std::string getPath() const;

		std::string make() const;
};

bool isDirectory(const std::string& path);

#endif