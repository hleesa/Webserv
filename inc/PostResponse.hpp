#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sstream>
#include "HttpRequestMessage.hpp"
#include "Config.hpp"
#include "HttpResponseMessage.hpp"

class PostResponse {
	private :
		//HttpResponseMessage msg_response;
		int _status_code;
		std::map<std::string, std::string> _header_fields;
		std::string _message_body;

		std::string abs_path;
		std::string request_url;
		std::string content_type;
		size_t content_length;

	public :
		PostResponse();
		PostResponse(const PostResponse& other);
		PostResponse& operator=(const PostResponse& other);
		~PostResponse();

		HttpResponseMessage run(HttpRequestMessage msg, Config config);

		void check_request_line(std::vector<std::string> request_line, std::string root);
		bool directory_exists(const std::string& path);
		void check_header_field(std::map<std::string, std::vector<std::string> > header_field);

//save string and make reponse
		void saveStringToFile(std::string message_body);
		void make_post_response(Config config);
		std::string PostResponse::make_response_body(const std::string& file_path);


//check request header field fn
		void check_header_content_type(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_content_length(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_content_desposition(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_user_agent(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_authorization(std::map<std::string, std::vector<std::string> > header_field);


//make response header field fn
//		void make_header_location();
//		void make_header_content_type();
//		void make_header_content_length();
//		void make_header_set_cookie();
//		void make_header_allow();
//		void make_header_chache_control();
//		void make_header_access_control_allow_origin();
};


#endif