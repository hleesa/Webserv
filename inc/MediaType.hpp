#ifndef MEDIATYPE_HPP
#define MEDIATYPE_HPP

#include <map>
#include <string>

#define DEFAULT "multipart/related"

class MediaType {
	private:
		static void init(std::map<std::string, std::string>& extensionToType);

	public:
		static std::string getType(const std::string resource_path);
		static std::string getExtention(const std::string content_type);
};

#endif