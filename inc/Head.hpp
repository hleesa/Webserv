#ifndef HEAD_HPP
#define HEAD_HPP

#include "Get.hpp"
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"

class Head : public Get {
	private:
		virtual HttpResponseMessage processReturnDirective() const;
		virtual HttpResponseMessage makeRedirectionResponse(const std::pair<int, std::string> return_value) const;
		std::map<std::string, std::string> makeHeaderFields(const std::string path) const;

	public:
		Head(const HttpRequestMessage* request, const Config* config);

		virtual HttpResponseMessage makeHttpResponseMessage();
};

#endif