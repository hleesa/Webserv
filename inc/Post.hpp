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

		//std::string location_key_post_cgi;
		std::string rel_path;
		std::string abs_path;		//config->root + url_path
		std::string content_type;	//요청->type
		std::string file_extention;	//요청->type에 따른 확장자
		std::string boundary;
		size_t content_length;		//요청 바디 length


	public :
		Post(const HttpRequestMessage* request, const Config* config);

		virtual HttpResponseMessage makeHttpResponseMessage();
		void set_member();

		void check_request_line(std::vector<std::string> request_line);
		std::string find_loc_key(std::string rel_path);
		bool directory_exists(const std::string& path);
		void check_header_field(std::map<std::string, std::vector<std::string> > header_field);

//cgi response
		void cgipost();
		std::string parent_read(int* pipe_write, int* pipe_read, pid_t pid);
		void child_write(int* pipe_write, int* pipe_read, Location location);
		char** postCgiEnv();

//save string and make reponse
		void saveToFile(std::string message_body);
		std::string generateFileName(const Config* config);
		void saveMultipartToFile(std::string message_body);
		void make_post_response();
		std::string make_response_body(const std::string& file_path);

//check request header field fn
		void check_header_content_type(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_content_length(std::map<std::string, std::vector<std::string> > header_field);

};


#endif