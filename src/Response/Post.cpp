#include "../../inc/Post.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/Location.hpp"
#include <fcntl.h>
#include <iostream>

Post::Post(const HttpRequestMessage* request, const Config* config) : Method(request, config){
	_status_code = 0;
	_message_body = "";
	rel_path = "";
	abs_path = "";
	content_type = "";
	content_length = 0;
}

HttpResponseMessage Post::makeHttpResponseMessage(){

	ssize_t limit_body_size = config->getLimitBodySize();

	if (config->getLocations()[location_key].getLimitBodySize() != -1) {
		limit_body_size = config->getLocations()[location_key].getLimitBodySize();
	}
	if (request->getMethod() != "POST") {
		_status_code = 300;
	} else if (request->getBodySize() == 0) {
		_status_code = 300;
	} else if (limit_body_size > 0 && request->getBodySize() > static_cast<size_t>(limit_body_size)) {
		throw 413;
	} else {
		check_request_line(request->getRequestLine());
		check_header_field(request->getHeaderFields());
	}

	if (_status_code == 0) {
		saveToFile();
	}

	make_post_response();

	return HttpResponseMessage(_status_code, _header_fields, _message_body);
}

void Post::check_request_line(std::vector<std::string> request_line) {
	std::string url_path;
	size_t pos;

	if (request_line.size() != 3)
		throw 404;
	if (request_line[1].compare(0, 7, "http://")  == 0) {
		pos = request_line[1].find('/', 8);
		url_path = request_line[1].substr(pos);
	} else if (request_line[1].compare(0, 8, "https://") == 0) {
		pos = request_line[1].find('/', 9);
		url_path = request_line[1].substr(pos);
	} else if (request_line[1][0] == '/') {
		url_path = request_line[1];
	} else {
		throw 404;
		return;
	}

	if (url_path.find(location_key) == 0) {
		rel_path = url_path.substr(location_key.size());
	} else {
		rel_path = url_path;
	}

	if (config->getLocations()[location_key].getRoot() == "") {
		if (config->getRoot() != "") {
			abs_path = config->getRoot() + '/' + url_path;
		} else {
			abs_path = config->getLocations()["/"].getRoot() + '/' + url_path;
		}
	} else {
		abs_path = config->getLocations()[location_key].getRoot() + '/' + rel_path;
	}
}

void Post::check_header_field(std::map<std::string, std::vector<std::string> > header_field) {
	check_header_content_type(header_field);
	check_header_content_length(header_field);
}

//check request header field fn
void Post::check_header_content_type(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-type") == header_field.end()) {
		throw 400;
	}
	if (header_field["content-type"].size() == 0) {
		throw 400;
	}
	content_type = header_field["content-type"][0];
	file_extension = MediaType::getExtension(header_field["content-type"][0]);
	if (file_extension.empty()) {
		throw 415;
	}
}

void Post::check_header_content_length(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("transfer-encoding") != header_field.end() && header_field["transfer-encoding"].front() == "chunked") {
        content_length = request->getBodySize();
	} else if ((header_field.find("content-length") != header_field.end()) && (header_field["content-length"].size() == 1)) {
		std::stringstream ss(header_field["content-length"][0]);
		if (!(ss >> content_length)) {
			throw 403;
		}
	} else {
		throw 403;
	}
}

void Post::saveToFile() {
	std::string filename;
	if (abs_path.find(".", 1) == std::string::npos) {
		filename = generateFileName();
	} else {
		size_t pos = abs_path.rfind('/');
		filename = abs_path.substr(pos);
	}
	std::string data_path = abs_path + "/" + filename;
	std::ofstream file_write(data_path, std::ios::app);

	if (request->getBodySize() != content_length) {
		throw 411;
	}
	if (file_write.is_open()) {
		char body[request->getBodySize() + 1];
		body[request->getBodySize()] = '\0';
		std::memmove(body,request->getMessageBodyPtr(), request->getBodySize());
		file_write << body;
		file_write.close();
		_status_code = 200;
	} else {
		throw 403;
	}
}

std::string Post::generateFileName() {
	static size_t fileIndex = 0;

	std::stringstream filenameStream;
	if (file_extension != DEFAULT) {
		filenameStream << "File_No_" << fileIndex++ << file_extension;
	} else {
		filenameStream << "File_No_" << fileIndex++;
	}
	return filenameStream.str();
}

void Post::make_post_response() {
	std::ostringstream body_length;

	if ((_status_code >= 200 && _status_code < 300) || _status_code == 0) {
		 _message_body = "File uploaded successfully\n";
		if (content_type != DEFAULT)
			_header_fields["content-type"] = content_type;
	} else if (_status_code >= 300 && _status_code < 400) {
		_header_fields["location"] = request->getURL();
		_header_fields["content-type"] = "text/html";
	} else {
		throw 500;
	}
	_status_code = 200;
	body_length << _message_body.size();
	_header_fields["content-length"] = body_length.str();
}
