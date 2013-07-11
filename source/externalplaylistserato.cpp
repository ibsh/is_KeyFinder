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

#include "externalplaylistserato.h"

QStringList SeratoDataStream::readCrate(QIODevice* device, CrateType ctype) {
  QStringList results;
  // crate type specific
  QString version;
  QString type;
  if (ctype == SeratoDataStream::SUBCRATE) {
    version = "81.0";
    type = "/Serato ScratchLive Crate";
  } else {
    version = "D1.0";
    type = "/Serato ScratchLive Smart Crate";
  }
  // open stream and read necessary headers
  strm = new QDataStream(device);
  readSingleByteString(4).toLocal8Bit().constData();
  skipBytes(2);
  readDoubleByteString(version.size() * 2);
  readDoubleByteString(type.size() * 2);
  while (!strm->atEnd()) {
    QString entryName = readSingleByteString(4);
    int length = readInt(4);
    QString entryData;
    if (entryName.left(4) == "otrk") { // bizarre lengthening in Windows
      skipBytes(8);
      entryData = readDoubleByteString(length - 8);
      results.push_back(entryData);
    } else {
      entryData = readSingleByteString(length);
    }
  }
  delete strm;
  return results;
}

QString SeratoDataStream::readSingleByteString(int chars) {
  char* data = new char[chars];
  strm->readRawData(data, chars);
  QString stringRead(data);
  delete[] data;
  return stringRead;
}

QString SeratoDataStream::readDoubleByteString(int chars) {
  QString stringRead;
  for (int i=0; i<(signed)chars; i += 2) {
    char* twoBytes = new char[2];
    strm->readRawData(twoBytes, 2);
    stringRead += QChar(twoBytes[1],twoBytes[0]);
    delete[] twoBytes;
  }
  return stringRead;
}

int SeratoDataStream::readInt(int bytes) {
  int number = 0;
  for (int i = 0; i < bytes; i++) {
    char* data = new char[1];
    strm->readRawData(data, 1);
    number *= 256;
    int n = (int)data[0];
    number += (n >= 0 ? n : n + 256);
    delete[] data;
  }
  return number;
}

void SeratoDataStream::skipBytes(int bytesToSkip) {
  strm->skipRawData(bytesToSkip);
}
