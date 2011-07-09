#include "exception.h"

Exception::Exception(char* c): message(c){}

const char* Exception::getMessage() const{
	return message;
}
