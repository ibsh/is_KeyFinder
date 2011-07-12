#include "exception.h"

Exception::Exception(const char* c){
	std::cerr << "EXCEPTION: " << c << std::endl;
}
