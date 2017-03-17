#include "error_type.h"

ErrorType::ErrorType() : ErrorNumber(ErrorNum::NO_ERROR) {
}

ErrorType::ErrorType(ErrorNum en) : ErrorNumber(en) {
}

bool ErrorType::ok() {
	return ErrorNumber==ErrorNum::NO_ERROR;
}
