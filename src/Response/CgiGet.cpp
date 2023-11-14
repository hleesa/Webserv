
#include "CgiGet.hpp"
#include <map>
#include <unistd.h>

#define ERROR -1
#define READ 0
#define WRITE 1
#define NUM_OF_CGI_ENV 18

//중복 RequestParser.cpp

// http://example.com
// /cgi-bin/cgi_script.py?input=Hello
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
    /**
     * https://datatracker.ietf.org/doc/html/rfc3986#page-12

      gen-delims  = ":" / "/" / "?" / "#" / "[" / "]" / "@"

      sub-delims  = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
     */
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

std::string getScriptPath(const std::string url) {
    size_t idx = url.find("?");
    if (idx == std::string::npos) {
        return std::string();
    }
    return "." + url.substr(0, idx);
}

bool CgiGet::isValidCgiGetUrl(const std::vector<std::string>& request_line, const std::map<int, Config>& configs,
                              int con_socket) {
    const std::vector<std::string> url_vec = parseUrl(request_line[1]);
    if (request_line.front() != "GET" || url_vec.front().find("cgi") == std::string::npos) {
        return false;
    }
    std::map<int, Config>::const_iterator found_config = configs.find(con_socket);
    if (found_config == configs.end()) {
        return false;
    }
    const Config& config = found_config->second;
    std::pair<std::string, CgiLocation> cgi_location = config.getCgiLocation();
    if (url_vec.front() != cgi_location.first.substr(1)) {
        return false;
    }
//    const std::string cgi_script_url = "./" + url_vec.front() + "/" + url_vec[1];
    const std::string cgi_script_url = getScriptPath(request_line[1]);
    if (access(cgi_script_url.c_str(), F_OK | X_OK) == ERROR) {
        return false;
    }
    return true;
}

/*
      AUTH_TYPE
      CONTENT_LENGTH
      CONTENT_TYPE
      GATEWAY_INTERFACE
      HTTP_*
      PATH_INFO
      PATH_TRANSLATED
      QUERY_STRING
      REMOTE_ADDR
      REMOTE_HOST
      REMOTE_IDENT
      REMOTE_USER
      REQUEST_METHOD
      SCRIPT_NAME
      SERVER_NAME
      SERVER_PORT
      SERVER_PROTOCOL
      SERVER_SOFTWARE
 */

std::map<std::string, std::string> setEnviron(const std::string query_string) {
    std::map<std::string, std::string> environ;
    environ["QUERY_STRING"] = query_string;
    environ["REQUEST_METHOD"] = "GET";
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
        return std::string();
    }
    return url.substr(idx+1);
}

void deleteExecveArg(char** cgi_environ, char* python_interpreter, char* python_script) {
    int i = 0;
    while(cgi_environ[i]) {
        delete cgi_environ[i];
        ++i;
    }
    delete[] cgi_environ;
    free(python_interpreter);
    free(python_script);
    return;
}

// /cgi-bin/cgi_script.py?input=Hello
HttpResponseMessage CgiGet::processCgiGet(const std::string url, CgiLocation cgi_location) {
    char** cgi_environ = createCgiEnviron(getQueryString(url));
    char* python_interpreter = strdup(cgi_location.getCgiPath().c_str());
    char* python_script = strdup(getScriptPath(url).c_str());
    char* const command[] = {
            python_interpreter,
            python_script,
            NULL
    };
    std::string body;

    int pipe_fd[2];
    if (pipe(pipe_fd) == ERROR) {
        throw 500;
    }
    pid_t pid = fork();
    if (pid == ERROR) {
        throw 500;
    }
    else if (pid > 0) {
        if (close(pipe_fd[WRITE]) == ERROR) {
            throw 500;
        }
        if (dup2(pipe_fd[READ], STDIN_FILENO) == ERROR) {
            throw 500;
        }
        if (close(pipe_fd[READ]) == ERROR) {
            throw 500;
        }
        char rcv_buffer[BUFSIZ];
        memset(rcv_buffer, 0, sizeof(rcv_buffer));
        int n;
        while ((n = read(STDIN_FILENO, rcv_buffer, sizeof(rcv_buffer)))) {
//            if (n != 0) {
//                rcv_buffer[n] = '\0';
                body += std::string(rcv_buffer);
//            }
//            write(conn_sock, rcv_buffer, strlen(rcv_buffer));
            memset(rcv_buffer, 0, sizeof(rcv_buffer));
        }
        waitpid(pid, 0, 0);
    }
    else {
        close(pipe_fd[READ]);
        if (dup2(pipe_fd[WRITE], STDOUT_FILENO) == ERROR) {
            exit(EXIT_FAILURE);
        }
        if ((close(pipe_fd[WRITE]) == ERROR)) {
            exit(EXIT_FAILURE);
        }
        if (access(python_script, F_OK | X_OK) == ERROR) {
            exit(EXIT_FAILURE);
        }
        if (execve(python_interpreter, command, cgi_environ) == ERROR) {
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    }
    // free()
    int status_code = 200;
    std::map<std::string, std::string> header_fields;
    header_fields["Content-type"] = "text/html";
    header_fields["Content-Length"] = std::to_string(body.size()-1);
    return HttpResponseMessage(status_code, header_fields, body);
}
