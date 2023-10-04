#include <string>

bool isHttpMethod(const std::string& method)
{
	std::string allow_methods[9] = {"GET", "POST", "PUT", \
								"PATCH", "DELETE", "HEAD", \
								"OPTIONS", "CONNECT", "TRACE"};
	int idx = 0;

	while (idx < 9 && method != allow_methods[idx])
		idx++;
	return idx != 9;
}