
#include "../../inc/PostCgiPipePid.hpp"

PostCgiPipePid::PostCgiPipePid() : read_pipe_fd(0), write_pipe_fd(0), child_pid(0) {
}

PostCgiPipePid::PostCgiPipePid(int* read_pipe_fd, int* write_pipe_fd, pid_t* child_pid) : read_pipe_fd(read_pipe_fd),
                                                                               write_pipe_fd(write_pipe_fd),
                                                                               child_pid(child_pid){
}

int PostCgiPipePid::getReadPipeFd() const {
    return *read_pipe_fd;
}

int PostCgiPipePid::getWritePipeFd() const {
    return *write_pipe_fd;
}

int PostCgiPipePid::getChildPid() const {
    return *child_pid;
}