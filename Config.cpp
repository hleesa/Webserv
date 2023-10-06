
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <stack>

/**
* 1. 파일 열기
* 2. 서버 블록을 읽어서 server 객체 생성
* 3. server {}의 내용물만
**/

std::string semicolonTrim(const std::string& line) {
    std::string ret = line;
    const std::string space = " \t\n\v\f\r";
    if (!ret.empty() && ret.back() == ';') {
        ret.pop_back();
        size_t end = ret.find_last_not_of(space);
        ret = ret.substr(0, end + 1);
        ret.append(";");
    }
    return ret;
}

std::string trim(const std::string& input) {
    const std::string space = " \t\n\v\f\r";
    size_t start = input.find_first_not_of(space);
    if (start == std::string::npos) {
        return "";
    }
    size_t end = input.find_last_not_of(space);
    return semicolonTrim(input.substr(start, end - start + 1));
}

bool isValidLineTerminator(const std::string& line) {
    const std::string line_term = "{;}";
    if (line.empty()) {
        return true;
    }
    else if (line_term.find(line.back()) == std::string::npos) {
        return false;
    }
    const int char_size = 256;
    int counter[char_size] = {0};
    for (std::string::const_iterator it = line.begin(); it != line.end(); ++it) {
        ++counter[*it];
    }
    int num_of_line_term = 0;
    for (std::string::const_iterator it = line_term.begin(); it != line_term.end(); ++it) {
        num_of_line_term += counter[*it];
    }
    return num_of_line_term == 1;
}

bool isValidServerOpen(const std::string& line) {
    const std::string target = "server";
    if (line.substr(0, target.length()) != target) {
        return false;
    }
    size_t i = target.length();
    while (isspace(line[i]))
        ++i;
    return (i + 1) == line.length();
}

bool isValidLocationOpen(const std::string& line) {
    const std::string target = "location";
    if (line.substr(0, target.length()) != target) {
        return false;
    }
    size_t i = target.length();
    while (isspace(line[i]))
        ++i;
    while (!isspace(line[i]))
        ++i;
    while (isspace(line[i]))
        ++i;
    return (i + 1) == line.length();
}

enum lineType getLineType(std::string& line, std::stack<char>& brace_stack) {
    if (!isValidLineTerminator(line)) {
        return INVALID;
    }
    else if (line.empty()) {
        return SPACE;
    }
    else if (line.front() == '#') {
        return COMMENT;
    }
    else if (line.back() == '{') {
        if (brace_stack.empty() && isValidServerOpen(line)) {
            return SERVER_OPEN;
        }
        else if (!brace_stack.empty() && isValidLocationOpen(line) ) {
            return LOCATION_OPEN;
        }
    }
    else if (line.back() == ';') {
        return SEMICOLON;
    }
    else if (line.back() == '}') {
        if (brace_stack.empty() || brace_stack.top() != '{' || line.length() != 1) {
            return INVALID;
        }
        else if (brace_stack.size() == 2) {
            return LOCATION_CLOSE;
        }
        else if (brace_stack.size() == 1) {
            return SERVER_CLOSE;
        }
    }
    return INVALID;
}

std::vector<std::string> getSeverContents(std::ifstream& config) {
    const int content_size = 500;
    std::string line;
    std::string server_content;
    std::vector<std::string> server_contents;
    std::stack<char> brace_stack;
    while (std::getline(config, line)) {
        line = trim(line);
        enum lineType line_type = getLineType(line, brace_stack);
        switch (line_type) {
            case SPACE:
            case COMMENT:
                continue;
                break;
            case INVALID:
                throw std::invalid_argument("Error: Syntax");
                break;
            case SERVER_OPEN:
                brace_stack.push(line.back());
                server_content.clear();
                server_content.reserve(content_size);
                break;
            case LOCATION_OPEN:
                brace_stack.push(line.back());
                server_content.append(line + "\n");
                break;
            case SEMICOLON:
                line.pop_back();
                server_content.append(line + "\n");
                break;
            case LOCATION_CLOSE:
                brace_stack.pop();
                server_content.append(line + "\n");
                break;
            case SERVER_CLOSE:
                brace_stack.pop();
                server_contents.push_back(server_content);
                break;
        }
    }
    return server_contents;
}


void print(std::istringstream& ss) {
    std::string line;
    while (std::getline(ss, line)) {
        std::cout << line << '\n';
    }
}

Config::Config(const std::string& config_file) {

    const std::string config_format = ".conf";
    if (config_file.length() < config_format.length()) {
        throw std::invalid_argument("Error: Invalid file name '" + config_file + "'");
    }
    else if (config_file.substr(config_file.length() - config_format.length()) != config_format) {
        throw std::invalid_argument("Error: Invalid file format '" + config_file + "'");
    }
    std::ifstream config(config_file.c_str());
    if (!config.is_open()) {
        throw std::invalid_argument("Error: Failed to open file '" + config_file + "'");
    }
    std::vector<std::string> server_contents = getSeverContents(config);

    for (std::vector<std::string>::iterator it = server_contents.begin(); it != server_contents.end(); ++it) {
        std::istringstream ifs(*it);
		Server server(ifs);
		servers.push_back(server);
		// print(ifs);
    }

    config.close();
}


Config::Config() {
}

Config::Config(const Config& other) {

}

Config& Config::operator=(const Config& ohter) {
    return *this;
}

Config::~Config() {
}







