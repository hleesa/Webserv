#ifndef BODY_HPP
#define BODY_HPP

#include "BodyStatus.hpp"
#include <string>

struct Body {
	BodyStatus status;
	std::string content;
	int length;

	Body() {
		status = None;
		content.clear();
		length = 0;
	};
};

#endif