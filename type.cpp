#include <string>

bool	isInteger(const std::string& string)
{
	int	idx = 0;
	int	size = string.size();

	while (idx < size && isdigit(string[idx]))
		idx++;
	return idx == size;
}