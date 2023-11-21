#include "../../inc/RequestParser.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

RequestParser::RequestParser() {}

RequestParser::RequestParser(const RequestParser& other) {
	*this = other;
}

RequestParser& RequestParser::operator=(const RequestParser& other) {
	if (this != &other)
	{
		this->parsing_data = other.parsing_data;
	}
	return *this;
}

RequestParser::~RequestParser() {}

ReadingStatus RequestParser::getReadingStatus(const int ident) {
	return parsing_data[ident].status;
}

HttpRequestMessage RequestParser::getHttpRequestMessage(const int ident, const long limit_body_size) {
	ParsingData data = parsing_data[ident];
    
	if (data.body.content.size() > static_cast<unsigned long>(limit_body_size)) {
		data.status_code = 413;
	}	
	return HttpRequestMessage(data.request_line, data.header_fields, data.body.content, data.status_code);
}

void RequestParser::run(const int ident, const char* newContent) {
	ParsingData *data = &parsing_data[ident];

	data->buffer += newContent;
	try {
		checkReadingStatus(*data);
    }
	catch (const int e) {
		data->status_code = e;
		data->status = END;
	}
}

void RequestParser::checkReadingStatus(ParsingData& data) {
	bool isRunning = true;

	if (data.status == StartLine) {
		isRunning = processStartLine(data);
	}
	if (!isRunning) {
		return ;
	}
	if (data.status == HEADER) {
		isRunning = processHeader(data);
	}
	if (!isRunning) {
		return ;
	}
	if (data.status == BODY) {
		data.status = processBody(data);
	}
}

bool RequestParser::processStartLine(ParsingData& data) {
	std::string line;
	unsigned long pos;
	
	pos = data.buffer.find('\n');
	if (pos == std::string::npos) {
		return false;
	}
	line = data.buffer.substr(0, pos);
	trimCarriageReturn(line);
	if (line.empty()) {
		data.buffer = data.buffer.substr(pos + 1);
		return false;
	}
	data.status = HEADER;
	data.buffer = data.buffer.substr(pos + 1);
	data.method = getMethod(line);
	if (data.method != "POST")
		data.body.status = MustNot;
	parseRequestLine(data, line);
	return true;
}

bool RequestParser::processHeader(ParsingData& data) {
	std::string line;
	unsigned long pos;
	
	pos = data.buffer.find('\n');
	if (pos == std::string::npos)
		return false;
	line = data.buffer.substr(0, pos);
	trimCarriageReturn(line);
	while (!isCRLF(line)) {
		data.body.status = getBodyStatus(line, data.body);
		parseHeaderFields(data, line);	
		data.buffer = data.buffer.substr(pos + 1);
		pos = data.buffer.find('\n');
		if (pos == std::string::npos)
			return false;
		line = data.buffer.substr(0, pos);
		trimCarriageReturn(line);
	}
	if (isCRLF(line)) {
		data.status = BODY;
		data.buffer = data.buffer.substr(pos + 1);
		if (data.header_fields.find("host") == data.header_fields.end())
        	throw 400;
	}
	return true;
}

ReadingStatus RequestParser::processBody(ParsingData& data) {
	if (data.body.status == MustNot) {
		return END;
	}
	if (data.body.status == None && data.method == "POST") {
		throw 411;
	}
	if (data.body.status == ENCODING) {
		return processEncoding(data.body, data.buffer);
	}
	if (data.body.status == ContentLength) {
		return processContentLength(data.body, data.buffer);
	}
	return BODY;
}

ReadingStatus RequestParser::processEncoding(Body& body, std::string& buffer) {
	int size_pos;
	int size;
	int pos;
	std::string line;

	while ((size = getChunkedSize(buffer, size_pos)) > 0) {
		pos = buffer.substr(size_pos + 1).find("\n");
		if (static_cast<unsigned long>(pos) == std::string::npos) {
			return BODY;
		}
		line = buffer.substr(size_pos + 1, pos);
		trimCarriageReturn(line);
		buffer = buffer.substr(size_pos + pos + 2);
		if (line.size() != static_cast<unsigned long>(size)) {
			throw 400;
		}
		body.content += line;
	}
	if (size == -1) {
		return BODY;
	}
	// size == 0
	if (buffer.size() < 5) {
		return BODY;
	}
	pos = buffer.substr(size_pos + 1).find("\n");
	if (static_cast<unsigned long>(pos) == std::string::npos) {
		buffer = buffer.substr(size_pos + 1);
		throw 400;
	}
	line = buffer.substr(size_pos + 1, pos);
	trimCarriageReturn(line);
	buffer = buffer.substr(size_pos + pos + 2);
	if (!line.empty()) {
		throw 400;
	}
	return END;
}

int RequestParser::getChunkedSize(std::string& buffer, int& pos) {
	std::string line;
	
	pos = buffer.find("\n");
	if (static_cast<unsigned long>(pos) == std::string::npos) {
		return -1;
	}
	line = buffer.substr(0, pos);
	trimCarriageReturn(line);
	if (!isIntegerLiteral(line)) {
		throw 400;
	}
	return std::atoi(line.c_str());
}

ReadingStatus RequestParser::processContentLength(Body& body, std::string& buffer) {
	if (buffer.size() < (unsigned long)body.length)
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
	if (body.status == ContentLength) {
		return ContentLength;
	}
	int length = getContentLength(line);
	if (length >= 0) {
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
	if (line.back() == '\r')
		line.resize(line.size() - 1);
}

void RequestParser::clear(const int ident) {
	ParsingData *data = &parsing_data[ident];
	
	data->request_line.clear();
	data->header_fields.clear();
	data->status_code = 0;
	data->status = StartLine;
	data->method.clear();
	data->body = Body();
}

std::string RequestParser::getMethod(const std::string& line) {
	std::stringstream ss(line);
	std::string method;

	ss >> method;
	return method;
}

/* utils */

void convertLowerCase(std::string& string) {
	for (unsigned int idx = 0; idx < string.size(); idx++) {
		string[idx] = tolower(string[idx]);
	}
}

bool	isIntegerLiteral(std::string literal) {
	int	idx = 0;
	int	size = literal.size();

	while (idx < size && isdigit(literal[idx]))
		idx++;
	return idx == size;
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

void RequestParser::parseRequestLine(ParsingData& data, const std::string& line) {
    std::vector<std::string> request = tokenizeRequestLine(line);
    validateMethod(request.front());
    // validRequestTarget
    validateHttpVersion(request.back());
	data.request_line = request;
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

void RequestParser::parseHeaderFields(ParsingData& data, const std::string line) {
	std::vector<std::string> header_field = tokenizeHeaderFiled(line);
	if (!header_field.empty())
		data.header_fields[header_field.front()]
		.insert(data.header_fields[header_field.front()].end(), header_field.begin() + 1, header_field.end());
}
