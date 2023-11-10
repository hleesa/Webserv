#include <string>

bool isHttpMethod(const std::string& method)
{
	std::string allow_methods[3] = {"GET", "POST", "DELETE"};
	int idx = 0;

	while (idx < 3 && method != allow_methods[idx])
		idx++;
	return idx != 3;
}

bool	isNumber(const std::string& string)
{
	int	idx = 0;
	int	size = string.size();

	while (idx < size && isdigit(string[idx]))
		idx++;
	return idx == size;
}