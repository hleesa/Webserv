#include "../../inc/Post.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/Location.hpp"
#include <fcntl.h>
#include <iostream>

#define BUFFSIZE 100000

Post::Post(const HttpRequestMessage* request, const Config* config) : Method(request, config){
	_status_code = 0;
	_message_body = "";
	rel_path = "";
	abs_path = "";
	content_type = "";
	content_length = 0;
}

HttpResponseMessage Post::makeHttpResponseMessage(){
	//std::map<std::string, std::vector<std::string> > header = request->getHeaderFields();
	//std::map<std::string, std::vector<std::string> > ::iterator itr = header.begin();
	//for (; itr != header.end(); itr++) {
	//	std::cout << itr->first << " : " << itr->second[0] << std::endl;
	//}
	//std::cout << request->getMessageBody().size() << std::endl;

	set_member();

	std::cout << _status_code << std::endl;

	return HttpResponseMessage(_status_code, _header_fields, _message_body);
}

void Post::set_member() {
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
	} else {
		//요청 헤더 파싱 후에 맞는 상황에 대하여  처리
		if (_status_code == 0) {
			saveToFile(request->getMessageBody());
		}
		//else if (content_type == "multipart/form-data" && _status_code == 0) {
		//	saveMultipartToFile(request.getMessageBody());
		//}
	}
	make_post_response();
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
		//상위 디렉토리 확인의  경우는  제외
		throw 404;
		return;
	}

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

bool Post::directory_exists(const std::string& path) {
	struct stat info;

	if (stat(path.c_str(), &info) != 0) {
		return false;
	} 
	return (info.st_mode & S_IFDIR) != 0;
}

void Post::check_header_field(std::map<std::string, std::vector<std::string> > header_field) {
	check_header_content_type(header_field);
	check_header_content_length(header_field);
}

//check request header field fn
void Post::check_header_content_type(std::map<std::string, std::vector<std::string> > header_field) {
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

void Post::check_header_content_length(std::map<std::string, std::vector<std::string> > header_field) {
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

void Post::cgipost() {
	std::ostringstream body_length;
	// if (access(abs_path.c_str(), F_OK | X_OK) == -1) {
	// 	throw 400;
	// }
	int pipe_ptoc[2];
	int pipe_ctop[2];
	if (pipe(pipe_ptoc) == -1 || pipe(pipe_ctop) == -1) {
		throw 500;
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
	_message_body = parent_read(pipe_ptoc, pipe_ctop, pid);

	body_length << _message_body.size();
	_header_fields["content-length"] = body_length.str();
	_header_fields["content-type"] = request->getHeaderFields()["content-type"][0];
}

std::string Post::parent_read(int* pipe_ptoc, int* pipe_ctop, pid_t pid) {
	std::string body;
	// size_t		count_body = 0;
	// int		write_let;

	if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1]) == -1) {
		throw 500;
	}
	// while (1) {
	// 	write_let = write(pipe_ptoc[1], request->getMessageBody().c_str() + count_body, BUFSIZ);
	// 	if (write_let == -1) {
	// 		//throw 500;
	// 	}
	// 	count_body += write_let;
	// 	if (count_body == request->getMessageBody().size()) {
	// 		break;
	// 	}
	// }

	std::cout << "before parent write"<<std::endl;
	if (write(pipe_ptoc[1], request->getMessageBody().c_str(), request->getMessageBody().size()) == -1) {
		throw 500;
	}

	if (close(pipe_ptoc[1]) == -1) {
		throw 500;
	}
	std::cout << "before parent read" <<std::endl;
	char	recv_buffer[BUFFSIZE];
	int		nByte;
	while ((nByte = read(pipe_ctop[0], recv_buffer, sizeof(recv_buffer))) > 0) {
		body.append(recv_buffer, nByte);
	}
	if (nByte == -1)
		throw 500;
	if (close(pipe_ctop[0]) == -1) {
		throw 500;
	}

	int status;
	if (waitpid(pid, &status, 0) == -1) {
		throw 500;
	}	
	if (WIFEXITED(status)) {
		int exit_status = WEXITSTATUS(status);
		if (exit_status == EXIT_FAILURE) {
			throw 500;
		}
	} else {
		throw 500;
	}
	return body;
}

