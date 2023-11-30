
#ifndef WEBSERV_EVENTTYPE_HPP
#define WEBSERV_EVENTTYPE_HPP

typedef enum EventType {
    ERROR = -1,
    NONE,
    LISTEN,
    PARSE_REQUEST,
    SEND_RESPONSE,
    READ_PIPE,
    WRITE_PIPE,
    TIMEOUT,
    TIMEOUT_CGI,
    CGI_END,
} EventType;

#endif //WEBSERV_EVENTTYPE_HPP
