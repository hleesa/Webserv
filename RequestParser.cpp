#include "RequestParser.hpp"
#include <sstream>
#include <algorithm>

RequestParser::RequestParser() {}

RequestParser::RequestParser(const RequestParser& other) {
	*this = other;
}

RequestParser& RequestParser::operator=(const RequestParser& other) {
	if (this != &other)
	{
		this->map = other.map;
	}
	return *this;
}

RequestParser::~RequestParser() {}

ReadingStatus RequestParser::getReadingStatus(const int ident) {
	return map[ident].status;
}

HttpRequestMessage RequestParser::getHttpRequestMessage(const int ident) {
	AssembleInfo info = map[ident];
    
	return HttpRequestMessage(info.request_line, info.header_fields, info.body.content, info.status_code);
}

void RequestParser::run(const int ident, const char* newContent) {
	AssembleInfo *info = &map[ident];

	info->buffer += newContent;
	try {
		checkReadingStatus(*info);
    }
	catch (const int e) {
		info->status_code = e;
		info->status = END;
	}
}

void RequestParser::checkReadingStatus(AssembleInfo& info) {
	bool isRunning = true;

	if (info.status == StartLine) {
		isRunning = processStartLine(info);
	}
	if (!isRunning) {
		return ;
	}
	if (info.status == HEADER) {
		isRunning = processHeader(info);
	}
	if (!isRunning) {
		return ;
	}
	if (info.status == BODY) {
		info.status = processBody(info);
	}
}

bool RequestParser::processStartLine(AssembleInfo& info) {
	std::string line;
	int pos;
	
	pos = info.buffer.find('\n');
	if (pos == std::string::npos) {
		return false;
	}
	line = info.buffer.substr(0, pos);
	trimCarriageReturn(line);
	if (line.empty()) {
		info.buffer = info.buffer.substr(pos + 1);
		return false;
	}
	info.status = HEADER;
	info.buffer = info.buffer.substr(pos + 1);
	info.method = getMethod(line);
	if (info.method != "POST")
		info.body.status = MustNot;
	parseRequestLine(info, line);
	return true;
}

bool RequestParser::processHeader(AssembleInfo& info) {
	std::string line;
	int pos;
	
	pos = info.buffer.find('\n');
	if (pos == std::string::npos)
		return false;
	line = info.buffer.substr(0, pos);
	trimCarriageReturn(line);
	while (!isCRLF(line)) {
		info.body.status = getBodyStatus(line, info.body);
		parseHeaderFields(info, line);	
		info.buffer = info.buffer.substr(pos + 1);
		pos = info.buffer.find('\n');
		if (pos == std::string::npos)
			return false;
		line = info.buffer.substr(0, pos);
		trimCarriageReturn(line);
	}
	if (isCRLF(line)) {
		info.status = BODY;
		info.buffer = info.buffer.substr(pos + 1);
		if (info.header_fields.find("host") == info.header_fields.end())
        	throw 400;
	}
	return true;
}

ReadingStatus RequestParser::processBody(AssembleInfo& info) {
	if (info.body.status == MustNot) {
		return END;
	}
	if (info.body.status == None && info.method == "POST") {
		throw 411;
	}
	if (info.body.status == ENCODING) {
		return processEncoding(info.body, info.buffer);
	}
	if (info.body.status == ContentLength) {
		return processContentLength(info.body, info.buffer);
	}
	return BODY;
}

ReadingStatus RequestParser::processEncoding(Body& body, std::string& buffer) {
	int tmp;
	int size;
	int pos;
	std::string line;

	while ((size = getChunkedSize(buffer, tmp)) > 0) {
		pos = buffer.substr(tmp + 1).find("\n");
		if (pos == std::string::npos) {
			return BODY;
		}
		line = buffer.substr(tmp + 1, pos);
		trimCarriageReturn(line);
		if (line.size() != size) {
			throw 400;
		}
		body.content += line;
		buffer = buffer.substr(tmp + pos + 2);
	}
	if (size == -1) {
		return BODY;
	}
	// size == 0
	pos = buffer.substr(tmp + 1).find("\n");
	if (pos == std::string::npos) {
		throw 400;
	}
	line = buffer.substr(tmp + 1, pos);
	trimCarriageReturn(line);
	if (!line.empty()) {
		throw 400;
	}
	return END;
}

int RequestParser::getChunkedSize(std::string& buffer, int& pos) {
	int size;
	std::string line;
	
	pos = buffer.find("\n");
	if (pos == std::string::npos) {
		return -1;
	}
	line = buffer.substr(0, pos);
	trimCarriageReturn(line);
	if (line.size() != 1 || !isdigit(line[0])) {
		throw 400;
	}
	return line[0] - '0';
}

ReadingStatus RequestParser::processContentLength(Body& body, std::string& buffer) {
	if (buffer.size() < body.length)
		return BODY;
	body.content = buffer.substr(0, body.length);
	buffer = buffer.substr(body.length);
	return END;
}

BodyStatus RequestParser::getBodyStatus(const std::string line, Body& body) {
	if (body.status == ENCODING || body.status == MustNot) {
		return body.status;
	}
	if (hasTransferEncoding(line)) {
		return ENCODING;
	}
	if (body.status == ContentLength)
		return ContentLength;
	int length = getContentLength(line);
	if (length > 0) {
		body.length = length;
		return ContentLength;	
	}
	return None;
}

