
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


void print(std::istringstream& ss) {
    std::string line;
    while (std::getline(ss, line)) {
        std::cout << line << '\n';
    }
}

std::string trim(const std::string& input) {
    size_t start = input.find_first_not_of(" \t\n\v\f\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = input.find_last_not_of(" \t\n\v\f\r");
    return input.substr(start, end - start + 1);
}

bool isValidLineTerminator(const std::string& line) {
    const std::string line_term = ";{}";
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

std::string semicolonTrim(const std::string& line) {
    std::string ret = line;
    if (!ret.empty() && ret.back() == ';') {
        ret.pop_back();
        std::string::reverse_iterator it = ret.rbegin();
        while (it != line.rend() && isspace(*it)) {
            ret.pop_back();
            ++it;
        }
        ret.append(";");
    }
    return ret;
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

bool isValidConfigLine(std::string& line, std::stack<char>& brace_stack) {
    if (!isValidLineTerminator(line)) {
        return false;
    }
    line = semicolonTrim(line);
    switch (line.back()) {
        case '{':
            if (brace_stack.empty()) {
                if (!isValidServerOpen(line)) {
                    return false;
                }
            }
            else if (!isValidLocationOpen(line)) {
                return false;
            }
            brace_stack.push(line.back());
            break;
        case '}':
            if (brace_stack.empty() || brace_stack.top() != '{') {
                return false;
            }
            else if (line.length() != 1) {
                return false;
            }
            brace_stack.pop();
            break;
    }
    return true;
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
    std::vector<std::string> server_contents;
    std::string line;
    std::stack<char> brace_stack;
    while (std::getline(config, line)) {
        line = trim(line); // server out
        if (line.front() == '#' || line.empty()) {
            continue;
        }
        else if (isValidConfigLine(line, brace_stack)) {
            if (brace_stack.size() == 1) { // server in
                std::string server_content;
                server_content.reserve(500);
                while (std::getline(config, line)) {
                    line = trim(line);
                    if (line.front() == '#') {
                        continue;
                    }
                    else if (isValidConfigLine(line, brace_stack)) {
                        if (brace_stack.empty()) break;
                        server_content.append(line);
                        server_content.append("\n");
                    }
                    else {
                        throw std::invalid_argument("Error: Syntax");
                    }
                }
                server_contents.push_back(server_content);
            }
            else {
                throw std::invalid_argument("Error: Syntax");
            }
        }
        else {
            throw std::invalid_argument("Error: Syntax");
        }
    }
    /**
     * test
     */

    for (std::vector<std::string>::iterator it = server_contents.begin(); it != server_contents.end(); ++it) {
        std::istringstream ifs(*it);
        print(ifs);
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







