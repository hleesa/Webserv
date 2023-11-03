#ifndef REQUESTASSEMBLER_HPP
#define REQUESTASSEMBLER_HPP

#include <vector>
#include <string>
#include <map>
#include "ReadingStatus.hpp"
#include "HttpRequestMessage.hpp"

#include <iostream>

#define NUM_OF_ALLOW_METHOD 3
#define NUM_OF_VERSION 3

enum BodyStatus {
	None,
	ENCODING,
	ContentLength,
	MustNot
};

struct Body {
	BodyStatus status;
	std::string content;
	int length;

	Body() {
		status = None;
		content.clear();
		length = 0;
	};
};

struct AssembleInfo {
	std::vector<std::string> request_line;
    std::map<std::string, std::vector<std::string> > header_fields;
	int status_code;

	std::string buffer;
	ReadingStatus status;
	std::string method;
	Body body;

	AssembleInfo() {
		request_line.clear();
		header_fields.clear();
		buffer.clear();
		status = StartLine;
	}
};

class RequestParser {
	private:
		std::map<int, AssembleInfo> map;

		bool processStartLine(AssembleInfo& info);
		std::string getMethod(const std::string& line);
		bool processHeader(AssembleInfo& info);
		BodyStatus getBodyStatus(const std::string line, Body& body);
		bool hasTransferEncoding(const std::string line);
		int getContentLength(const std::string line);
		bool isCRLF(const std::string line);
		ReadingStatus processBody(AssembleInfo& info);
		ReadingStatus processEncoding(Body& body, std::string& buffer);
		int getChunkedSize(std::string& buffer, int& pos);
		ReadingStatus processContentLength(Body& body, std::string& buffer);
		
		void trimCarriageReturn(std::string& line);
		void convertLowerCase(std::string& string);
    
		void parseRequestLine(AssembleInfo& info, const std::string& line);
		void parseHeaderFields(AssembleInfo& info, const std::string line);

	public:
		RequestParser();
		RequestParser(const RequestParser& other);
		RequestParser& operator=(const RequestParser& other);
		~RequestParser();
		
		ReadingStatus getReadingStatus(const int ident);
		HttpRequestMessage getHttpRequestMessage(const int ident);
		void run(const int ident, const char* newContent);
		void checkReadingStatus(AssembleInfo& info);
		void clear(const int ident);

};


#endif