#include "../../inc/Post.hpp"
#include "../../inc/MediaType.hpp"
#include "../../inc/Location.hpp"

Post::Post() {
	_status_code = 0;
	_message_body = "";
	abs_path = "";
	content_type = "";
	content_length = 0;
}

Post::Post(const Post& other) {
	*this = other;
}

Post& Post::operator=(const Post& other) {
	if (this != &other) {
		this->_status_code = other._status_code;
		this->abs_path = other.abs_path;
		this->content_type = other.content_type;
		this->content_length = other.content_length;
	}
	return *this;
}

Post::~Post() {}

HttpResponseMessage Post::run(HttpRequestMessage request_msg, Config config) {
	if (request_msg.getMethod() != "POST") {
		_status_code = 300;
		request_url = request_msg.getRequestLine()[1];
	} else if (request_msg.getMessageBody() == "") {
		//redirect to get
		//make
		_status_code = 300; 
		request_url = request_msg.getRequestLine()[1];
	} else {
		//요청 url 형태 및 실행 가능 여부 확인
		check_request_line(request_msg.getRequestLine(), config);
		//헤더 필드 확인 -> body가 있는 경우이기 때문에 필수 헤더 확인
		check_header_field(request_msg.getHeaderFields());
	}
	std::cout << "4." << location_key << std::endl;
	if (location_key == "/cgi-bin") {
		//cgi post 처리
		cgipost(config, request_msg.getHeaderFields());
	} else {
		//요청 헤더 파싱 후에 맞는 상황에 대하여  처리
		if (_status_code == 0) {
			saveToFile(request_msg.getMessageBody(), config);
		}
		//else if (content_type == "multipart/form-data" && _status_code == 0) {
		//	saveMultipartToFile(request_msg.getMessageBody(), config);
		//}
		make_post_response(config);
	}
	return HttpResponseMessage(_status_code, _header_fields, _message_body);
}

void Post::check_request_line(std::vector<std::string> request_line, Config config) {
	std::string rel_path;
	size_t pos;

	if (request_line.size() != 3)
		throw 404;
	if (request_line[1].compare(0, 7, "http://")  == 0) {
		pos = request_line[1].find('/', 8);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1].compare(0, 8, "https://") == 0) {
		pos = request_line[1].find('/', 9);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1][0] == '/') {
		rel_path = request_line[1];
	} else {
		//상위 디렉토리 확인의  경우는  제외
		//this->_status_code = 404;
		throw 404;
		return;
	}
	std::cout << "2." << rel_path << std::endl;
	if (rel_path.find("cgi") == std::string::npos) {
		//location 확인 후 그곳에서의 가능 메서드 확인
		location_key = find_loc_key(rel_path, config);
		if (config.getLocations()[location_key].isNotAllowedMethod("POST") == true) {
			throw 405;
		}
		abs_path = config.getRoot() + rel_path;
		if (!directory_exists(abs_path)) {
			throw 404;
		}
	} else {
		//cgi 인 경우
		location_key = find_cgi_loc_key(rel_path, config);
		if (location_key == "/")
			throw 405;
		abs_path = config.getCgiLocation().second.getRoot() + rel_path;
	}
	std::cout << "1." << location_key << std::endl;

	//해당 디렉토리가 있는지  확인
	std::cout << "5." << abs_path << std::endl;
}

std::string Post::find_loc_key(std::string rel_path, Config config) {
	std::string path_checker = rel_path;
	std::map<std::string, Location> locs = config.getLocations();
	size_t pos = path_checker.length();

	while (locs.find(path_checker) == locs.end()) {
		pos = path_checker.rfind('/', pos - 1);
		if (pos == 0)
			path_checker = '/';
		else
			path_checker = path_checker.substr(0, pos);
	}
	return path_checker;
}

std::string Post::find_cgi_loc_key(std::string rel_path, Config config) {
	std::string path_checker = rel_path;
	std::pair<std::string, CgiLocation> locs = config.getCgiLocation();
	size_t pos = path_checker.length();

	std::cout << "3." << path_checker << std::endl;
	while (locs.first != path_checker) {
		pos = path_checker.rfind('/', pos - 1);
		if (pos == 0)
			path_checker = '/';
		else
			path_checker = path_checker.substr(0, pos);
	}
	return path_checker;
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
	check_header_content_desposition(header_field);
	check_header_user_agent(header_field);
	check_header_authorization(header_field);
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
		//_status_code = 400;
		//return;
		throw 400;
	}
}

void Post::check_header_content_length(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-length") == header_field.end()) {
		throw 403;
	}
	if ((header_field["content-length"].size() != 1)) {
		throw 403;
	}
	try {
		content_length = std::stoi(header_field["content-length"][0]);
	} catch (const std::invalid_argument& e) {
		 throw 403;
	}
}
void Post::check_header_content_desposition(std::map<std::string, std::vector<std::string> > header_field){
	if (header_field.find("content-desposition") == header_field.end()) {
		return;
	}
	//처리 보류

}
void Post::check_header_user_agent(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-user-agent") == header_field.end()) {
		return;
	}
	//처리 보류

}
void Post::check_header_authorization(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-authorization") == header_field.end()) {
		return;
	}
	//처리 보류

}

