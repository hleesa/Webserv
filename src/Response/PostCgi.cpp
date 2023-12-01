#include "../../inc/PostCgi.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/Location.hpp"
#include <fcntl.h>
#include <iostream>
#include "../../inc/CgiData.hpp"
#include "../../inc/ToString.hpp"

#define ERROR -1
#define BUFFSIZE 100000

PostCgi::PostCgi(const HttpRequestMessage* request, const Config* config) : request(request), config(config) {
	this->location_key = findLocationKey(config, request);
	rel_path = "";
	abs_path = "";
	content_length = 0;
}

//void sigchld_handler(int signo) {
//     SIGCHLD 시그널 핸들러
//     자식 프로세스의 종료를 처리
//    int status;
//    pid_t pid = waitpid(child_pid, &status, WNOHANG);
//    if (pid > 0) {
//         자식 프로세스 종료 후 로직
//         ...
//        std::cout << "Child process terminated: " << pid << std::endl;
//    }
//}

CgiData* PostCgi::cgipost() {
	//요청 url 형태 및 실행 가능 여부 확인
	check_request_line(request->getRequestLine());
	//헤더 필드 확인 -> body가 있는 경우이기 때문에 필수 헤더 확인
	check_header_field(request->getHeaderFields());



	//파일 권한 안주면 실패됨
	// if (access(abs_path.c_str(), F_OK | X_OK) == -1) {
	// 	throw 400;
	// }
	int* pipe_ptoc = new int[2];
	int* pipe_ctop = new int[2];
	if (pipe(pipe_ptoc) == -1 || pipe(pipe_ctop) == -1) {
		throw 500;
	}
	if (fcntl(pipe_ptoc[1], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
		throw (strerror(errno));
	}
	if (fcntl(pipe_ctop[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
		throw (strerror(errno));
	}

    pid_t pid = fork();
	if (pid == -1)
		throw 500;
	else if (!pid) {
		if (location_key == "/cgi-bin") {
			child_write_py(pipe_ptoc, pipe_ctop, config->getLocations()[location_key]);
		} else {
			child_write(pipe_ptoc, pipe_ctop, config->getLocations()[location_key]);
		}
	}
    if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
        throw 500;
    }

    CgiData* cgiPipePid = new CgiData(pipe_ctop, pipe_ptoc, pid, -1);
    return cgiPipePid;
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
	file_extension = config->getLocations()[location_key].getCgiExt();
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
	if (fcntl(pipe_ptoc[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
		throw (strerror(errno));
	}
	if (fcntl(pipe_ptoc[0], F_SETFL, O_NONBLOCK, FD_CLOEXEC) == ERROR) {
		throw (strerror(errno));
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
    if (request->getHeaderFields().find("x-secret-header-for-test") != request->getHeaderFields().end()) {
        if (!request->getHeaderFields()["x-secret-header-for-test"].empty()) {
            env["HTTP_X_SECRET_HEADER_FOR_TEST"] = request->getHeaderFields()["x-secret-header-for-test"].front();
        }
    }
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

HttpResponseMessage PostCgi::makeResponse(const std::string cgi_response) {
	std::istringstream ss(cgi_response);
	std::map<std::string, std::string> header_fields;
	
	//if (cgiResponse == "") {
	//	throw 400;
	//}
	int status_code = findStatusCode(ss);
	parseHeaderLine(ss, header_fields);
	std::string body;
	ss >> body;
	header_fields["Content-length"] = to_string(body.length());

	return HttpResponseMessage(status_code, header_fields, body);
}

int PostCgi::findStatusCode(std::istringstream& ss) {
	std::string line;
	std::getline(ss, line);

	return std::atoi(line.substr(8, 3).c_str());
}

void PostCgi::parseHeaderLine(std::istringstream& ss, std::map<std::string, std::string>& header_fields) {
	std::string line;
	std::string key;
	std::string contents;
	size_t pos;

	while (std::getline(ss, line) && line != "\r") {
		pos = line.find(':');
		key = line.substr(0, pos);
		contents = line.substr(pos + 1);
		if (!contents.empty() && *contents.rbegin() == '\r') {
            contents = contents.substr(0, contents.size() - 1);
        }
		header_fields.insert(std::make_pair(key, contents));
	}
}

//"Status: 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\nINPUT=HELLO"...