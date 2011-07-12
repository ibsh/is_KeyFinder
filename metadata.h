#ifndef METADATA_H
#define METADATA_H

#include <iostream>
#include <string>

class Metadata{
public:
	static Metadata* getMetadata(char*);
	virtual std::string getTitle() const = 0;
	virtual std::string getArtist() const = 0;
	virtual std::string getGrouping() const = 0;
	virtual void setGrouping(char*) = 0;
};

#endif // METADATA_H
