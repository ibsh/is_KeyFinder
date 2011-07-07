#include "decoderlibav.h"

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

using std::cerr;
using std::endl;

AudioBuffer* LibAvDecoder::decodeFile(char* filename) throw (FatalException){
	av_register_all();
	AVCodec *codec = NULL;
	AVFormatContext *fCtx = NULL;
	AVCodecContext *cCtx = NULL;
	// Find audio stream
	if(av_open_input_file(&fCtx, filename, NULL, 0, NULL) != 0) throw FatalException("Failed to open audio file");
	if(av_find_stream_info(fCtx) < 0) throw FatalException("Failed to find stream information");
	int audioStream = -1;
	for(int i=0; i<fCtx->nb_streams; i++){
		if(fCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){ // was CODEC_TYPE_AUDIO under Linux (older version)
			audioStream = i;
			break;
		}
	}
	if(audioStream == -1) throw FatalException("Failed to find an audio stream");
	// Determine stream codec
	cCtx = fCtx->streams[audioStream]->codec;
	codec = avcodec_find_decoder(cCtx->codec_id);
	if(codec == NULL) throw FatalException("Audio stream has unsupported codec");
	if(avcodec_open(cCtx, codec) < 0) throw FatalException("Error opening codec");
	// Prep buffer
	AudioBuffer *ab = new AudioBuffer();
	ab->setFrameRate(cCtx->sample_rate);
	ab->setChannels(cCtx->channels);
	// Decode stream
	AVPacket avpkt;
	av_init_packet(&avpkt);
	while(av_read_frame(fCtx, &avpkt) == 0){
		if(avpkt.stream_index == audioStream){
			if(decodePacket(cCtx, &avpkt, ab)!=0){
				cerr << "Error while processing packet" << endl;
			}
		}
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
		if(bytesConsumed < 0){
			avpkt->size = 0;
			return 1;
		}else{
			int samplesDecoded = outputBufferSize/sizeof(int16_t);
			int oldSampleCount = ab->getSampleCount();
			ab->addSamples(samplesDecoded);
			for(int i=0; i<samplesDecoded; i++){
				ab->setSample(oldSampleCount+i,((float)samples[i] / 32768.0));
			}
		}
		if(bytesConsumed < avpkt->size){
			int newLength = avpkt->size - bytesConsumed;
			uint8_t* datacopy = avpkt->data;
			avpkt->data = (uint8_t*)malloc(newLength);
			memcpy(avpkt->data,datacopy + bytesConsumed,newLength);
			free(datacopy); // was av_free(), but no symbol for that exists any more.
		}
		avpkt->size -= bytesConsumed;
	}
	return 0;
}
