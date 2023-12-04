#ifndef POSTCGI_HPP
#define POSTCGI_HPP

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
#include "CgiData.hpp"
#include "ServerUtils.hpp"

#define BUFFSIZE 100000

class PostCgi {
	private :
		const HttpRequestMessage* request;
		const Config* config;
		std::string location_key;
		std::string rel_path;
		std::string abs_path;
		std::string file_extension;
		size_t content_length;

	public :
		PostCgi(const HttpRequestMessage* request, const Config* config);
		CgiData* cgipost();

		void check_request_line(std::vector<std::string> request_line);
		void check_header_field(std::map<std::string, std::vector<std::string> > header_field);

//check request header field fn
		void check_header_content_type(std::map<std::string, std::vector<std::string> > header_field);
		void check_header_content_length(std::map<std::string, std::vector<std::string> > header_field);

//cgi response
		std::string parent_read(int* pipe_write, int* pipe_read, pid_t pid);
		void child_write(int* pipe_write, int* pipe_read, Location location);
		void child_write_py(int* pipe_write, int* pipe_read, Location location);
		char** postCgiEnv();

		static HttpResponseMessage makeResponse(std::string cgiResponse);
		static int findStatusCode(std::istringstream& ss);
		static void parseHeaderLine(std::istringstream& ss, std::map<std::string, std::string>& header_fields);
};


#endif