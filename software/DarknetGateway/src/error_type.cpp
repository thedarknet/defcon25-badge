#include "error_type.h"

ErrorType::ErrorType() : ErrorNumber(ErrorType::ErrorNum::NO_ERROR) {
}

ErrorType::ErrorType(ErrorNum en) : ErrorNumber(en) {
}

bool ErrorType::ok() {
	return ErrorNumber==ErrorNum::NO_ERROR;
}


const char *ErrorType::getMessage() {
	return "ErrorType:  TODO";
}

ErrorType::ErrorType(const ErrorType &r) {
	(*this)=r;
}

ErrorType &ErrorType::operator=(const ErrorType &r) {
	ErrorNumber = r.ErrorNumber;
	return (*this);
}