bool RequestParser::hasTransferEncoding(const std::string line) {
	std::stringstream ss(line);
	std::string key("transfer-encoding");
	std::string tmp;

	std::getline(ss, tmp, ':');
	convertLowerCase(tmp);
	if (tmp != key) {
		return false;
	}
	ss >> tmp;
	return tmp == "chunked";
}

int RequestParser::getContentLength(const std::string line) {
	std::stringstream ss(line);
	std::string key("content-length");
	std::string tmp;
	int value;
	
	std::getline(ss, tmp, ':');
	convertLowerCase(tmp);
	if (tmp != key) {
		return 0;
	}
	ss >> value;
	return value;
}

bool RequestParser::isCRLF(const std::string line) {
	return line == "\r" || line.empty();
}

void RequestParser::trimCarriageReturn(std::string& line) {
	if (*--line.end() == '\r')
		line.resize(line.size() - 1);
}

void RequestParser::convertLowerCase(std::string& string) {
	for (unsigned int idx = 0; idx < string.size(); idx++) {
		string[idx] = tolower(string[idx]);
	}
}

void RequestParser::clear(const int ident) {
	AssembleInfo *info = &map[ident];
	
	info->request_line.clear();
	info->header_fields.clear();
	info->status_code = 0;
	info->status = StartLine;
	info->method.clear();
	info->body = Body();
}

std::string RequestParser::getMethod(const std::string& line) {
	std::stringstream ss(line);
	std::string method;

	ss >> method;
	return method;
}


/* parse */

bool isValidRequestLineFormat(const std::string& request_line) {
    if (request_line.front() == ' ' || request_line.back() == ' ')
        return false;
    std::vector<size_t> spaces;
    int num_of_other_whitespace = 0;
    for (size_t i = 0; i < request_line.size(); ++i) {
        char ch = request_line[i];
        if (!isspace(ch)) {
            continue;
        }
        else if (ch == ' ') {
            spaces.push_back(i);
        }
        else {
            ++num_of_other_whitespace;
        }
    }
    return (spaces.size() == 2 && num_of_other_whitespace == 0) && (spaces.back() - spaces.front() > 1);
}

std::vector<std::string> tokenizeRequestLine(const std::string& request_line) {
    if (!isValidRequestLineFormat(request_line)) {
		throw 400;
    }
    std::string token;
    std::vector<std::string> tokens;
    std::stringstream ss(request_line);
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void validateMethod(const std::string& method) {
    std::string allow_method_list[] = {"GET", "POST", "DELETE"};
    std::vector<std::string> allow_method(allow_method_list, allow_method_list + NUM_OF_ALLOW_METHOD);
    if (std::find(allow_method.begin(), allow_method.end(), method) == allow_method.end()) {
		throw 501;
    }
    return;
}

void validateHttpVersion(const std::string& http_version) {
    std::string http_version_list[] = {"HTTP/1.1", "HTTP/1.0", "HTTP/0.9"};
    std::vector<std::string> versions(http_version_list, http_version_list + NUM_OF_VERSION);
    if (std::find(versions.begin(), versions.end(), http_version) == versions.end()) {
		throw 505;
	}
    return;
}

void RequestParser::parseRequestLine(AssembleInfo& info, const std::string& line) {
    std::vector<std::string> request = tokenizeRequestLine(line);
    validateMethod(request.front());
    // validRequestTarget
    validateHttpVersion(request.back());
	info.request_line = request;
}

bool isVisibleString(const std::string& str) {
    for (std::string::const_iterator ch = str.begin(); ch != str.end(); ++ch) {
        if (!isprint(*ch))
            return false;
    }
    return true;
}

bool isValidFieldName(const std::string& field_name) {
    if (field_name.empty()) {
        return false;
    }
    for (std::string::const_iterator ch = field_name.begin(); ch != field_name.end(); ++ch) {
        if (isspace(*ch) || !isprint(*ch)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> tokenizeFieldValue(const std::string& field_value) {
    std::string new_field_value = field_value;
    std::string delim = ",;";
    for (std::string::iterator ch = new_field_value.begin(); ch != new_field_value.end(); ++ch) {
        if (delim.find(*ch) != std::string::npos) {
            *ch = ' ';
        }
    }
    std::stringstream ss(new_field_value);
    std::string token;
    std::vector<std::string> tokens;
    while (ss >> token) {
        if (isVisibleString(token)) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<std::string> tokenizeHeaderFiled(const std::string& header) {
    size_t colon_idx = header.find(":");
    if (colon_idx == std::string::npos) {
        return std::vector<std::string>();
    }
    std::string field_name = header.substr(0, colon_idx);
    std::vector<std::string> file_value = tokenizeFieldValue(header.substr(colon_idx + 1));
    if (!isValidFieldName(field_name)) {
        return std::vector<std::string>();
    }
    std::transform(field_name.begin(), field_name.end(), field_name.begin(), ::tolower);
    std::vector<std::string> tokens;
    tokens.push_back(field_name);
    tokens.insert(tokens.begin() + 1, file_value.begin(), file_value.end());
    return tokens;
}

void RequestParser::parseHeaderFields(AssembleInfo& info, const std::string line) {
	std::vector<std::string> header_field = tokenizeHeaderFiled(line);
	if (!header_field.empty())
		info.header_fields[header_field.front()]
		.insert(info.header_fields[header_field.front()].end(), header_field.begin() + 1, header_field.end());
}