void Post::cgipost(Config config, std::map<std::string, std::vector<std::string> > header_field) {
	std::ostringstream body_length;

	if (access(abs_path.c_str(), F_OK | X_OK) == -1) {
		throw 400;
	}
	int pipe_ptoc[2];
	int pipe_ctop[2];
	if (pipe(pipe_ptoc) == -1 || pipe(pipe_ctop) == -1) {
		throw 500;
	}
	pid_t pid = fork();
	if (pid == -1)
		throw 500;
	else if (pid) {
		_message_body = parent_read(pipe_ptoc, pipe_ctop, pid);
	} else {
		child_write(pipe_ptoc, pipe_ctop, config.getCgiLocation().second, header_field);
	}
	body_length << _message_body.size();
	_header_fields["content-length"] = body_length.str();
	_header_fields["content-type"] = "text/html";
}

std::string Post::parent_read(int* pipe_ptoc, int* pipe_ctop, pid_t pid) {
	std::string body;

	std::cout << "p1." << std::endl;
	if (close(pipe_ptoc[0]) == -1 || close(pipe_ctop[1] == -1)) {
		throw 500;
	}
	std::cout << "p2." << std::endl;
	if (write(pipe_ptoc[1], _message_body.c_str(), content_length) == -1) {
		throw 500;
	}
	std::cout << "p3." << std::endl;
	char	recv_buffer[BUFSIZ];
	int		nByte;
	while ((nByte = read(pipe_ctop[0], recv_buffer, sizeof(recv_buffer))) > 0) {
		body.append(recv_buffer, nByte);
	}
	std::cout << "p4." << std::endl;
	if (nByte == -1)
		throw 500;

	int status;
	std::cout << "p5." << std::endl;
	if (waitpid(pid, &status, 0) == -1) {
		throw 500;
	}
	std::cout << "p6." << std::endl;
	if (WIFEXITED(status)) {
		int exit_status = WEXITSTATUS(status);
		if (exit_status == EXIT_FAILURE) {
			throw 500;
		}
	} else {
		throw 500;
	}
	std::cout << "p7." << std::endl;
	return body;
}

void Post::child_write(int* pipe_ptoc, int* pipe_ctop, CgiLocation cgi_location, std::map<std::string, std::vector<std::string> > header_field) {
	char** cgi_environ = postCgiEnv(header_field);
	char* python_interpreter = strdup(cgi_location.getCgiPath().c_str());
	char* python_script = strdup(abs_path.c_str());
	char* const command[] = {python_interpreter, python_script, NULL};

	std::cout << "c1." << std::endl;
	if (close(pipe_ptoc[1]) == -1 || close(pipe_ctop[0]) == -1) {
		exit(EXIT_FAILURE);
	}
	std::cout << "c2." << std::endl;
	if (dup2(pipe_ptoc[0], STDIN_FILENO) == -1 || dup2(pipe_ctop[1], STDOUT_FILENO) == -1) {
	std::cout << "checker---->" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "c3." << std::endl;
	if (execve(python_interpreter, command, cgi_environ) == -1 ) {
		exit(EXIT_FAILURE);
	}
	std::cout << "c4." << std::endl;
	exit(EXIT_SUCCESS);
}

char** Post::postCgiEnv(std::map<std::string, std::vector<std::string> > header_field) {
	std::map<std::string, std::string> env;
	env["REQUEST_METHOD"] = "POST";
	env["CONTENT_LENGTH"] = header_field["content-length"][0];
	env["CONTENT_TYPE"] = header_field["content-type"][0];
	//env["SERVER_NAME"] = ""

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




void Post::saveToFile(std::string message_body, Config config) {
	std::string filename = generateFileName(config);
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

std::string Post::generateFileName(Config config) {
	static size_t fileIndex = 0;

	std::stringstream filenameStream;
	filenameStream << config.getPort() << "_" << config.getName()[0] << "_No_" << fileIndex++ << file_extention;
	return filenameStream.str();
}

void Post::saveMultipartToFile(std::string message_body, Config config) {
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

void Post::make_post_response(Config config) {
	std::string redirect_path = "docroot/redirect_page/";
	// std::string redirect_path = config.getRoot() + "/redirect_page/";
	std::string errorpage_path = config.getRoot() + "/error_pages/";
	std::ostringstream body_length;

	if ((_status_code >= 200 && _status_code < 300) || _status_code == 0) {
		_message_body = "File uploaded successfully\n";
		if (content_type != DEFAULT)
			_header_fields["content-type"] = content_type;
	} else if (_status_code >= 300 && _status_code < 400) {
		// 리다이렉션
		_header_fields["location"] = request_url;
		_header_fields["content-type"] = "text/html";

		// 리다이렉트 될 경우에 html 페이지 띄워준다.
		// _message_body = "This page will be redirected";
		_message_body = make_response_body(redirect_path + "redirection.html");
	} else {
		// 600 번대 이상의 코드 처리?
		throw 500;
	}
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

//make response header field fn
//void Post::make_header_location();
//void Post::make_header_content_type();
//void Post::make_header_content_length();
//void Post::make_header_set_cookie();
//void Post::make_header_allow();
//void Post::make_header_chache_control();
//void Post::make_header_access_control_allow_origin();