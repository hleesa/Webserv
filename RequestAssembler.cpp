#include "RequestAssembler.hpp"
#include <sstream>

RequestAssembler::RequestAssembler() {}

RequestAssembler::RequestAssembler(const RequestAssembler& other) {
	*this = other;
}

RequestAssembler& RequestAssembler::operator=(const RequestAssembler& other) {
	if (this != &other)
	{
		this->map = other.map;
	}
	return *this;
}

RequestAssembler::~RequestAssembler() {}

ReadingStatus RequestAssembler::getReadingStatus(const int ident) {
	return map[ident].status;
}

std::vector<std::string> RequestAssembler::getContent(const int ident) {
	AssembleInfo info = map[ident];
	std::vector<std::string> result = info.contents;

	result.push_back(info.body.content);
	return result;
}

void RequestAssembler::run(const int ident, const unsigned char* newContent) {
	AssembleInfo *info = &map[ident];

	info->buffer += (char*)newContent;
	checkReadingStatus(*info);
}

void RequestAssembler::checkReadingStatus(AssembleInfo& info) {
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
	if (info.status == ERROR) {
		// error 처리
	}
}

bool RequestAssembler::processStartLine(AssembleInfo& info) {
	std::string line;
	int pos;
	
	pos = info.buffer.find('\n');
	if (pos == std::string::npos)
		return false;
	line = info.buffer.substr(0, pos);
	trimCarriageReturn(line);
	info.contents.push_back(line);
	info.status = HEADER;
	info.buffer = info.buffer.substr(pos + 1);
	info.method = getMethod(line);
	if (info.method != "POST")
		info.body.status = MustNot;
	return true;
}

bool RequestAssembler::processHeader(AssembleInfo& info) {
	std::string line;
	int pos;
	
	pos = info.buffer.find('\n');
	if (pos == std::string::npos)
		return false;
	line = info.buffer.substr(0, pos);
	trimCarriageReturn(line);
	while (!isCRLF(line)) {
		info.body.status = getBodyStatus(line, info.body);
		info.contents.push_back(line);
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
	}
	return true;
}

ReadingStatus RequestAssembler::processBody(AssembleInfo& info) {
	if (info.body.status == MustNot) {
		return END;
	}
	if (info.body.status == None && info.method == "POST") {
		return ERROR;
	}
	if (info.body.status == ENCODING) {
		return processEncoding(info.body, info.buffer);
	}
	if (info.body.status == ContentLength) {
		return processContentLength(info.body, info.buffer);
	}
	return BODY;
}

ReadingStatus RequestAssembler::processEncoding(Body& body, std::string& buffer) {
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
			return ERROR;
		}
		body.content += line;
		buffer = buffer.substr(tmp + pos + 2);
	}
	if (size == -1)
		return BODY;
	if (size == -2)
		return ERROR;
	// size == 0
	pos = buffer.substr(tmp + 1).find("\n");
	if (pos == std::string::npos) {
		return ERROR;
	}
	line = buffer.substr(tmp + 1, pos);
	trimCarriageReturn(line);
	return line.empty() ? END : ERROR;
}

int RequestAssembler::getChunkedSize(std::string& buffer, int& pos) {
	int size;
	std::string line;
	
	pos = buffer.find("\n");
	if (pos == std::string::npos) {
		return -1;
	}
	line = buffer.substr(0, pos);
	trimCarriageReturn(line);
	if (line.size() != 1 || !isdigit(line[0])) {
		return -2;
	}
	return line[0] - '0';
}

ReadingStatus RequestAssembler::processContentLength(Body& body, std::string& buffer) {
	if (buffer.size() < body.length)
		return BODY;
	body.content = buffer.substr(0, body.length);
	buffer = buffer.substr(body.length);
	return END;
}

BodyStatus RequestAssembler::getBodyStatus(const std::string line, Body& body) {
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

bool RequestAssembler::hasTransferEncoding(const std::string line) {
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

int RequestAssembler::getContentLength(const std::string line) {
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

bool RequestAssembler::isCRLF(const std::string line) {
	return line == "\r" || line.empty();
}

void RequestAssembler::trimCarriageReturn(std::string& line) {
	if (*--line.end() == '\r')
		line.resize(line.size() - 1);
}

void RequestAssembler::convertLowerCase(std::string& string) {
	for (unsigned int idx = 0; idx < string.size(); idx++) {
		string[idx] = tolower(string[idx]);
	}
}

void RequestAssembler::clear(const int ident) {
	AssembleInfo *info = &map[ident];
	
	info->contents.clear();
	info->status = StartLine;
	info->body = Body();	
}

std::string RequestAssembler::getMethod(const std::string& line) {
	std::stringstream ss(line);
	std::string method;

	ss >> method;
	return method;
}