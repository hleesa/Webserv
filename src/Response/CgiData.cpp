#include "../../inc/Constants.hpp"
#include "../../inc/CgiData.hpp"

CgiData::CgiData() : pipe_child_parent(NULL), pipe_parent_child(NULL), child_pid(0), conn_socket(0) {
}

CgiData::CgiData(int* pipe_child_parent, int* pipe_parent_child, pid_t child_pid) :
        pipe_child_parent(pipe_child_parent), pipe_parent_child(pipe_parent_child), child_pid(child_pid),
        conn_socket(0) {
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

CgiData::~CgiData() {
    if (pipe_child_parent != NULL) {
        close(pipe_child_parent[READ]);
        delete[] pipe_child_parent;
        pipe_child_parent = NULL;
    }
    if (pipe_parent_child != NULL) {
        close(pipe_parent_child[WRITE]);
        delete[] pipe_parent_child;
        pipe_parent_child = NULL;
    }
}