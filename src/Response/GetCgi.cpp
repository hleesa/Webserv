#include <map>
#include <unistd.h>
#include "../../inc/GetCgi.hpp"
#include "../../inc/Constants.hpp"
#include "../../inc/ServerUtils.hpp"

GetCgi::GetCgi(const HttpRequestMessage* request, const Config* config) : request(request), config(config) {
    location_key = ::findLocationKey(config, request);
}

// /cgi-bin/cgi_get.py?input=Hello
std::vector<std::string> tokenizeUrl(const std::string& url) {
    std::string new_url = url.substr(1);
    for (std::string::iterator ch = new_url.begin(); ch != new_url.end(); ++ch) {
        if (*ch == '/') {
            *ch = ' ';
        }
    }
    std::string token;
    std::vector<std::string> tokens;
    std::stringstream ss_url(new_url);
    while (ss_url >> token) {
        tokens.push_back(token);
    }
    std::string path;
    if (!tokens.empty()) {
        path = tokens.back();
        tokens.pop_back();
    }
    for (std::string::iterator ch = path.begin(); ch != path.end(); ++ch) {
        if (*ch == '?') {
            *ch = ' ';
        }
    }
    std::stringstream ss_path(path);
    while (ss_path >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void validateComponent(const std::vector<std::string>& url_vec) {
    if (url_vec.size() != 3) {
        throw 400;
    }
    for (std::vector<std::string>::const_iterator component = url_vec.begin();
         component != url_vec.end(); ++component) {
        if (!isVisibleString(*component)) {
            throw 400;
        }
    }
    const std::string reserved_characters = ":/?#[]@!$&'()*+,;=";
    int cnt = 0;
    std::map<char, int> checker;
    for (std::string::const_iterator ch = url_vec.back().begin(); ch != url_vec.back().end(); ++ch) {
        if (reserved_characters.find(*ch) != std::string::npos) {
            ++cnt;
            ++checker[*ch];
        }
    }
    if (cnt == 1 && checker['='] == 1)
        return;
    throw 400;
}

std::vector<std::string> parseUrl(const std::string url) {
    std::vector<std::string> url_vec = tokenizeUrl(url);
    validateComponent(url_vec);
    return url_vec;
}

std::string getScriptPath(const std::string url, const std::string root) {
    size_t idx = url.find("?");
    if (idx == std::string::npos) {
        throw 400;
    }
    std::string base_url = root.empty() ? "./" : root;
    return base_url + url.substr(1, idx - 1);
}

bool GetCgi::isValidCgiGetUrl() const {
    const std::vector<std::string>& request_line = request->getRequestLine();

    const std::vector<std::string> url_vec = parseUrl(request_line[1]);
//    if (request_line.front() != "GET" || url_vec.front().find("cgi") == std::string::npos) {
//        return false;
//    }
//    std::string path = "/" + *url_vec.begin();
//    if (config->getLocations().find(path) == config->getLocations().end()) {
//        return false;
//    }
    Location cgi_location = config->getLocations()[location_key];
    const std::string cgi_script_url = getScriptPath(request_line[1], cgi_location.getRoot());
    if (cgi_script_url.length() < cgi_location.getCgiExt().length()) {
        return false;
    }
    else if (cgi_script_url.substr(cgi_script_url.length() - cgi_location.getCgiExt().length()) !=
             cgi_location.getCgiExt()) {
        return false;
    }
    else if (access(cgi_script_url.c_str(), F_OK | X_OK) == ERROR) {
        return false;
    }
    return true;
}

std::map<std::string, std::string> setEnviron(const std::string query_string) {
    std::map<std::string, std::string> environ;
    environ["QUERY_STRING"] = query_string;
    return environ;
};

char** createCgiEnviron(const std::string query_string) {
    const std::map<std::string, std::string> environ = setEnviron(query_string);
    char** cgi_environ = new char* [environ.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::const_iterator env = environ.begin(); env != environ.end(); ++env) {
        std::string temp_env = env->first + "=" + env->second;
        cgi_environ[i] = new char[temp_env.length() + 1];
        strcpy(cgi_environ[i], temp_env.c_str());
        ++i;
    }
    cgi_environ[i] = NULL;
    return cgi_environ;
}

std::string getQueryString(const std::string url) {
    size_t idx = url.find("?");
    if (idx == std::string::npos) {
        throw 400;
    }
    return url.substr(idx + 1);
}

void execveCgiScript(int* pipe_fd, const std::string requestUrl, Location cgi_location) {
    char** cgi_environ = createCgiEnviron(getQueryString(requestUrl));
    char* python_interpreter = strdup(cgi_location.getCgiPath().c_str());
    char* python_script = strdup(getScriptPath(requestUrl, cgi_location.getRoot()).c_str());
    char* const command[] = {python_interpreter, python_script, NULL};
    if (close(pipe_fd[READ]) == ERROR) {
        exit(EXIT_FAILURE);
    }
    if (dup2(pipe_fd[WRITE], STDOUT_FILENO) == ERROR) {
        exit(EXIT_FAILURE);
    }
    if (close(pipe_fd[WRITE]) == ERROR) {
        exit(EXIT_FAILURE);
    }
    if (execve(python_interpreter, command, cgi_environ) == ERROR) {
        exit(EXIT_FAILURE);
    }
    exit(EXIT_FAILURE);
}

std::string readCgiResponse(int* pipe_fd, pid_t pid) {
    std::string body;

    (void) pid;
//    char rcv_buffer[BUFSIZ];
//    ssize_t bytes_read;
//    while ((bytes_read = read(pipe_fd[READ], rcv_buffer, sizeof(rcv_buffer))) > 0) {
//        body.append(rcv_buffer, bytes_read);
//    }
//    if (bytes_read == ERROR) {
//        throw 500;
//    }
    if (close(pipe_fd[READ]) == ERROR) {
        throw 500;
    }
    return body;
}

// /cgi-bin/cgi_get.py?input=Hello
CgiData* GetCgi::makeHttpResponseMessage() {
    if (!isValidCgiGetUrl()) {
        throw 400;
    }
    int pipe_child_parent[2];
    if (pipe(pipe_child_parent) == ERROR) {
        throw 500;
    }
    pid_t pid = fork();
    if (pid == ERROR) {
        throw 500;
    }
    else if (pid > 0) {
        if (close(pipe_child_parent[WRITE]) == ERROR) {
            throw 500;
        }
    }
    else {
        execveCgiScript(pipe_child_parent, request->getURL(), config->getLocations()[location_key]);
    }
    return new CgiData(NULL, pipe_child_parent, pid);
}

void GetCgi::checkAllowed(const std::string method) const {
    if (method != "GET")
        throw 405;
}
