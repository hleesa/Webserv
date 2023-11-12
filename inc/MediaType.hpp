#ifndef MIME_HPP
#define MIME_HPP

#include <map>
#include <string>

#define DEFAULT "multipart/related"

class MediaType {
	private:
		static void init(std::map<std::string, std::string>& extensionToType);

	public:
		static std::string getType(const std::string resource_path);
};

#endif