void Post::child_write(int* pipe_ptoc, int* pipe_ctop, Location location) {
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

void Post::child_write_py(int* pipe_ptoc, int* pipe_ctop, Location location) {
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

char** Post::postCgiEnv() {
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

void Post::saveToFile(std::string message_body) {
	std::string filename;
	if (abs_path.find(".") == std::string::npos) {
		filename = generateFileName(config);
	} else {
		size_t pos = abs_path.rfind('/');
		filename = abs_path.substr(pos);
	}
	std::string data_path = abs_path + "/" + filename;
	std::ofstream file_write(data_path, std::ios::app);

	if (message_body.size() != content_length) {
		// 헤더 콘텐츠 길이와 실제 바디의 글자수가 다를경우
		throw 411;
	}
	if (file_write.is_open()) {
		file_write << message_body;
		file_write.close();
		_status_code = 200;
	} else {
		throw 403; // wirte fail
	}
}

std::string Post::generateFileName(const Config* config) {
	static size_t fileIndex = 0;

	std::stringstream filenameStream;
	filenameStream << config->getPort() << "_" << config->getName()[0] << "_No_" << fileIndex++ << file_extention;
	return filenameStream.str();
}

void Post::saveMultipartToFile(std::string message_body) {
	std::string filename = generateFileName(config);
	std::string data_path = abs_path + "/" + filename;
	std::ofstream file_write(data_path, std::ios::app);
	std::string in_boundary;

	if (file_write.is_open()) {
		size_t start_boundary = message_body.find(boundary);
		if (start_boundary != std::string::npos) {
			size_t end_boundary = message_body.find(boundary, start_boundary + 1);
			if (end_boundary != std::string::npos) {
				in_boundary = message_body.substr(start_boundary + boundary.size(), end_boundary - (start_boundary + boundary.size()));
				file_write << in_boundary;
				file_write.close();
				_status_code = 200;
			} else {
				throw 400;
			}
		} else {
			throw 400;
		}
	} else {
		throw 400;
	}
}

void Post::make_post_response() {
	std::string redirect_path = "docroot/redirect_page/";
	// std::string redirect_path = config.getRoot() + "/redirect_page/";
	std::string errorpage_path = config->getRoot() + "/error_pages/";
	std::ostringstream body_length;

	if ((_status_code >= 200 && _status_code < 300) || _status_code == 0) {
		// _message_body = "File uploaded successfully\n";
		if (content_type != DEFAULT)
			_header_fields["content-type"] = content_type;
	} else if (_status_code >= 300 && _status_code < 400) {
		// 리다이렉션
		_header_fields["location"] = request->getURL();
		_header_fields["content-type"] = "text/html";

		// 리다이렉트 될 경우에 html 페이지
		// _message_body = "This page will be redirected";
		_message_body = make_response_body(redirect_path + "redirection.html");
	} else {
		// 600 번대 이상의 코드 처리?
		throw 500;
	}
	_status_code = 200;
	body_length << _message_body.size();
	_header_fields["content-length"] = body_length.str();
}

std::string Post::make_response_body(const std::string& file_path) {
	std::ifstream input_file(file_path, std::ios::binary);

	if(!input_file.is_open()) {
		// error page -> 필수로 있어야함.
		return "";
	}
	input_file.seekg(0, std::ios::end);
	std::streampos file_size = input_file.tellg();
	input_file.seekg(0, std::ios::beg);

	std::string content;
	content.resize(static_cast<std::size_t>(file_size));
	input_file.read(&content[0], file_size);
	input_file.close();
	return content;
}
