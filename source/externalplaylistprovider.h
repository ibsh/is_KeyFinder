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

#ifndef EXTERNALPLAYLIST_H
#define EXTERNALPLAYLIST_H

#include <QUrl>
#include <QMutex>
#include <QRegularExpressionMatch>
#include <QtXml/QtXml>
#include <QtXmlPatterns/QtXmlPatterns>

#include "preferences.h"
#include "strings.h"
#include "externalplaylistproviderserato.h"

const QString SOURCE_KEYFINDER = GuiStrings::getInstance()->appName();
const QString SOURCE_ITUNES    = "iTunes";
const QString SOURCE_TRAKTOR   = "Traktor";
const QString SOURCE_SERATO    = "Serato";

enum external_playlist_source_t {
  EXTERNAL_PLAYLIST_SOURCE_ITUNES,
  EXTERNAL_PLAYLIST_SOURCE_TRAKTOR,
  EXTERNAL_PLAYLIST_SOURCE_SERATO,
};

class ExternalPlaylist {
public:
  ExternalPlaylist(const QString&, const external_playlist_source_t);
  QString name;
  external_playlist_source_t source;
  QList<QUrl> tracks;
};

class ExternalPlaylistProvider {
public:
  static QList<ExternalPlaylist> readLibrary(const Preferences&);
  static QList<QUrl> readITunesStandalonePlaylist(const QString&);
  static QList<QUrl> readM3uStandalonePlaylist(const QString&);
private:
  // iTunes
  static QList<ExternalPlaylist> readPlaylistsFromITunesLibrary(const Preferences&);
  static QList<ExternalPlaylist> readPlaylistsFromITunesFile(QFile& xmlFile);
  static QUrl fixITunesAddressing(const QString&);
  // Traktor
  static QList<ExternalPlaylist> readPlaylistsFromTraktorLibrary(const Preferences&);
  static QUrl fixTraktorAddressing(const QString&);
  // Serato
  static QList<ExternalPlaylist> readPlaylistsFromSeratoLibrary(const Preferences&);
  static QList<QUrl> readSeratoLibraryPlaylist(const QString&, const Preferences&);
};

#endif // EXTERNALPLAYLIST_H
