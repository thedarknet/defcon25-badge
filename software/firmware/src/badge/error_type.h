#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

#include "stdint.h"

class ErrorType {
public:
	enum ErrorNum {
		NO_ERROR = 0,
		SPI_INIT_ERROR
	};
public:
	ErrorType();
	ErrorType(ErrorNum en);
	bool ok();
private:
	ErrorNum ErrorNumber;
};

#endif
