
#include "RequestParser.hpp"
#include "sstream"
#include "Config.hpp"
#include "algorithm"

#define NUM_OF_ALLOW_METHOD 3
#define NUM_OF_VERSION 3

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
        throw std::invalid_argument("400");
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
        throw std::invalid_argument("501");
    }
    return;
}

void validateHttpVersion(const std::string& http_version) {
    std::string http_version_list[] = {"HTTP/1.1", "HTTP/1.0", "HTTP/0.9"};
    std::vector<std::string> versions(http_version_list, http_version_list + NUM_OF_VERSION);
    if (std::find(versions.begin(), versions.end(), http_version) == versions.end()) {
        throw std::invalid_argument("505");
    }
    return;
}

std::vector<std::string> RequestParser::parseRequestLine(const std::string& request_line) {
    std::vector<std::string> request = tokenizeRequestLine(request_line);
    validateMethod(request.front());
    // validRequestTarget
    validateHttpVersion(request.back());
    return request;
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

std::map<std::string, std::vector<std::string> >
RequestParser::parseHeaderFields(const std::vector<std::string>& request) {
    std::map<std::string, std::vector<std::string> > header_fields;
    for (size_t i = 1; i + 1 < request.size(); ++i) {
        std::vector<std::string> header_field = tokenizeHeaderFiled(request[i]);
        if (header_field.empty()) continue;
        header_fields[header_field.front()].insert(header_fields[header_field.front()].end(), header_field.begin() + 1,
                                                   header_field.end());
    }
    return header_fields;
}

HttpRequestMessage RequestParser::parseRequestMessage(std::vector<std::string> request) {
    // request 또는 request 의 각 라인  or config 가 비어있는 경우 예외처리?
    int status_code = 0;
    std::vector<std::string> request_line;
    std::map<std::string, std::vector<std::string> > header_fields;
    std::string message_body;
    try{
        request_line = parseRequestLine(request.front());
        header_fields = parseHeaderFields(request);
        message_body = request.back();
    } catch (std::exception& e) {
        status_code = std::stoi(e.what());
    }
    return HttpRequestMessage(request_line, header_fields, message_body, status_code);
}


