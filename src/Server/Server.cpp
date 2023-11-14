#include "../../inc/Server.hpp"
#include "../../inc/CgiGet.hpp"
#include "../../inc/Get.hpp"
#include "../../inc/Post.hpp"
#include "../../inc/Delete.hpp"
#include "../../inc/ErrorPage.hpp"

Server::Server() {}

Server::Server(int connection, int listen) {
	connection_socket = connection;
	listen_socket = listen;
}

void Server::setRequest(const HttpRequestMessage& request_message) {
	this->request = request_message;
}

std::string Server::makeResponse(std::map<int, Config>& configs) {
	try {
		if (request.getMethod() == "POST") {
			Post method;
			return method.run(request, configs[listen_socket]).toString();
		}
		if (request.getMethod() == "GET" && request.getURL().find("cgi") == std::string::npos) {
			Get method(request, configs[listen_socket]);
			return method.makeHttpResponseMessage().toString();
		}
		if (request.getMethod() == "DELETE") {
			Delete method(request, configs[listen_socket]);
			return method.makeHttpResponseMessage().toString();
		}
	}
	catch (const int status_code) {
		return ErrorPage::makeErrorPageResponse(status_code, configs[listen_socket]).toString();
	}
    try {
        if (CgiGet::isValidCgiGetUrl(request.getRequestLine(), configs, listen_socket)) {
            std::map<int, Config>::const_iterator found_config = configs.find(listen_socket);
            if (found_config == configs.end()) {
                // config not found
                return "";
            }
            HttpResponseMessage response = CgiGet::processCgiGet(request.getURL(),
                                                                 found_config->second.getCgiLocation().second,
                                                                 connection_socket);
            return response.toString();
        }
    } 
	catch (int status_code) {
        return "";
    }
	return HttpResponseMessage().toString();
}
