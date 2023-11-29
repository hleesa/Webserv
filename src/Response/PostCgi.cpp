#include "../../inc/PostCgi.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/Location.hpp"
#include <fcntl.h>
#include <iostream>
#include "../../inc/PostCgiPipePid.hpp"

#define BUFFSIZE 100000

PostCgi::PostCgi(const HttpRequestMessage* request, const Config* config) : Method(request, config){
	_status_code = 0;
	_message_body = "";
	rel_path = "";
	abs_path = "";
	content_type = "";
	content_length = 0;
}

HttpResponseMessage PostCgi::makeHttpResponseMessage(){

	if (request->getMethod() != "POST") {
		_status_code = 300;
	} else if (request->getMessageBody() == "") {
		_status_code = 300;
	} else {
		//요청 url 형태 및 실행 가능 여부 확인
		check_request_line(request->getRequestLine());
		//헤더 필드 확인 -> body가 있는 경우이기 때문에 필수 헤더 확인
		check_header_field(request->getHeaderFields());
	}

	if (isCgi() == true) {
		//cgi post 처리
		cgipost();
	}

	return HttpResponseMessage(_status_code, _header_fields, _message_body);
}

void PostCgi::check_request_line(std::vector<std::string> request_line) {
	std::string url_path;
//	size_t pos;

	url_path = request_line[1];

	//request URL_path -> rel_path 값찾아내기
	if (url_path.find(location_key) == 0) {
		rel_path = url_path.substr(location_key.size());
	} else {
		rel_path = url_path;
	}

	//abs_path 를 기준에 따라서 완성
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

void PostCgi::check_header_field(std::map<std::string, std::vector<std::string> > header_field) {
	check_header_content_type(header_field);
	check_header_content_length(header_field);
}

//check request header field fn
void PostCgi::check_header_content_type(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-type") == header_field.end()) {
		//_status_code = 400;
		//return ;
		throw 400;
	}
	if (header_field["content-type"].size() == 0) {
		throw 400;
	}
	content_type = header_field["content-type"][0];
	file_extention = MediaType::getExtention(header_field["content-type"][0]);
	if (file_extention == DEFAULT) {
		file_extention = config->getLocations()[location_key].getCgiExt();
		//_status_code = 400;
		//return;
		//throw 415;
	}
}

void PostCgi::check_header_content_length(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("transfer-encoding") != header_field.end() && header_field["transfer-encoding"].front() == "chunked") {
		content_length=request->getMessageBody().size();
	} else if ((header_field.find("content-length") != header_field.end()) && (header_field["content-length"].size() == 1)) {
		std::stringstream ss(header_field["content-length"][0]);
		if (!(ss >> content_length)) {
			throw 403;
		}
	} else {
		throw 403;
	}
}

PostCgiPipePid* PostCgi::cgipost() {
	std::ostringstream body_length;
	//파일 권한 안주면 실패됨
	// if (access(abs_path.c_str(), F_OK | X_OK) == -1) {
	// 	throw 400;
	// }
    int* pipe_ptoc = new int[2];
	int* pipe_ctop = new int[2];
	if (pipe(pipe_ptoc) == -1 || pipe(pipe_ctop) == -1) {
		throw 500;
	}
    pid_t* pid = new pid_t;
	*pid = fork();
	if (*pid == -1)
		throw 500;
	else if (!*pid) {
		if (location_key == "/cgi-bin") {
			child_write_py(pipe_ptoc, pipe_ctop, config->getLocations()[location_key]);
		} else {
			child_write(pipe_ptoc, pipe_ctop, config->getLocations()[location_key]);
		}
	}

    if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
        throw 500;
    }
    PostCgiPipePid* cgiPipePid = new PostCgiPipePid(pipe_ctop + 0, pipe_ptoc + 1, pid);
    return cgiPipePid;

//	_message_body = parent_read(pipe_ptoc, pipe_ctop, pid);

//	body_length << _message_body.size();
//	_header_fields["content-length"] = body_length.str();
//	_header_fields["content-type"] = request->getHeaderFields()["content-type"][0];
}

