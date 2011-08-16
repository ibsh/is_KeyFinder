/*************************************************************************

	Copyright 2011 Ibrahim Sha'ath

	This file is part of KeyFinder.

	KeyFinder is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	KeyFinder is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "decoderlibav.h"


// Thread safety is a bit more complex here, see av_lockmgr_register documentation

bool LibAvDecoder::concurrencyRegistered = false;

LibAvDecoder::LibAvDecoder(){
	if(!concurrencyRegistered){
		if(av_lockmgr_register(libAv_mutexManager)){
			qCritical("Failed to register LibAV concurrency manager");
			throw Exception();
		}
		concurrencyRegistered = true;
	}
}

int LibAvDecoder::libAv_mutexManager(void** av_mutex, enum AVLockOp op){
	QMutex* kf_mutex;
	switch(op){
		case AV_LOCK_CREATE:
			try{
				kf_mutex = new QMutex();
				*av_mutex = kf_mutex;
			}catch(...){
				return 1;
			}
			return 0;
		case AV_LOCK_OBTAIN:
			kf_mutex = (QMutex*)*av_mutex;
			if(kf_mutex->tryLock()){
				return 0;
			}else{
				return 1;
			}
		case AV_LOCK_RELEASE:
			kf_mutex = (QMutex*)*av_mutex;
			kf_mutex->unlock();
			return 0;
		case AV_LOCK_DESTROY:
			kf_mutex = (QMutex*)*av_mutex;
			delete kf_mutex;
			*av_mutex = NULL;
			return 0;
	}
	return 1;
}

AudioBuffer* LibAvDecoder::decodeFile(char* fileName) throw (Exception){
	av_register_all();
	AVCodec *codec = NULL;
	AVFormatContext *fCtx = NULL;
	AVCodecContext *cCtx = NULL;
	// Find audio stream
	if(avformat_open_input(&fCtx, fileName, NULL, NULL) != 0){
		qCritical("Failed to open audio file: %s", fileName);
		throw Exception();
	}
	if(av_find_stream_info(fCtx) < 0){
		qCritical("Failed to find stream information in file: %s", fileName);
		throw Exception();
	}
	int audioStream = -1;
	for(int i=0; i<(signed)fCtx->nb_streams; i++){
		if(fCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			audioStream = i;
			break;
		}
	}
	if(audioStream == -1){
		qCritical("Failed to find an audio stream in file: %s", fileName);
		throw Exception();
	}
	// Determine stream codec
	cCtx = fCtx->streams[audioStream]->codec;
	codec = avcodec_find_decoder(cCtx->codec_id);
	if(codec == NULL){
		qCritical("Audio stream has unsupported codec in file: %s", fileName);
		throw Exception();
	}
	if(avcodec_open(cCtx, codec) < 0){
		qCritical("Error opening audio codec: %s", codec->long_name);
		throw Exception();
	}
	// Prep buffer
	AudioBuffer *ab = new AudioBuffer();
	ab->setFrameRate(cCtx->sample_rate);
	ab->setChannels(cCtx->channels);
	// Decode stream
	AVPacket avpkt;
	av_init_packet(&avpkt);
	while(av_read_frame(fCtx, &avpkt) == 0){
		if(avpkt.stream_index == audioStream)
			try{
				if(decodePacket(cCtx, &avpkt, ab) != 0)
					qWarning("LibAV: Error while processing packet");
			}catch(Exception& e){
				throw e;
			}
		av_free_packet(&avpkt);
	}
	avcodec_close(cCtx);
	av_close_input_file(fCtx);
	return ab;
}

int LibAvDecoder::decodePacket(AVCodecContext* cCtx, AVPacket* avpkt, AudioBuffer* ab) throw (Exception){
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
			try{
				ab->addSamples(newSamplesDecoded);
			}catch(Exception& e){
				throw e;
			}
			for(int i=0; i<newSamplesDecoded; i++)
				ab->setSample(oldSampleCount+i,(float)samples[i]); // can divide samples[i] by 32768 if you want unity values. Makes no difference.
		}
		if(bytesConsumed < avpkt->size){
			int newLength = avpkt->size - bytesConsumed;
			uint8_t* datacopy = avpkt->data;
			avpkt->data = (uint8_t*)malloc(newLength);
			memcpy(avpkt->data,datacopy + bytesConsumed,newLength);
			av_free(datacopy);
		}
		avpkt->size -= bytesConsumed;
	}
	return 0;
}
