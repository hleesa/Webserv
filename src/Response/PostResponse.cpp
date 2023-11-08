#include "../../inc/PostResponse.hpp"

PostResponse::PostResponse() {}

PostResponse::PostResponse(const PostResponse& other) {
	*this = other;
}

PostResponse& PostResponse::operator=(const PostResponse& other) {
	if (this != &other) {
		
	}
	return *this;
}

PostResponse::~PostResponse() {}

HttpResponseMessage PostResponse::run(HttpRequestMessage request_msg, Config config) {
	if (request_msg.getMethod() != "POST") {
		return;
	} else if (request_msg.getMessageBody() == "") {
		//redirect to get
		//make
	} else {
		check_request_line(request_msg.getRequestLine(), config.getRoot());
		check_header_field(request_msg.getHeaderFields());
	}
}

void PostResponse::check_request_line(std::vector<std::string> request_line, std::string root) {
	std::string abs_path = root;
	std::string rel_path;
	size_t pos;


	if (request_line.size() != 3) {
		this->status_code = 404;
		return;
	}
	if (request_line[1].compare(0, 7, "http://") == 0) {
		pos = request_line[1].find('/', 8);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1].compare(0, 8, "https://")) {
		pos = request_line[1].find('/', 9);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1][0] == '/') {
		rel_path = request_line[1];
	}

	std::fstream file(.c_str());

	if (file.fail()) {
		this->status_code = 404;
		return;
	} else {
		//this->status_code = 303;
		file.close();
		return;
	}
}

void PostResponse::check_header_field(std::map<std::string, std::vector<std::string> > header_field) {
	std::map<std::string, std::vector<std::string> >::iterator ite;
	for (ite = header_field.begin(); ite != header_field.end(); ite++) {
		check_header_content_type(ite->first, ite->second);
		check_header_content_length(ite->first, ite->second);
		check_header_content_desposition(ite->first, ite->second);
		check_header_user_agent(ite->first, ite->second);
		check_header_authorization(ite->first, ite->second);
	}
}

//check request header field fn
void PostResponse::check_header_content_type(std::string key, std::vector<std::string> value) {
	if (key != "content_type") {
		return;
	}
}
void PostResponse::check_header_content_length(std::string key, std::vector<std::string> value) {
	if (key != "content_length") {
		return;
	}

}
void PostResponse::check_header_content_desposition(std::string key, std::vector<std::string> value){
	if (key != "content_desposition") {
		return;
	}

}
void PostResponse::check_header_user_agent(std::string key, std::vector<std::string> value) {
	if (key != "content_user_agent") {
		return;
	}

}
void PostResponse::check_header_authorization(std::string key, std::vector<std::string> value) {
	if (key != "content_authorization") {
		return;
	}

}

//make response header field fn
void PostResponse::make_header_location();
void PostResponse::make_header_content_type();
void PostResponse::make_header_content_length();
void PostResponse::make_header_set_cookie();
void PostResponse::make_header_allow();
void PostResponse::make_header_chache_control();
void PostResponse::make_header_access_control_allow_origin();