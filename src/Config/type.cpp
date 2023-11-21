#include <string>

#define NUMBER_OF_METHOD 4

bool isHttpMethod(const std::string& method) {
	std::string allow_methods[NUMBER_OF_METHOD] = {"GET", "POST", "DELETE", "HEAD"};
	int idx = 0;

	while (idx < NUMBER_OF_METHOD && method != allow_methods[idx])
		idx++;
	return idx != NUMBER_OF_METHOD;
}

bool	isNumber(const std::string& string) {
	int	idx = 0;
	int	size = string.size();

	while (idx < size && isdigit(string[idx]))
		idx++;
	return idx == size;
}

bool isValidRangeStatusCode(const int status_code) {
	return ((status_code == 100 || status_code == 101)
		|| (status_code >= 200 && status_code <= 205)
		|| ((status_code >= 300 && status_code <= 305) || status_code == 307)
		|| (status_code == 400 || (status_code >= 402 && status_code <= 406)
			|| (status_code >= 408 && status_code <= 411) || (status_code >= 413 && status_code <= 415)
			|| status_code == 417 || status_code == 426)
		|| (status_code >= 500 && status_code <= 505));
}