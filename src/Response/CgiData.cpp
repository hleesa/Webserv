#include "../../inc/Constants.hpp"
#include "../../inc/CgiData.hpp"

CgiData::CgiData() : pipe_child_parent(NULL), pipe_parent_child(NULL), child_pid(-1), conn_socket(-1), cgi_died(false) {
}

CgiData::CgiData(int* pipe_child_parent, int* pipe_parent_child, pid_t child_pid) :
        pipe_child_parent(pipe_child_parent), pipe_parent_child(pipe_parent_child), child_pid(child_pid),
        conn_socket(-1), cgi_died(false) {
}

int CgiData::getReadPipeFd() const {
    return pipe_child_parent[READ];
}

int CgiData::getWritePipeFd() const {
    return pipe_parent_child[WRITE];
}

int CgiData::getChildPid() const {
    return child_pid;
}

int CgiData::getConnSocket() const {
    return conn_socket;
}

void CgiData::setConnSocket(int socket) {
    this->conn_socket = socket;
}

void CgiData::setCgiDie(bool cgi_die) {
    this->cgi_died = cgi_die;
}

bool CgiData::cgiDied() const {
    return cgi_died;
}

void CgiData::closePipes() {
    if (pipe_child_parent != NULL) {
        close(pipe_child_parent[READ]);
    }
    if (pipe_parent_child != NULL) {
        close(pipe_parent_child[WRITE]);
    }
    pipe_child_parent = NULL;
    pipe_parent_child = NULL;
}

void CgiData::closeReadPipeFd() {
    if (pipe_child_parent != NULL) {
        close(pipe_child_parent[READ]);
    }
    pipe_child_parent = NULL;
}

void CgiData::closeWritePipeFd() {
    if (pipe_parent_child[WRITE]) {
        close(pipe_parent_child[WRITE]);
    }
    pipe_parent_child = NULL;
}

CgiData::~CgiData() {
    if (pipe_child_parent != NULL) {
        delete[] pipe_child_parent;
    }
    if (pipe_parent_child != NULL) {
        delete[] pipe_parent_child;
    }
    pipe_child_parent = NULL;
    pipe_parent_child = NULL;
}