#include "exception.h"

const char* Ex::what() const noexcept {
	return _msg; 
}