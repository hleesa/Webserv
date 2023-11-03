#ifndef REQUESTASSEMBLER_HPP
#define REQUESTASSEMBLER_HPP

#include <vector>
#include <string>
#include <map>
#include "ReadingStatus.hpp"

#include <iostream>

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
	std::vector<std::string> contents;
	std::string buffer;
	ReadingStatus status;
	std::string method;
	Body body;

	AssembleInfo() {
		contents.clear();
		buffer.clear();
		status = StartLine;
	}
};

class RequestAssembler {
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

	public:
		RequestAssembler();
		RequestAssembler(const RequestAssembler& other);
		RequestAssembler& operator=(const RequestAssembler& other);
		~RequestAssembler();
		
		ReadingStatus getReadingStatus(const int ident);
		std::vector<std::string> getContent(const int ident);
		void run(const int ident, const unsigned char* newContent);
		void checkReadingStatus(AssembleInfo& info);
		void clear(const int ident);

};


#endif