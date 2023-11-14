#include "../../inc/Server.hpp"
#include "../../inc/CgiGet.hpp"
#include "../../inc/Get.hpp"
#include "../../inc/Post.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
	this->request = request_message;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
	if (request.getMethod() == "POST") {
		Post method;
		return method.run(request, configs[listen_socket]).toString();
	}
	if (request.getMethod() == "GET" && request.getURL().find("cgi") == std::string::npos) {
		Get method(request, configs[listen_socket]);
		return method.makeHttpResponseMessage().toString();
	}
    try {
        if (CgiGet::isValidCgiGetUrl(request.getRequestLine(), configs, listen_socket)) {
            std::map<int, Config>::const_iterator found_config = configs.find(listen_socket);
            if (found_config == configs.end()) {
                // config not found
                return "";
            }
            HttpResponseMessage response = CgiGet::processCgiGet(request.getURL(),
                                                                 found_config->second.getCgiLocation().second);
            return response.toString();
        }
    } 
	catch (int status_code) {
        return "";
    }
	return HttpResponseMessage().toString();
}
