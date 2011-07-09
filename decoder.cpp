#include "decoder.h"

#include "decoderlibav.h"
#include "decoderlibsndfile.h"

AudioFileDecoder* AudioFileDecoder::getDecoder(const std::string& filePath){
	std::string fileExt  = filePath.substr(filePath.rfind(".")+1);
	// choose the default for this file extension
	if(fileExt == "wav" || fileExt == "aif" || fileExt == "aiff" || fileExt == "flac"){
		return new LibSndFileDecoder();
	}else{
		return new LibAvDecoder();
	}
}
