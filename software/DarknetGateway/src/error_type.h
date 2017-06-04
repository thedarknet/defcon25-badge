#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

#include <stdint.h>

class ErrorType {
public:
	enum ErrorNum {
		NO_ERROR = 0,
		SPI_INIT_ERROR
	};
public:
	ErrorType();
	ErrorType(ErrorNum en);
	ErrorType(const ErrorType &r);
	bool ok();
	ErrorType &operator=(const ErrorType &r);
	uint8_t getError() {return ErrorNumber;}
	const char *getMessage();
private:
	ErrorNum ErrorNumber;
};

#endif
