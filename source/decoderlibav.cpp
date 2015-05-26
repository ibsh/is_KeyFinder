/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

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

#define AVCODEC_MAX_AUDIO_FRAME_SIZE_IS_DEAD 192000

QMutex codecMutex;

AudioFileDecoder::AudioFileDecoder(const QString& filePath, const int maxDuration) : filePathCh(NULL), frameBufferSize(((AVCODEC_MAX_AUDIO_FRAME_SIZE_IS_DEAD * 3) / 2) * sizeof(uint8_t)), audioStream(-1), badPacketCount(0), badPacketThreshold(100), codec(NULL), fCtx(NULL), cCtx(NULL), dict(NULL), rsCtx(NULL), decodedFrame(NULL) {

  // convert filepath
#ifdef Q_OS_WIN
  const wchar_t* filePathWc = reinterpret_cast<const wchar_t*>(filePath.constData());
  filePathCh = qstrdup(utf16_to_utf8(filePathWc));
#else
  QByteArray encodedPath = QFile::encodeName(filePath);
  filePathCh = qstrdup(encodedPath.constData());
#endif

  frameBuffer = (uint8_t*)av_malloc(frameBufferSize);
  frameBufferConverted = (uint8_t*)av_malloc(frameBufferSize);

  QMutexLocker codecMutexLocker(&codecMutex); // mutex the libAV preparation

  // open file
  int openInputResult = avformat_open_input(&fCtx, filePathCh, NULL, NULL);
  if (openInputResult != 0) {
    qWarning("Could not open file %s (%d)", filePathCh, openInputResult);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotOpenFile(openInputResult).toUtf8().constData());
  }

  if (avformat_find_stream_info(fCtx, NULL) < 0) {
    qWarning("Could not find stream information for file %s", filePathCh);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotFindStreamInformation().toUtf8().constData());
  }

  for (int i=0; i<(signed)fCtx->nb_streams; i++) {
    if (fCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
      audioStream = i;
      break;
    }
  }

  if (audioStream == -1) {
    qWarning("Could not find an audio stream for file %s", filePathCh);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotFindAudioStream().toUtf8().constData());
  }

  // Determine duration
  int durationSeconds = fCtx->duration / AV_TIME_BASE;
  int durationMinutes = durationSeconds / 60;
  // First condition is a hack for bizarre overestimation of some MP3s
  if (durationMinutes < 720 && durationSeconds > maxDuration * 60) {
    qWarning("Duration of file %s (%d:%d) exceeds specified maximum (%d:00)", filePathCh, durationMinutes, durationSeconds % 60, maxDuration);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->durationExceedsPreference(durationMinutes, durationSeconds % 60, maxDuration).toUtf8().constData());
  }

  // Determine stream codec
  cCtx = fCtx->streams[audioStream]->codec;
  codec = avcodec_find_decoder(cCtx->codec_id);
  if (codec == NULL) {
    qWarning("Audio stream has unsupported codec in file %s", filePathCh);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavUnsupportedCodec().toUtf8().constData());
  }

  // Open codec
  int codecOpenResult = avcodec_open2(cCtx, codec, &dict);
  if (codecOpenResult < 0) {
    qWarning("Could not open audio codec %s (%d) for file %s", codec->long_name, codecOpenResult, filePathCh);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotOpenCodec(codec->long_name, codecOpenResult).toUtf8().constData());
  }

  rsCtx = avresample_alloc_context();

  if (rsCtx == NULL) {
    qWarning("Could not create ReSampleContext for file %s", filePathCh);
    free();
    throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotCreateResampleContext().toUtf8().constData());
  }

  av_opt_set_int(rsCtx, "in_channel_layout",  cCtx->channels,    0);
  av_opt_set_int(rsCtx, "out_channel_layout", cCtx->channels,    0);
  av_opt_set_int(rsCtx, "in_sample_rate",     cCtx->sample_rate, 0);
  av_opt_set_int(rsCtx, "out_sample_rate",    cCtx->sample_rate, 0);
  av_opt_set_int(rsCtx, "in_sample_fmt",      cCtx->sample_fmt,  0);
  av_opt_set_int(rsCtx, "out_sample_fmt",     AV_SAMPLE_FMT_S16, 0);

  qDebug("Decoder prepared for %s (%s, %d)", filePathCh, av_get_sample_fmt_name(cCtx->sample_fmt), cCtx->sample_rate);
}

