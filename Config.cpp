
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

bool isValidLineTerminator(std::vector<std::string>& server_content) {
    const std::string line_term = "{;}";
    if (server_content.empty()) {
        return true;
    }
    else if (line_term.find(server_content.back().back()) == std::string::npos) {
        return false;
    }
    const int char_size = 256;
    int counter[char_size] = {0};
    for (std::vector<std::string>::const_iterator content = server_content.begin();
         content != server_content.end(); ++content) {
        for (std::string::const_iterator ch = content->begin(); ch != content->end(); ++ch) {
            ++counter[*ch];
        }
    }
    int num_of_line_term = 0;
    for (std::string::const_iterator it = line_term.begin(); it != line_term.end(); ++it) {
        num_of_line_term += counter[*it];
    }
    return num_of_line_term == 1;
}

std::vector<std::string> lineToWords(const std::string& line) {
    std::string word;
    std::vector<std::string> words;
    std::stringstream ss(line);
    while (ss >> word) {
        words.push_back(word);
    }
    return words;
}

bool isTargetBlockOpen(const std::vector<std::string> words, const std::string target, const int num_of_words) {
    if (words.size() != num_of_words) {
        return false;
    }
    return (words.front() == target) && (words.back() == "{");
}

bool isServerOpen(const std::vector<std::string>& words) {
    return isTargetBlockOpen(words, "server", 2);
}

bool isLocationOpen(const std::vector<std::string>& words) {
    return isTargetBlockOpen(words, "location", 3);
}

enum lineType getLineType(std::vector<std::string>& server_content, std::stack<char>& brace_stack) {
    if (!isValidLineTerminator(server_content)) {
        return INVALID;
    }
    else if (server_content.empty()) {
        return SPACE;
    }
    else if (server_content.front().front() == '#') {
        return COMMENT;
    }
    else if (server_content.back().back() == '{') {
        if (brace_stack.empty() && isServerOpen(server_content)) {
            return SERVER_OPEN;
        }
        else if (!brace_stack.empty() && isLocationOpen(server_content)) {
            return LOCATION_OPEN;
        }
    }
    else if (server_content.back().back() == ';') {
        return SEMICOLON;
    }
    else if (server_content.back().back() == '}') {
        if (brace_stack.empty() || brace_stack.top() != '{' || server_content.size() != 1) {
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

std::vector<std::vector<std::string> > getSeverContents(std::ifstream& config) {
    std::string line;
    std::vector<std::vector<std::string> > server_contents;
    std::stack<char> brace_stack;
    while (std::getline(config, line)) {
        std::vector<std::string> server_content = lineToWords(line);
        enum lineType line_type = getLineType(server_content, brace_stack);
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
                break;
            case LOCATION_OPEN:
                brace_stack.push(line.back());
                server_contents.push_back(server_content);
                break;
            case SEMICOLON:
                line.pop_back();
                server_content.back().pop_back();
                server_contents.push_back(server_content);
                break;
            case LOCATION_CLOSE:
                brace_stack.pop();
                server_contents.push_back(server_content);
                break;
            case SERVER_CLOSE:
                brace_stack.pop();
                break;
        }
    }
    return server_contents;
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
    std::vector<std::vector<std::string> > server_contents = getSeverContents(config);

    /**
     * test
     */
    for (std::vector<std::vector<std::string> >::iterator it = server_contents.begin();
         it != server_contents.end(); ++it) {
        for (auto c: *it) {
            std::cout << c << " ";
        }
        std::cout << '\n';
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







