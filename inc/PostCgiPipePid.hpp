
#ifndef WEBSERV_POSTCGIPIPEPID_HPP
#define WEBSERV_POSTCGIPIPEPID_HPP

#include <unistd.h>

class PostCgiPipePid {

private:
    int* read_pipe_fd;
    int* write_pipe_fd;
    pid_t* child_pid;

public:
    PostCgiPipePid();
    PostCgiPipePid(int* read_pipe_fd, int* write_pipe_fd, pid_t* child_pid);

    int getReadPipeFd() const;
    int getWritePipeFd() const;
    int getChildPid() const;
};

#endif //WEBSERV_POSTCGIPIPEPID_HPP
