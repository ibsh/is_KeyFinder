#include "decoderlibav.h"

AudioBuffer* LibAvDecoder::decodeFile(char* filename) throw (Exception){
	av_register_all();
	AVCodec *codec = NULL;
	AVFormatContext *fCtx = NULL;
	AVCodecContext *cCtx = NULL;
	// Find audio stream
	if(avformat_open_input(&fCtx, filename, NULL, NULL) != 0)
		throw Exception("Failed to open audio file");
	if(av_find_stream_info(fCtx) < 0)
		throw Exception("Failed to find stream information");
	int audioStream = -1;
	for(int i=0; i<(signed)fCtx->nb_streams; i++){
		if(fCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			audioStream = i;
			break;
		}
	}
	if(audioStream == -1)
		throw Exception("Failed to find an audio stream");
	// Determine stream codec
	cCtx = fCtx->streams[audioStream]->codec;
	codec = avcodec_find_decoder(cCtx->codec_id);
	if(codec == NULL)
		throw Exception("Audio stream has unsupported codec");
	if(avcodec_open(cCtx, codec) < 0)
		throw Exception("Error opening codec");
	// Prep buffer
	AudioBuffer *ab = new AudioBuffer();
	ab->setFrameRate(cCtx->sample_rate);
	ab->setChannels(cCtx->channels);
	// Decode stream
	AVPacket avpkt;
	av_init_packet(&avpkt);
	while(av_read_frame(fCtx, &avpkt) == 0){
		if(avpkt.stream_index == audioStream)
			if(decodePacket(cCtx, &avpkt, ab) != 0)
				std::cerr << "Error while processing packet" << std::endl;
		av_free_packet(&avpkt);
	}
	avcodec_close(cCtx);
	av_close_input_file(fCtx);
	return ab;
}

int LibAvDecoder::decodePacket(AVCodecContext* cCtx, AVPacket* avpkt, AudioBuffer* ab){
	int16_t outputBuffer[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int16_t *samples = (int16_t*)outputBuffer;
	int outputBufferSize, bytesConsumed;
	while (avpkt->size > 0) {
		outputBufferSize = sizeof(outputBuffer);
		bytesConsumed = avcodec_decode_audio3(cCtx, samples, &outputBufferSize, avpkt);
		if(bytesConsumed <= 0){ // < 0 for an error, == 0 for no frame data decompressed
			avpkt->size = 0;
			return 1;
		}else{
			int newSamplesDecoded = outputBufferSize/sizeof(int16_t);
			int oldSampleCount = ab->getSampleCount();
			ab->addSamples(newSamplesDecoded);
			for(int i=0; i<newSamplesDecoded; i++)
				ab->setSample(oldSampleCount+i,(float)samples[i]); // can divide samples[i] by 32768 if you want unity values. Makes no difference.
		}
		if(bytesConsumed < avpkt->size){
			int newLength = avpkt->size - bytesConsumed;
			uint8_t* datacopy = avpkt->data;
			avpkt->data = (uint8_t*)malloc(newLength);
			memcpy(avpkt->data,datacopy + bytesConsumed,newLength);
			free(datacopy); // was av_free(), but no symbol for that exists in the newer libav.
			// I haven't found a codec which gets us into this conditional anyway.
		}
		avpkt->size -= bytesConsumed;
	}
	return 0;
}
