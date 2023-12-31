#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include <string>
#include <map>
#include "HttpRequestMessage.hpp"
#include "ParsingData.hpp"

class RequestParser {
	private:
		std::map<int, ParsingData> parsing_data;

		void checkReadingStatus(ParsingData& data);
		bool processStartLine(ParsingData& data);
		std::string getMethod(const std::string& line);
		bool processHeader(ParsingData& data);
		BodyStatus getBodyStatus(const std::string line, Body& body);
		bool hasTransferEncoding(const std::string line);
		int getContentLength(const std::string line);
		bool isCRLF(const std::string line);
		ReadingStatus processBody(ParsingData& data);
		ReadingStatus processEncoding(Body& body, std::string& buffer);
		int getChunkedSize(std::string& buffer, int& pos);
		ReadingStatus processContentLength(Body& body, std::string& buffer);
		
		void trimCarriageReturn(std::string& line);
    
		void parseRequestLine(ParsingData& data, const std::string& line);
		void parseHeaderFields(ParsingData& data, const std::string line);

	public:
		RequestParser();
		RequestParser(const RequestParser& other);
		RequestParser& operator=(const RequestParser& other);
		~RequestParser();
		
		ReadingStatus getReadingStatus(const int ident);
		HttpRequestMessage getHttpRequestMessage(const int ident);
		void run(const int ident, const char* newContent);
		void clear(const int ident);
};

void convertLowerCase(std::string& string);
bool isHexadecimal(const std::string& literal);
bool isVisibleString(const std::string& str);

#endif