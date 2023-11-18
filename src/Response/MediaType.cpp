#include "../../inc/MediaType.hpp"

void MediaType::init(std::map<std::string, std::string>& mime_table) {
	mime_table[".bin"] = "application/octet-stream";
	mime_table[".css"] = "text/css";
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

std::string MediaType::getType(const std::string resource_path) {
	static std::map<std::string, std::string> mime_table;
	int pos = resource_path.rfind(".");
	if (pos == std::string::npos) {
		return DEFAULT;
	}
	std::string filename_extenstion = resource_path.substr(pos);
	init(mime_table);
	if (mime_table.find(filename_extenstion) == mime_table.end()) {
		return DEFAULT;
	}
	return mime_table[filename_extenstion];
}

std::string MediaType::getExtention(const std::string content_type) {
	static std::map<std::string, std::string> mime_table;
	std::map<std::string, std::string>::const_iterator ite;

	init(mime_table);
	for (ite = mime_table.begin(); ite != mime_table.end(); ++ite) {
		if (ite->second == content_type) {
			return ite->first;
		}
	}
	return DEFAULT;
}