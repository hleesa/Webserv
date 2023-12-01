
#include "../../inc/CgiData.hpp"

CgiData::CgiData() : pipe_child_parent(NULL), pipe_parent_child(NULL), child_pid(-1), conn_socket(-1) {
}

CgiData::CgiData(int* pipe_child_parent, int* pipe_parent_child, pid_t child_pid, int conn_socket) :
        pipe_child_parent(pipe_child_parent), pipe_parent_child(pipe_parent_child), child_pid(child_pid), conn_socket(conn_socket) {
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
    delete[] pipe_child_parent;
    delete[] pipe_parent_child;
}