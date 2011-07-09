#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

class Exception{
public:
	Exception(char*);
	const char* getMessage() const;
private:
	char* message;
};

#endif