void AudioFileDecoder::free() {

  av_free(frameBuffer);
  av_free(frameBufferConverted);

  if (rsCtx != NULL) {
    avresample_close(rsCtx);
  }

  if (cCtx != NULL) {
    int codecCloseResult = avcodec_close(cCtx);
    if (codecCloseResult < 0) {
      qCritical("Error closing audio codec: %s (%d)", codec->long_name, codecCloseResult);
    }
  }

  if (fCtx != NULL) {
    avformat_close_input(&fCtx);
  }

  if (filePathCh != NULL) {
    delete[] filePathCh;
  }
}

AudioFileDecoder::~AudioFileDecoder() {
  QMutexLocker codecMutexLocker(&codecMutex);
  free();
}

KeyFinder::AudioData* AudioFileDecoder::decodeNextAudioPacket() {

  // Prep buffer
  KeyFinder::AudioData* audioData = NULL;

  // Decode stream
  AVPacket avpkt;
  do {
    av_init_packet(&avpkt);
    if (av_read_frame(fCtx, &avpkt) < 0) return audioData;
    if (avpkt.stream_index != audioStream) av_free_packet(&avpkt);
  } while (avpkt.data == NULL);

  try {

    audioData = new KeyFinder::AudioData();
    audioData->setFrameRate((unsigned int) cCtx->sample_rate);
    audioData->setChannels(cCtx->channels);

    if (!decodePacket(&avpkt, audioData)) {

      badPacketCount++;
      if (badPacketCount > badPacketThreshold) {
        av_free_packet(&avpkt);
        qWarning("Too many bad packets (%d) while decoding file %s", badPacketCount, filePathCh);
        throw KeyFinder::Exception(GuiStrings::getInstance()->libavTooManyBadPackets(badPacketThreshold).toUtf8().constData());
      }
    }

  } catch (KeyFinder::Exception& e) {

    av_free_packet(&avpkt);
    qWarning("Encountered KeyFinder::Exception (%s) while decoding file %s", e.what(), filePathCh);
    throw e;

  } catch (...) {

    qWarning("Encountered unknown exception while decoding file %s", filePathCh);
  }

  av_free_packet(&avpkt);
  return audioData;
}

bool AudioFileDecoder::decodePacket(AVPacket* originalPacket, KeyFinder::AudioData* audioData) {

  // copy packet so we can shift data pointer about without endangering garbage collection
  AVPacket tempPacket;
  tempPacket.size = originalPacket->size;
  tempPacket.data = originalPacket->data;

  // loop in case audio packet contains multiple frames
  while (tempPacket.size > 0) {

    int16_t* dataBuffer = (int16_t*)frameBuffer;

    if (!decodedFrame) {

      if (!(decodedFrame = av_frame_alloc())) {

        throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotAllocateFrame().toUtf8().constData());
      }

    } else {

      av_frame_unref(decodedFrame);
    }

    int gotFrame = 0;
    int bytesConsumed = avcodec_decode_audio4(cCtx, decodedFrame, &gotFrame, &tempPacket);
    if (bytesConsumed < 0) {
      tempPacket.size = 0;
      return false;
    }

    tempPacket.data += bytesConsumed;
    tempPacket.size -= bytesConsumed;

    if (gotFrame == 0) {
      continue; // nothing decoded
    }

    int newSamplesDecoded = av_samples_get_buffer_size(NULL, cCtx->channels, decodedFrame->nb_samples, cCtx->sample_fmt, 1);

    // Resample if necessary
//    if (cCtx->sample_fmt != AV_SAMPLE_FMT_S16) {
//      int resampleResult = avresample_convert(rsCtx, (uint8_t**)&frameBufferConverted, 0, frameBufferSize, (uint8_t**)&frameBuffer, 0, newSamplesDecoded);
//      if (resampleResult < 0) {
//        throw KeyFinder::Exception(GuiStrings::getInstance()->libavCouldNotResample().toUtf8().constData());
//      }
//      dataBuffer = (int16_t*)frameBufferConverted;
//    }

    int oldSampleCount = audioData->getSampleCount();
    audioData->addToSampleCount(newSamplesDecoded);
    audioData->resetIterators();
    audioData->advanceWriteIterator(oldSampleCount);

    for (int i = 0; i < newSamplesDecoded; i++) {
      audioData->setSampleAtWriteIterator(static_cast<double>(dataBuffer[i]));
      audioData->advanceWriteIterator();
    }
  }
  return true;
}
