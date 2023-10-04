#include "type.hpp"

bool isHttpMethod(const std::string& string)
{
	std::string allow_methods[9] = {"GET", "POST", "PUT", \
								"PATCH", "DELETE", "HEAD", \
								"OPTIONS", "CONNECT", "TRACE"};
	int idx = 0;

	while (idx < 9 && string != allow_methods[idx])
		idx++;
	return idx != 9;
}

bool	isInteger(const std::string& string)
{
	int	idx = 0;
	int	size = string.size();

	while (idx < size && isdigit(string[idx]))
		idx++;
	return idx == size;
}