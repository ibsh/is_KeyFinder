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

QMutex codecMutex; // I don't think this should be necessary if I get the lock manager right.

AudioStream* LibAvDecoder::decodeFile(const QString& filePath){

  QMutexLocker codecMutexLocker(&codecMutex); // mutex the preparatory section of this method

  AVCodec *codec = NULL;
  AVFormatContext *fCtx = NULL;
  AVCodecContext *cCtx = NULL;
  AVDictionary* dict = NULL;

  // convert filepath
#ifdef Q_OS_WIN
  const wchar_t* filePathWc = reinterpret_cast<const wchar_t*>(filePath.constData());
  const char* filePathCh = utf16_to_utf8(filePathWc);
#else
  QByteArray encodedPath = QFile::encodeName(filePath);
  const char* filePathCh = encodedPath;
#endif

  // open file
  int openInputResult = avformat_open_input(&fCtx, filePathCh, NULL, NULL);
  if(openInputResult != 0){
    qCritical("Failed to open audio file: %s (%d)", filePathCh, openInputResult);
    throw Exception();
  }

  if(avformat_find_stream_info(fCtx, 0) < 0){
    qCritical("Failed to find stream information in file: %s", filePathCh);
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
    qCritical("Failed to find an audio stream in file: %s", filePathCh);
    throw Exception();
  }

  // Determine stream codec
  cCtx = fCtx->streams[audioStream]->codec;
  codec = avcodec_find_decoder(cCtx->codec_id);
  if(codec == NULL){
    qCritical("Audio stream has unsupported codec in file: %s", filePathCh);
    throw Exception();
  }

  // Open codec
  int codecOpenResult = avcodec_open2(cCtx, codec, &dict);
  if(codecOpenResult < 0){
    qCritical("Error opening audio codec: %s (%d)", codec->long_name, codecOpenResult);
    throw Exception();
  }

  ReSampleContext* rsCtx = av_audio_resample_init(
        cCtx->channels, cCtx->channels,
        cCtx->sample_rate, cCtx->sample_rate,
        AV_SAMPLE_FMT_S16, cCtx->sample_fmt,
        0, 0, 0, 0);
  if(rsCtx == NULL){
    qCritical("Failed to create ReSampleContext for file: %s", filePathCh);
    throw Exception();
  }

  qDebug("Decoding %s (%s, %d)", filePathCh, av_get_sample_fmt_name(cCtx->sample_fmt), cCtx->sample_rate);

  codecMutexLocker.unlock();

  // Prep buffer
  AudioStream *astrm = new AudioStream();
  astrm->setFrameRate(cCtx->sample_rate);
  astrm->setChannels(cCtx->channels);
  // Decode stream
  AVPacket avpkt;
  int badPacketCount = 0;
  while(true){
    av_init_packet(&avpkt);
    if(av_read_frame(fCtx, &avpkt) < 0)
      break;
    if(avpkt.stream_index == audioStream){
      try{
        int result = decodePacket(cCtx, rsCtx, &avpkt, astrm);
        if(result != 0){
          if(badPacketCount < 100){
            badPacketCount++;
          }else{
            qCritical("100 bad packets, may be DRM or corruption in file: %s", filePathCh);
            throw Exception();
          }
        }
      }catch(Exception& e){
        throw e;
      }
    }
    av_free_packet(&avpkt);
  }

  codecMutexLocker.relock();
  audio_resample_close(rsCtx);
  int codecCloseResult = avcodec_close(cCtx);
  if(codecCloseResult < 0){
    qCritical("Error closing audio codec: %s (%d)", codec->long_name, codecCloseResult);
  }
  codecMutexLocker.unlock();

  avformat_close_input(&fCtx);
  return astrm;
}

LibAvDecoder::LibAvDecoder(){
  frameBufferSize = ((AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2) * sizeof(uint8_t);
  frameBuffer = (uint8_t*)av_malloc(frameBufferSize);
  frameBufferConverted = (uint8_t*)av_malloc(frameBufferSize);
}

LibAvDecoder::~LibAvDecoder(){
  av_free(frameBuffer);
  av_free(frameBufferConverted);
}

int LibAvDecoder::decodePacket(AVCodecContext* cCtx, ReSampleContext* rsCtx, AVPacket* originalPacket, AudioStream* ab){
  // copy packet so we can shift data pointer about without endangering garbage collection
  AVPacket tempPacket;
  tempPacket.size = originalPacket->size;
  tempPacket.data = originalPacket->data;
  // loop in case audio packet contains multiple frames
  while(tempPacket.size > 0){
    int dataSize = frameBufferSize;
    int16_t* dataBuffer = (int16_t*)frameBuffer;
    int bytesConsumed = avcodec_decode_audio3(cCtx, dataBuffer, &dataSize, &tempPacket);
    if(bytesConsumed < 0){ // error
      tempPacket.size = 0;
      return 1;
    }
    tempPacket.data += bytesConsumed;
    tempPacket.size -= bytesConsumed;
    if(dataSize <= 0)
      continue; // nothing decoded
    int newSamplesDecoded = dataSize / av_get_bytes_per_sample(cCtx->sample_fmt);
    // Resample if necessary
    if(cCtx->sample_fmt != AV_SAMPLE_FMT_S16){
      int resampleResult = audio_resample(rsCtx, (short*)frameBufferConverted, (short*)frameBuffer, newSamplesDecoded);
      if(resampleResult < 0){
        qCritical("Failed to resample");
        throw Exception();
      }
      dataBuffer = (int16_t*)frameBufferConverted;
    }
    int oldSampleCount = ab->getSampleCount();
    try{
      ab->addToSampleCount(newSamplesDecoded);
    }catch(Exception& e){
      throw e;
    }
    for(int i = 0; i < newSamplesDecoded; i++){
      ab->setSample(oldSampleCount+i, (float)dataBuffer[i]);
    }
  }
  return 0;
}
