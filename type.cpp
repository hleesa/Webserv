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

bool	isNumber(const std::string& string)
{
	int	idx = 0;
	int	size = string.size();

	while (idx < size && isdigit(string[idx]))
		idx++;
	return idx == size;
}