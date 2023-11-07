#ifndef POSTRESPONSE_HPP
#define POSTRESPONSE_HPP

#include <iostream>
#include <fstream>
#include "HttpRequestMessage.hpp"
#include "Config.hpp"
#include "HttpResponseMessage.hpp"

class PostResponse {
	private :
		HttpResponseMessage msg_response;
		int status_code;


	public :
		PostResponse();
		PostResponse(const PostResponse& other);
		PostResponse& operator=(const PostResponse& other);
		~PostResponse();

		HttpResponseMessage run(HttpRequestMessage msg, Config config);

		void check_request_line(std::vector<std::string> request_line, std::string root);
		void check_header_field(std::map<std::string, std::vector<std::string> > header_field);


//check request header field fn
		void check_header_content_type(std::string key, std::vector<std::string> value);
		void check_header_content_length(std::string key, std::vector<std::string> value);
		void check_header_content_desposition(std::string key, std::vector<std::string> value);
		void check_header_user_agent(std::string key, std::vector<std::string> value);
		void check_header_authorization(std::string key, std::vector<std::string> value);

//make response header field fn
		void make_header_location();
		void make_header_content_type();
		void make_header_content_length();
		void make_header_set_cookie();
		void make_header_allow();
		void make_header_chache_control();
		void make_header_access_control_allow_origin();
};


#endif