#include "../../inc/Server.hpp"
#include "../../inc/Method.hpp"
#include "../../inc/GetCgi.hpp"
#include "../../inc/Get.hpp"
#include "../../inc/Post.hpp"
#include "../../inc/Delete.hpp"
#include "../../inc/ErrorPage.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
    connection_socket = connection;
    listen_socket = listen;
}

int Server::getListenSocket() const {
	return this->listen_socket;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
    this->request = request_message;
}

void Server::setResponse(std::string response_str) {
    char *response_message = new char[response_str.length() + 1];
    strcpy(response_message, response_str.c_str());
    this->response = response_message;
    this->response_length = response_str.length();
    this->bytes_send = 0;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
    try {
		if (request.getStatusCode()) {
			throw(request.getStatusCode());
		}
        Method *method = Method::generate(request.getMethod(), &request, &configs[listen_socket]);
		method->checkAllowed(request.getMethod());
        std::string response_message = method->makeHttpResponseMessage().toString();
        delete method;
        return response_message;
    }
    catch (const int status_code) {
        return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
    }
    return HttpResponseMessage().toString();
}

ssize_t Server::getBytesSend() {
    return bytes_send;
}

size_t Server::getResponseLength() {
    return response_length;
}

char *Server::getResponse() {
    return response;
}

void Server::setBytesSend(ssize_t bytes) {
    this->bytes_send = bytes;
}

//void Server::setResponseLength(size_t response_size) {
//    this->response_length = response_size;
//}