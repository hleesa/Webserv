#ifndef CODETOREASON_HPP
#define CODETOREASON_HPP

#include <map>
#include <string>

class CodeToReason {
	private:
		std::map<int, std::string> codeToReason;
		
		void init();

	public:
		CodeToReason();
		
		std::string getReasonPharse(const int status_code);
};

#endif
