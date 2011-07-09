#ifndef METADATAREADER_H
#define METADATAREADER_H

#include <iostream>
#include <string>

class MetadataReader{
public:
	virtual std::string getTitle() = 0;
	virtual std::string getArtist() = 0;
};

#endif // METADATAREADER_H
