#ifndef GET_HPP
#define GET_HPP

#include "Method.hpp"
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class Get : public Method {
	protected:
		std::string findResourcePath() const;
		bool isDirectoryList(const std::string path) const;
		Resource makeResource() const;

		virtual HttpResponseMessage processReturnDirective() const;
		virtual HttpResponseMessage makeRedirectionResponse(const std::pair<int, std::string> return_value) const;

		std::map<std::string, std::string> makeHeaderFileds(const std::string& body, const std::string path) const;

	public:
		Get(const HttpRequestMessage* request, const Config* config);

		virtual HttpResponseMessage makeHttpResponseMessage();
};

bool checkFileExistence(const std::string file_name);

#endif