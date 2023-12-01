
#ifndef WEBSERV_CGIDATA_HPP
#define WEBSERV_CGIDATA_HPP

#include <unistd.h>
#define READ 0
#define WRITE 1

class CgiData {

private:
    int* pipe_child_parent;
    int* pipe_parent_child;
    pid_t child_pid;
    int conn_socket;

public:
    CgiData();
	CgiData(int* pipe_parent_child, int* pipe_child_parent, pid_t child_pid, int conn_socket);

    ~CgiData();

    void setConnSocket(int socket);

    int getReadPipeFd() const;
    int getWritePipeFd() const;
    int getChildPid() const;
    int getConnSocket() const;
};

#endif //WEBSERV_CGIDATA_HPP
