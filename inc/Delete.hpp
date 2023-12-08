#ifndef DELETE_HPP
#define DELETE_HPP

#include "Method.hpp"
#include "HttpRequestMessage.hpp"
#include "HttpResponseMessage.hpp"
#include "Config.hpp"
#include "Resource.hpp"

class Delete : public Method {
	private:
		
		void handleError(const std::string path) const;
		std::string findPathByRoot(const std::string url) const;
		
	public:
		Delete(const HttpRequestMessage* request, const Config* config);

		HttpResponseMessage makeHttpResponseMessage();
};

#endif