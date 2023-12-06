
#include "../../inc/HttpResponseMessage.hpp"
#include "../../inc/CodeToReason.hpp"
#include <sstream>

HttpResponseMessage::HttpResponseMessage() : status_code(0), header_fields(), message_body(), response(),
                                             response_size(0), response_ptr(NULL), bytes_to_send(0), bytes_sent(0) {
}

HttpResponseMessage::HttpResponseMessage(int status_code, std::map<std::string, std::string> header_fields,
                                         std::string message_body) :
        status_code(status_code), header_fields(header_fields), message_body(message_body), response_size(0),
        response_ptr(NULL), bytes_to_send(0), bytes_sent(0) {

    response = toString();
    if (!response.empty())
        response_size = response.size();
    this->response_ptr = new unsigned char[this->response_size];
    std::memmove(this->response_ptr, response.c_str(), response_size);
    bytes_to_send = response_size;
}

HttpResponseMessage& HttpResponseMessage::operator=(const HttpResponseMessage& other) {
    if (&other != this) {
        status_code = other.status_code;
        header_fields = other.header_fields;
        message_body = other.message_body;
        response = other.response;
        response_size = other.response_size;
        bytes_to_send = other.bytes_to_send;
        bytes_sent = other.bytes_sent;
        if (response_ptr != NULL) {
            delete[] response_ptr;
            response_ptr = NULL;
        }
        if (other.response_ptr != NULL) {
            response_ptr = new unsigned char[other.response_size];
            std::memmove(response_ptr, other.response_ptr, other.response_size);
        }
    }
    return *this;
}

#include <iostream>
HttpResponseMessage::~HttpResponseMessage() {
    std::cout << "~HttpResponseMessage()\n";
    if (response_ptr != NULL) {
        delete[] response_ptr;
        response_ptr = NULL;
    }
}

std::string& HttpResponseMessage::getResponseRef() {
    return response;
}

std::string HttpResponseMessage::getResponse() {
    return response;
}

unsigned char* HttpResponseMessage::getResponsePtr() const {
    return response_ptr;
}

void HttpResponseMessage::updateBytesSent(ssize_t new_bytes_sent) {
    bytes_to_send -= static_cast<size_t>(new_bytes_sent);
    bytes_sent += static_cast<size_t>(new_bytes_sent);
}

void* HttpResponseMessage::getSendBuffer() {
    return response_ptr + bytes_sent;
}

bool HttpResponseMessage::sendComplete() const {
    return bytes_to_send == 0;
}

size_t HttpResponseMessage::getBytesSent() const {
    return bytes_sent;
}

size_t HttpResponseMessage::getBytesToSend() const {
    return bytes_to_send;
}

void HttpResponseMessage::clear() {
    response.clear();
}

std::string HttpResponseMessage::toString() {
	static CodeToReason code_to_reason;
    std::stringstream ss;
    
	ss << "HTTP/1.1 " << status_code << " " << code_to_reason.getReasonPharse(status_code) << "\r\n";
    for (std::map<std::string, std::string>::const_iterator header = header_fields.begin(); header != header_fields.end(); ++header) {
        ss << header->first << ": " << header->second << "\r\n";
	}   
	ss << "\r\n";
    ss << message_body;
    return ss.str();
}
