#include "metadatareaderlibav.h"

LibAvMetadataReader::LibAvMetadataReader(char* fileName){
	av_register_all();
	fCtx = NULL;
	if (avformat_open_input(&fCtx, fileName, NULL, NULL) != 0)
		fCtx = NULL;
}

LibAvMetadataReader::~LibAvMetadataReader(){
	if(fCtx != NULL)
		avformat_free_context(fCtx);
}

std::string LibAvMetadataReader::getTitle(){
	if(fCtx == NULL) return "";
	AVDictionaryEntry* tag = NULL;
	tag = av_dict_get(fCtx->metadata, "title", NULL, AV_DICT_IGNORE_SUFFIX);
	if(tag != NULL)
		return tag->value;
	else
		return "";
}

std::string LibAvMetadataReader::getArtist(){
	if(fCtx == NULL) return "";
	AVDictionaryEntry* tag = NULL;
	tag = av_dict_get(fCtx->metadata, "artist", NULL, AV_DICT_IGNORE_SUFFIX);
	if(tag != NULL)
		return tag->value;
	else
		return "";
}
