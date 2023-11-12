#include "../../inc/Resource.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dirent.h>

Resource::Resource() {}

Resource::Resource(const std::string& resource_path, const ResourceStatus& status) {
	this->path = resource_path;
	this->status = status;
}

Resource::Resource(const Resource& other) {
	*this = other;
}

Resource::~Resource() {}

Resource& Resource::operator=(const Resource& other) {
	if (this != &other) {
		this->path = other.path;
		this->status = other.status;
	}
	return *this;
}

ResourceStatus Resource::getStatus() const {
	return this->status;
}

std::string Resource::getPath() const {
	return this->path;
}

std::string Resource::read() const {
	std::ifstream file_in(path.c_str());
	std::string content;
	
	if (!file_in.is_open()) {
		return "";
	}
	file_in.seekg(0, std::ios::end);
	int size = file_in.tellg();
	content.resize(size);
	file_in.seekg(0, std::ios::beg);
	file_in.read(&content[0], size);
	return content;
}

std::string Resource::makeDirectoryList() const {
	std::string content = "<!DOCTYPE html>\n" 
						 "<html lang=\"en\">\n"
						 "<head>\n"
						 "\t<meta charset=\"UTF-8\">\n"
						 "\t<meta name=\"viewport\"\n"
						 "\tcontent=\"width=device-width, initial-scale=1.0\">\n"
						 "\t<title>Directory Listing</title>\n"
						 "</head>\n<body>\n"
						 "\t<h1>Index of " + path + "</h1>\n\t<ul>\n";
    DIR *dir;
    struct dirent *entry;

	dir = opendir(path.c_str());
	if (dir == NULL) {
		// 예외처리
	}
	while ((entry = readdir(dir)) != NULL) {
		std::string name(entry->d_name);
		if (entry->d_type == DT_DIR) {
			name += "/";
		}
		content += "\t\t<li><a href=\"" + name + "\">" + name + "</a></li>\n";	
	}
	content += "\t\t</ul>\n</body>\n</html>\n";
    closedir(dir);
	return content;
}

std::string Resource::makeResource() const {
	// if (status == FOUND) {
	// 	return read();
	// }
	if (status == DirectoryList) {
		return makeDirectoryList();
	}
	return read();
}