//std::string PostCgi::parent_read(int* pipe_ptoc, int* pipe_ctop, pid_t pid) {
//	std::string body;
//
//	if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
//		throw 500;
//	}
//
//	std::cout << "before parent write"<<std::endl;
//
//	if (write(pipe_ptoc[1], request->getMessageBody().c_str(), request->getMessageBody().size()) == -1) {
//		throw 500;
//	}
//	if (close(pipe_ptoc[1]) == -1) {
//		throw 500;
//	}
//	std::cout << "before parent read" <<std::endl;
//	char	recv_buffer[BUFFSIZE];
//	int		nByte;
//
///////////////////return 하는 부분
	//return (pipe, pipe, pid);



	//while ((nByte = read(pipe_ctop[0], recv_buffer, sizeof(recv_buffer))) > 0) {
	//	body.append(recv_buffer, nByte);
	//}
	//if (nByte == -1)
	//	throw 500;
	//if (close(pipe_ctop[0]) == -1) {
	//	throw 500;
	//}

	//int status;
	//if (waitpid(pid, &status, 0) == -1) {
	//	throw 500;
	//}	
	//if (WIFEXITED(status)) {
	//	int exit_status = WEXITSTATUS(status);
	//	if (exit_status == EXIT_FAILURE) {
	//		throw 500;
	//	}
	//} else {
	//	throw 500;
	//}
	//return body;
//}

void PostCgi::child_write(int* pipe_ptoc, int* pipe_ctop, Location location) {
	char** cgi_environ = postCgiEnv();
	char* path_name = strdup((location.getRoot() + '/' + location.getCgiPath()).c_str());
	char* const arguments[] = {path_name, NULL};

	if (close(pipe_ptoc[1]) == -1 || close(pipe_ctop[0]) == -1) {
		exit(EXIT_FAILURE);
	}
	if (dup2(pipe_ptoc[0], STDIN_FILENO) == -1 || dup2(pipe_ctop[1], STDOUT_FILENO) == -1) {
		exit(EXIT_FAILURE);
	}
	if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
		exit(EXIT_FAILURE);
	}
	if (execve(path_name, arguments, cgi_environ) == -1 ) {
		exit(EXIT_FAILURE);
	}
}

void PostCgi::child_write_py(int* pipe_ptoc, int* pipe_ctop, Location location) {
	char** cgi_environ = postCgiEnv();
	char* path_name = strdup((location.getCgiPath()).c_str());
	char* python_script = strdup(abs_path.c_str());
	char* const arguments[] = {path_name, python_script, NULL};

	if (close(pipe_ptoc[1]) == -1 || close(pipe_ctop[0]) == -1) {
		exit(EXIT_FAILURE);
	}
	if (dup2(pipe_ptoc[0], STDIN_FILENO) == -1 || dup2(pipe_ctop[1], STDOUT_FILENO) == -1) {
		exit(EXIT_FAILURE);
	}
	if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
		exit(EXIT_FAILURE);
	}
	if (execve(path_name, arguments, cgi_environ) == -1 ) {
		exit(EXIT_FAILURE);
	}
}

char** PostCgi::postCgiEnv() {
	std::map<std::string, std::string> env;
	std::stringstream port_string;
	std::stringstream content_length_string;
	port_string << config->getPort();
	content_length_string << request->getMessageBody().size();
	env["REQUEST_METHOD"] = "POST";
	env["CONTENT_LENGTH"] = content_length_string.str();
	env["CONTENT_TYPE"] = request->getHeaderFields()["content-type"][0];
	env["SERVER_NAME"] = request->getHeaderFields()["host"][0];
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["PATH_INFO"] = abs_path;
	env["PATH_TRANSLATED"] = "http://localhost:" + port_string.str() + request->getURL();
	env["QUERY_STRING"] = "";
	env["SERVER_PROTOCOL"] = "HTTP/1.1";
	env["SERVER_NAME"] = "webserv";
	env["SERVER_PORT"] = port_string.str();

	char ** cgi_env = new char* [env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator ite = env.begin(); ite != env.end(); ++ite) {
		std::string tmp = ite->first + '=' + ite->second;
		cgi_env[i] = new char[tmp.length() + 1];
		strcpy(cgi_env[i], tmp.c_str());
		++i;
	}
	cgi_env[i] = NULL;
	return cgi_env;
}
