
#ifndef WEBSERV_POSTCGIPIPEPID_HPP
#define WEBSERV_POSTCGIPIPEPID_HPP

#include <unistd.h>
#define READ 0
#define WRITE 1

class PostCgiPipePid {

private:
    int* pipe_child_parent;
    int* pipe_parent_child;
    pid_t* child_pid;

public:
    PostCgiPipePid();
    PostCgiPipePid(int* pipe_parent_child, int* pipe_child_parent, pid_t* child_pid);

    ~PostCgiPipePid();

    int getReadPipeFd() const;
    int getWritePipeFd() const;
    int getChildPid() const;
};

#endif //WEBSERV_POSTCGIPIPEPID_HPP
