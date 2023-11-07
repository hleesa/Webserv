#include "../../inc/Resource.hpp"
#include <unistd.h>
#include <fstream>
#include <iostream>

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

std::string Resource::read() const {
	std::ifstream file_in(path.c_str());
	std::string content;
	
	// if (!file_in.is_open()) {
	// }
	file_in.seekg(0, std::ios::end);
	int size = file_in.tellg();
	content.resize(size);
	file_in.seekg(0, std::ios::beg);
	file_in.read(&content[0], size);
	return content;
}
