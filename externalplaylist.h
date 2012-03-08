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

#ifndef EXTERNALPLAYLIST_H
#define EXTERNALPLAYLIST_H

#include <QUrl>
#include <QMutex>
#ifdef Q_OS_WIN
#include <QtXml/QtXml>
#include <QtXmlPatterns/QtXmlPatterns>
#else
#include <Qt/QtXml>
#include <Qt/QtXmlPatterns>
#include <xqilla/xqilla-simple.hpp>
#endif

#include "preferences.h"

const QString SOURCE_KEYFINDER = "KeyFinder";
const QString SOURCE_ITUNES = "iTunes";
const QString SOURCE_TRAKTOR = "Traktor";
const QString SOURCE_SERATO = "Serato";

class ExternalPlaylistObject{
public:
  ExternalPlaylistObject(const QString&, const QString&);
  QString name;
  QString source;
};

class ExternalPlaylist{
public:
  static QList<ExternalPlaylistObject> readLibrary(const Preferences&);
  static QList<QUrl> readLibraryPlaylist(const QString&, const QString&, const Preferences&);
  static QList<QUrl> readITunesStandalonePlaylist(const QString&);
  static QList<QUrl> readM3uStandalonePlaylist(const QString&);
private:
  // iTunes
  static QList<ExternalPlaylistObject> readPlaylistsFromITunesLibrary(const Preferences&);
  static QList<QUrl> readITunesLibraryPlaylist(const QString&, const Preferences&);
  static QUrl fixITunesAddressing(const QString&);
  // Serato
  static QList<ExternalPlaylistObject> readPlaylistsFromSeratoLibrary(const Preferences&);
  static QList<QUrl> readSeratoLibraryPlaylist(const QString&, const Preferences&);
  static QUrl fixSeratoAddressing(const QString&, const QString&);
  // Traktor
  static QList<ExternalPlaylistObject> readPlaylistsFromTraktorLibrary(const Preferences&);
  static QList<QUrl> readTraktorLibraryPlaylist(const QString&, const Preferences&);
  static QUrl fixTraktorAddressing(const QString&);
};

// composition rather than inheritance
class SeratoDataStream{
public:
  enum CrateType {
    SUBCRATE = 0,
    SMARTCRATE
  };
  QStringList readCrate(QIODevice*, CrateType);
private:
  QDataStream* strm;
  QString readSingleByteString(int);
  QString readDoubleByteString(int);
  int readInt(int);
  void skipBytes(int);
};

#endif // EXTERNALPLAYLIST_H
