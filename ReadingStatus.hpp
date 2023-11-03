#ifndef READINGSTATUS_HPP
#define READINGSTATUS_HPP

enum ReadingStatus {
	StartLine,
	HEADER,
	BODY,
	END,
	ERROR
};

#endif