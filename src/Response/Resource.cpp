#include "../../inc/Resource.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

Resource::Resource() {}

Resource::Resource(const std::string& resource_path, const bool isDirectoryList) {
	this->path = resource_path;
	this->isDirectoryList = isDirectoryList;
}

Resource::Resource(const Resource& other) {
	*this = other;
}

Resource::~Resource() {}

Resource& Resource::operator=(const Resource& other) {
	if (this != &other) {
		this->path = other.path;
		this->isDirectoryList = other.isDirectoryList;
	}
	return *this;
}

std::string Resource::getPath() const {
	return this->path;
}

std::string Resource::read() const {
	std::ifstream file_in(path.c_str());
	std::string content;
	
	if (!file_in.is_open()) {
		throw 500;
	}
	file_in.seekg(0, std::ios::end);
	int size = file_in.tellg();
	content.resize(size);
	file_in.seekg(0, std::ios::beg);
	file_in.read(&content[0], size);
	return content;
}

std::string Resource::makeDirectoryList() {
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
		throw 500;
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
	this->path += "/.html";
	return content;
}

std::string Resource::make() {
	if (isDirectoryList) {
		return makeDirectoryList();
	}
	return read();
}

bool isDirectory(const std::string& path) {
    struct stat file_info;
    
    if (stat(path.c_str(), &file_info) != 0) {
        return false;
    }
    return S_ISDIR(file_info.st_mode);
}
