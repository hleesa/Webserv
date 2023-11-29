
#include "../../inc/PostCgiPipePid.hpp"

PostCgiPipePid::PostCgiPipePid() : pipe_child_parent(NULL), pipe_parent_child(NULL), child_pid(NULL) {
}

PostCgiPipePid::PostCgiPipePid(int* pipe_child_parent, int* pipe_parent_child, pid_t* child_pid) :
        pipe_child_parent(pipe_child_parent), pipe_parent_child(pipe_parent_child), child_pid(child_pid) {
}

int PostCgiPipePid::getReadPipeFd() const {
    return pipe_child_parent[READ];
}

int PostCgiPipePid::getWritePipeFd() const {
    return pipe_parent_child[WRITE];
}

int PostCgiPipePid::getChildPid() const {
    return *child_pid;
}

PostCgiPipePid::~PostCgiPipePid() {
    delete[] pipe_child_parent;
    delete[] pipe_parent_child;
    delete child_pid;
}