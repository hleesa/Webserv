#include "../../inc/Post.hpp"

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
		check_request_line(request_msg.getRequestLine(), config.getRoot());
		check_header_field(request_msg.getHeaderFields());
	}
	//요청 헤더 파싱 후에 맞는 상황에 대하여  처리
	if (content_type == "text/plain" && _status_code == 0) {
		saveTextPlainToFile(request_msg.getMessageBody(), config);
	} else if (content_type == "multipart/form-data" && _status_code == 0) {
		saveMultipartToFile(request_msg.getMessageBody(), config);
	}

	make_post_response(config);
	return HttpResponseMessage(_status_code, _header_fields, _message_body);
}

void Post::check_request_line(std::vector<std::string> request_line, std::string root) {
	std::string rel_path;
	size_t pos;

	if (request_line.size() != 3) {
		this->_status_code = 404;
		return;
	}
	if (request_line[1].compare(0, 7, "http://")  == 0) {
		pos = request_line[1].find('/', 8);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1].compare(0, 8, "https://") == 0) {
		pos = request_line[1].find('/', 9);
		rel_path = request_line[1].substr(pos);
	} else if (request_line[1][0] == '/') {
		rel_path = request_line[1];
	} else {	//상위 디렉토리 확인의  경우는  제외
		this->_status_code = 404;
		return;
	}
	//해당 디렉토리가 있는지  확인
	abs_path = root + rel_path;

	if (!directory_exists(abs_path)) {
		_status_code = 404;
		return;
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
	check_header_content_desposition(header_field);
	check_header_user_agent(header_field);
	check_header_authorization(header_field);
}

//check request header field fn
void Post::check_header_content_type(std::map<std::string, std::vector<std::string> > header_field) {
	//HTML, JSON, TEXT data 세가지만 처리
	if (header_field.find("content-type") == header_field.end()) {
		_status_code = 400;
		return;
	} else if (header_field["content-type"][0] == "application/x-www-form-urlencoded") {
		//HTML 폼 데이터를 인코딩한 것으로, 기본적인 폼 제출 방식
		content_type = "application/x-www-form-urlencoded";
		return;
	} else if (header_field["content-type"][0] == "multipart/form-data" && header_field["content-type"].size() == 2) {
		//폼 데이터를 여러 부분으로 나눠서 전송할 때 사용됩니다. 주로 파일 업로드와 함께 사용
		content_type = "multipart/form-data";
		if (header_field["content-type"][1].find("boundary=") == 0) {
			boundary = header_field["content-type"][1].substr(9);
		} else {
			_status_code = 400;
		}
		return;
	} else if (header_field["content-type"][0] == "application/json") {
		//JSON 형식의 데이터를 전송할 때 사용
		content_type = "application/json";
		return;
	} else if (header_field["content-type"][0] == "text/plain") {
		//텍스트 데이터를 평문으로 전송
		content_type = "text/plain";
		return;
	} else if (header_field["content-type"][0] == "application/xml") {
		//XML 형식의 데이터를 전송할 때 사용
		//처리 안함
		content_type = "application/xml";

		return;
	} else if (header_field["content-type"][0] == "application/octet-stream") {
		//이진 데이터를 전송할 때 사용
		//처리 안함
		content_type = "application/octet-stream";

		return;
	} else {
		//post 요청이지만 content-type이 이상하기때문에 400에러 반환
		_status_code = 400;
		return;
	}
}
void Post::check_header_content_length(std::map<std::string, std::vector<std::string> > header_field) {
	if (header_field.find("content-length") == header_field.end()) {
		_status_code = 403;
		return;
	} else if ((header_field["content-length"].size() != 1)) {
		_status_code = 403;
		return;
	} else {
		try {
			content_length = std::stoi(header_field["content-length"][0]);
		} catch (const std::invalid_argument& e) {
			_status_code = 403;
			return;
		}
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

void Post::saveTextPlainToFile(std::string message_body, Config config) {
	std::string filename = generateTextPlainFileName(config);
	std::string data_path = abs_path + "/" + filename;
	std::ofstream file_write(data_path, std::ios::app);

	if (message_body.size() != content_length) {
		// 헤더 콘텐츠 길이와 실제 바디의 글자수가 다를경우
		_status_code = 411;
		return;
	}
	if (file_write.is_open()) {
		file_write << message_body;
		file_write.close();
		_status_code = 200;
	} else {
		_status_code = 403; // wirte fail
		return;
	}

}

std::string Post::generateTextPlainFileName(Config config) {
	static size_t fileIndex = 0;

	std::stringstream filenameStream;
	filenameStream << config.getPort() << "_" << config.getName()[0] << "_No_" << fileIndex++ << ".txt";
	return filenameStream.str();
}

void Post::saveMultipartToFile(std::string message_body, Config config) {
	std::string filename = generateTextPlainFileName(config);
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
				_status_code = 400;
			}
		} else {
			_status_code = 400;
		}
	} else {
		_status_code = 400;
	}
}


void Post::make_post_response(Config config) {
	std::string redirect_path = "docroot/redirect_page/";
	// std::string redirect_path = config.getRoot() + "/redirect_page/";
	std::string errorpage_path = config.getRoot() + "/error_pages/";
	std::ostringstream body_length;

	if ((_status_code >= 200 && _status_code < 300) || _status_code == 0) {
		if (content_type == "text/plain") {
			_message_body = "데이터가 성공적으로 저장되었습니다.\n";
			_header_fields["content-type"] = "text/plain";
		} else if (content_type == "multipart/form-data") {
			_message_body = "File uploaded successfully\n";
			_header_fields["content-type"] = "text/plain";
		} else if (content_type == "application/x-www-form-urlencoded") {
			_message_body = "제이슨 파일 처리가 필요합니다.\n";
			_header_fields["content-type"] = "application/jason";
		}
		//5가지 각 형식에 대한 처리 
	} else if (_status_code >= 300 && _status_code < 400) {
		// 리다이렉션
		_header_fields["location"] = request_url;
		_header_fields["content-type"] = "text/html";

		// 리다이렉트 될 경우에 html 페이지 띄워준다.
		// _message_body = "This page will be redirected";
		_message_body = make_response_body(redirect_path + "redirection.html");
	} else if (_status_code >= 400 && _status_code < 500) {
		_message_body = make_response_body(errorpage_path + "/400.html");
		_header_fields["content-type"] = "text/html";
	} else if (_status_code >= 500 && _status_code < 600) {
		_message_body = make_response_body(errorpage_path + "/400.html");
		_header_fields["content-type"] = "text/html";
	} else {
		// 600 번대 이상의 코드 처리?
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