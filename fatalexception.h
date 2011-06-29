#ifndef FATALEXCEPTION_H
#define FATALEXCEPTION_H

#include <string>

class FatalException{
public:
	std::string message;
	FatalException(std::string);
};

#endif
