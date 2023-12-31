#ifndef POST_HPP
#define POST_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <map>
#include "Method.hpp"
#include "Config.hpp"
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"

class Post : public Method {
	private :
		//응답 요소 : HttpResponseMessage msg_response에 들어갈 3개의 필드
		int _status_code;
		std::map<std::string, std::string> _header_fields;
		std::string _message_body;

		std::string rel_path;
		std::string abs_path;		//config->root + url_path
		std::string content_type;	//요청->type
		std::string file_extension;	//요청->type에 따른 확장자
		std::string boundary;
		size_t content_length;		//요청 바디 length

	public :
		Post(const HttpRequestMessage* request, const Config* config);

		virtual HttpResponseMessage makeHttpResponseMessage();

		void check_request_line(std::vector<std::string> request_line);
		void check_header_field(std::map<std::string, std::vector<std::string> > header_field);

//check request header field fn
		void check_header_content_type(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_content_length(std::map<std::string, std::vector<std::string> > header_field);

//save string and make reponse
		void saveToFile();
		std::string generateFileName();
		void make_post_response();
};

#endif