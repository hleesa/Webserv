#include "../../inc/MediaType.hpp"

// text/
// img html txt json

void MediaType::init(std::map<std::string, std::string>& mime_table) {
	mime_table[".bin"] = "application/octet-stream";
	mime_table[".css"] = "text/css";
	mime_table[".gif"] = "image/gif";
	mime_table[".gif"] = "image/gif";
	mime_table[".htm"] = "text/html";
	mime_table[".html"] = "text/html";
	mime_table[".gz"] = "application/gzip";
	mime_table[".ico"] = "image/vnd.microsoft.icon";
	mime_table[".jpeg"] = "image/jpeg";
	mime_table[".jpg"] = "image/jpeg";
	mime_table[".js"] = "text/javascript";
	mime_table[".json"] = "application/json";
	mime_table[".mjs"] = "text/javascript";
	mime_table[".png"] = "image/png";
	mime_table[".pdf"] = "application/pdf";
	mime_table[".sh"] = "application/x-sh";
	mime_table[".txt"] = "text/plain";
	mime_table[".xhtml"] = "application/xhtml+xml";
	mime_table[".xml"] = "application/xml";
}
#include <iostream>

std::string MediaType::getType(const std::string resource_path) {
	static std::map<std::string, std::string> mime_table;
	std::string filename_extenstion = resource_path.substr(resource_path.rfind("."));

	init(mime_table);
	std::cout << "extensiont : " << filename_extenstion << std::endl;
	if (mime_table.find(filename_extenstion) == mime_table.end()) {
		return DEFAULT;
	}
	return mime_table[filename_extenstion];
}