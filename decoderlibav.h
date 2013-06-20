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

#ifndef LIBAVDECODER_H
#define LIBAVDECODER_H

#include <iomanip>

#include <QString>
#include <QMutex>
#include <QFile>

#include "keyfinder/exception.h"
#include "keyfinder/audiodata.h"

#include "strings.h"

#ifndef INT64_C
#define UINT64_C(c) (c ## ULL)
#endif
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
extern "C"{
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#ifdef Q_OS_WIN
#include "os_windows.h"
#endif

class AudioFileDecoder{
public:
  AudioFileDecoder(const QString&, const int);
  ~AudioFileDecoder();
  KeyFinder::AudioData* decodeFile(const bool&);
private:
  char* filePathCh;
  uint8_t* frameBuffer;
  uint8_t* frameBufferConverted;
  int frameBufferSize;
  int audioStream;
  int badPacketCount;
  int badPacketThreshold;
  AVCodec* codec;
  AVFormatContext* fCtx;
  AVCodecContext* cCtx;
  AVDictionary* dict;
  ReSampleContext* rsCtx;
  bool decodePacket(AVPacket*, KeyFinder::AudioData*);
};

#endif
