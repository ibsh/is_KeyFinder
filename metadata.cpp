#include "metadata.h"

#include "metadatataglib.h"

Metadata* Metadata::getMetadata(char* c){
	return new TagLibMetadata(c);
}

