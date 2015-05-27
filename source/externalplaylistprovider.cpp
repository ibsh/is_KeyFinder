/*************************************************************************

  Copyright 2011-2015 Ibrahim Sha'ath

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

#include "externalplaylistprovider.h"

#define ITUNES "iTunes"
#define TRAKTOR "Traktor"
#define SERATO "Serato"
#define M3U "M3U"

#define READ_LIBRARY_FAIL "%s library: failed reading library from %s"
#define READ_LIBRARY_BEGIN "%s library: reading playlists from %s"
#define READ_LIBRARY_END "%s library: successfully read %d playlists"

#define READ_PLAYLIST_BEGIN "%s playlist: reading contents of '%s'"
#define READ_PLAYLIST_END "%s playlist: successfully read %d entries"

#define READ_STANDALONE_FAIL "Standalone %s playlist: failed reading '%s'"
#define READ_STANDALONE_BEGIN "Standalone %s playlist: reading contents of '%s'"
#define READ_STANDALONE_END "Standalone %s playlist: successfully read %d entries"

QMutex externalPlaylistMutex;

ExternalPlaylist::ExternalPlaylist(const QString& n, const external_playlist_source_t s) : tracks() {
  name = n;
  source = s;
}

QList<ExternalPlaylist> ExternalPlaylistProvider::readLibrary(const Preferences& prefs) {
  QList<ExternalPlaylist> playlists;
  playlists << readPlaylistsFromITunesLibrary(prefs);
  playlists << readPlaylistsFromTraktorLibrary(prefs);
  playlists << readPlaylistsFromSeratoLibrary(prefs);
  return playlists;
}


QList<ExternalPlaylist> ExternalPlaylistProvider::readPlaylistsFromITunesLibrary(const Preferences& prefs) {

  QMutexLocker locker(&externalPlaylistMutex);
  QFile xmlFile(prefs.getITunesLibraryPath());
  QList<ExternalPlaylist> results;

  if (!xmlFile.open(QIODevice::ReadOnly)) {

    qDebug(READ_LIBRARY_FAIL, ITUNES, prefs.getITunesLibraryPath().toUtf8().constData());

  } else {

    qDebug(READ_LIBRARY_BEGIN, ITUNES, prefs.getITunesLibraryPath().toUtf8().constData());
    results = readPlaylistsFromITunesFile(xmlFile);
    qDebug(READ_LIBRARY_END, ITUNES, results.size());
  }

  return results;
}

enum itunes_library_state_t {
  ITUNES_LIBRARY_STATE_OUTSIDE,
  ITUNES_LIBRARY_STATE_TRACKS_OUTER_DICT,
  ITUNES_LIBRARY_STATE_TRACKS_TRACK_DICT,
  ITUNES_LIBRARY_STATE_PLAYLISTS_ARRAY,
  ITUNES_LIBRARY_STATE_PLAYLIST_DICT,
  ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_ARRAY,
  ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_DICT,
};

QList<ExternalPlaylist>ExternalPlaylistProvider::readPlaylistsFromITunesFile(QFile& xmlFile) {

  itunes_library_state_t state = ITUNES_LIBRARY_STATE_OUTSIDE;

  QList<ExternalPlaylist> results;

  QXmlStreamReader xmlReader(&xmlFile);
  QString key;

  QHash<QString, QString> tracks;
  QString currentTrackID;

  ExternalPlaylist *currentPlaylist = NULL;
  QStringList defaultPlaylistNames = GuiStrings::getInstance()->iTunesDefaultPlaylists();

  //Reading from the file
  while (!xmlReader.atEnd()) {

    xmlReader.readNext();
    QString elementName = xmlReader.name().toString();

    if (xmlReader.isStartElement()) {

      if (elementName == "key") {

        key = xmlReader.readElementText();

      } else if (state == ITUNES_LIBRARY_STATE_TRACKS_TRACK_DICT && key == "Track ID") {

        if (currentTrackID.isEmpty()) {

          currentTrackID = xmlReader.readElementText();
        }

      } else if (state == ITUNES_LIBRARY_STATE_TRACKS_TRACK_DICT && key == "Location") {

        if (!currentTrackID.isEmpty()) {

          tracks[currentTrackID] = xmlReader.readElementText();
          currentTrackID = "";
        }

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_DICT && key == "Name") {

        if (currentPlaylist == NULL) {

          currentPlaylist = new ExternalPlaylist(xmlReader.readElementText(), EXTERNAL_PLAYLIST_SOURCE_ITUNES);
        }

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_DICT && key == "Track ID") {

        if (currentPlaylist != NULL) {

          QString trackURLString = tracks[xmlReader.readElementText()];
          QUrl trackURL = fixITunesAddressing(trackURLString);
          currentPlaylist->tracks.push_back(trackURL);
        }

      } else if (state == ITUNES_LIBRARY_STATE_OUTSIDE && elementName == "dict" && key == "Tracks") {

        state = ITUNES_LIBRARY_STATE_TRACKS_OUTER_DICT;

      } else if (state == ITUNES_LIBRARY_STATE_TRACKS_OUTER_DICT && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_TRACKS_TRACK_DICT;

      } else if (state == ITUNES_LIBRARY_STATE_OUTSIDE && elementName == "array" && key == "Playlists") {

        state = ITUNES_LIBRARY_STATE_PLAYLISTS_ARRAY;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLISTS_ARRAY && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_PLAYLIST_DICT;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_DICT && elementName == "array" && key == "Playlist Items") {

        state = ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_ARRAY;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_ARRAY && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_DICT;
      }

    } else if (xmlReader.isEndElement()) {

      if (state == ITUNES_LIBRARY_STATE_TRACKS_TRACK_DICT && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_TRACKS_OUTER_DICT;

      } else if (state == ITUNES_LIBRARY_STATE_TRACKS_OUTER_DICT && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_OUTSIDE;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_DICT && elementName == "dict") {

        state = ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_ARRAY;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_TRACK_ARRAY && elementName == "array") {

        state = ITUNES_LIBRARY_STATE_PLAYLIST_DICT;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLIST_DICT && elementName == "dict") {

        if (currentPlaylist != NULL) {

          if (defaultPlaylistNames.contains(currentPlaylist->name)) {

            delete currentPlaylist;
            currentPlaylist = NULL;

          } else {

            results.push_back(*currentPlaylist);
            currentPlaylist = NULL;
          }
        }

        state = ITUNES_LIBRARY_STATE_PLAYLISTS_ARRAY;

      } else if (state == ITUNES_LIBRARY_STATE_PLAYLISTS_ARRAY && elementName == "array") {

        state = ITUNES_LIBRARY_STATE_OUTSIDE;
      }

    }
  }

  return results;
}

enum traktor_library_state_t {
  TRAKTOR_LIBRARY_STATE_OUTSIDE,
  TRAKTOR_LIBRARY_STATE_PLAYLISTS,
  TRAKTOR_LIBRARY_STATE_PLAYLIST_NODE,
  TRAKTOR_LIBRARY_STATE_PLAYLIST_PROPER,
  TRAKTOR_LIBRARY_STATE_PLAYLIST_ENTRY,
};

QList<ExternalPlaylist> ExternalPlaylistProvider::readPlaylistsFromTraktorLibrary(const Preferences& prefs) {

  QMutexLocker locker(&externalPlaylistMutex);
  QList<ExternalPlaylist> results;
  QFile xmlFile(prefs.getTraktorLibraryPath());

  if (!xmlFile.open(QIODevice::ReadOnly)) {

    qDebug(READ_LIBRARY_FAIL, TRAKTOR, prefs.getTraktorLibraryPath().toUtf8().constData());
    return results;

  } else {

    qDebug(READ_LIBRARY_BEGIN, TRAKTOR, prefs.getTraktorLibraryPath().toUtf8().constData());

    traktor_library_state_t state = TRAKTOR_LIBRARY_STATE_OUTSIDE;

    QXmlStreamReader xmlReader(&xmlFile);
    ExternalPlaylist *currentPlaylist = NULL;
    QStringList defaultPlaylistNames = GuiStrings::getInstance()->traktorDefaultPlaylists();

    //Reading from the file
    while (!xmlReader.atEnd()) {

      xmlReader.readNext();
      QString elementName = xmlReader.name().toString();

      if (xmlReader.isStartElement()) {

        if (state == TRAKTOR_LIBRARY_STATE_OUTSIDE && elementName == "PLAYLISTS") {

          state = TRAKTOR_LIBRARY_STATE_PLAYLISTS;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLISTS && elementName == "NODE") {

          QXmlStreamAttributes atts = xmlReader.attributes();

          if (atts.hasAttribute("TYPE") && atts.hasAttribute("NAME") && atts.value("TYPE") == "PLAYLIST") {

            if (currentPlaylist == NULL) {
              currentPlaylist = new ExternalPlaylist(atts.value("NAME").toString(), EXTERNAL_PLAYLIST_SOURCE_TRAKTOR);
            }

            state = TRAKTOR_LIBRARY_STATE_PLAYLIST_NODE;
          }

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_NODE && elementName == "PLAYLIST") {

          state = TRAKTOR_LIBRARY_STATE_PLAYLIST_PROPER;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_PROPER && elementName == "ENTRY") {

          state = TRAKTOR_LIBRARY_STATE_PLAYLIST_ENTRY;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_ENTRY && elementName == "PRIMARYKEY") {

          QXmlStreamAttributes atts = xmlReader.attributes();

          if (currentPlaylist != NULL && atts.hasAttribute("TYPE") && atts.hasAttribute("KEY") && atts.value("TYPE") == "TRACK") {

            QString trackURLString = atts.value("KEY").toString();
            QUrl trackURL = fixTraktorAddressing(trackURLString);
            currentPlaylist->tracks.push_back(trackURL);
          }
        }

      } else if (xmlReader.isEndElement()) {

        if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_ENTRY && elementName == "ENTRY") {

          state = TRAKTOR_LIBRARY_STATE_PLAYLIST_PROPER;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_PROPER && elementName == "PLAYLIST") {

          state = TRAKTOR_LIBRARY_STATE_PLAYLIST_NODE;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLIST_NODE && elementName == "NODE") {

          if (currentPlaylist != NULL) {

            if (defaultPlaylistNames.contains(currentPlaylist->name)) {

              delete currentPlaylist;
              currentPlaylist = NULL;

            } else {

              results.push_back(*currentPlaylist);
              currentPlaylist = NULL;
            }
          }

          state = TRAKTOR_LIBRARY_STATE_PLAYLISTS;

        } else if (state == TRAKTOR_LIBRARY_STATE_PLAYLISTS && elementName == "PLAYLISTS") {

          state = TRAKTOR_LIBRARY_STATE_OUTSIDE;
        }
      }
    }

    qDebug(READ_LIBRARY_END, TRAKTOR, results.size());

    return results;
  }
}

QList<ExternalPlaylist> ExternalPlaylistProvider::readPlaylistsFromSeratoLibrary(const Preferences& prefs) {

  QMutexLocker locker(&externalPlaylistMutex);
  QList<ExternalPlaylist> results;

  qDebug(READ_LIBRARY_BEGIN, SERATO, prefs.getSeratoLibraryPath().toUtf8().constData());

  QString path = prefs.getSeratoLibraryPath();
  path = path.left(path.lastIndexOf("/")+1);

  // Standard crates

  QDir dir = QDir(path + GuiStrings::getInstance()->seratoSubcratesDirName());
  QStringList filters;
  filters << "*.crate";
  QFileInfoList contents = dir.entryInfoList(filters);

  for (int i = 0; i<(signed)contents.size(); i++) {

    ExternalPlaylist playlist(contents[i].baseName(), EXTERNAL_PLAYLIST_SOURCE_SERATO);
    playlist.name = playlist.name.replace(QString("%%"),QString("/"));
    playlist.tracks = readSeratoLibraryPlaylist(playlist.name, prefs);
    results.push_back(playlist);
  }

  // Smart crates

  dir = QDir(path + GuiStrings::getInstance()->seratoSmartCratesDirName());
  filters.clear();
  filters << "*.scrate";
  contents = dir.entryInfoList(filters);

  for (int i = 0; i<(signed)contents.size(); i++) {

    ExternalPlaylist playlist(contents[i].baseName(), EXTERNAL_PLAYLIST_SOURCE_SERATO);
    playlist.name = playlist.name.replace(QString("%%"),QString("/"));
    playlist.tracks = readSeratoLibraryPlaylist(playlist.name, prefs);
    results.push_back(playlist);
  }

  qDebug(READ_LIBRARY_END, SERATO, results.size());
  return results;
}

QList<QUrl> ExternalPlaylistProvider::readSeratoLibraryPlaylist(const QString& playlistName, const Preferences& prefs) {

  QList<QUrl> results;

  qDebug(READ_PLAYLIST_BEGIN, SERATO, playlistName.toUtf8().constData());

  QString path = prefs.getSeratoLibraryPath();
  path = path.left(path.lastIndexOf("/")+1);

  // is it a [sub]crate or a smartcrate?
  QString playlistNameCopy = playlistName;
  playlistNameCopy = playlistNameCopy.replace(QString("/"), QString("%%"));

  QFile* crate = new QFile(path + GuiStrings::getInstance()->seratoSubcratesDirName() + QString("/") + playlistNameCopy + QString(".crate"));
  bool sub = true;

  if (!crate->exists()) {

    delete crate;
    crate = new QFile(path + GuiStrings::getInstance()->seratoSmartCratesDirName() + QString("/") + playlistNameCopy + QString(".scrate"));
    sub = false;
    qDebug("Serato playlist: '%s' is a smartcrate", playlistName.toUtf8().constData());

  } else {

    qDebug("Serato playlist: '%s' is a subcrate", playlistName.toUtf8().constData());
  }

  // Serato path stuff
  QString pathPrefix = "/";
#ifdef Q_OS_MAC
  QRegularExpression regex("^(/Volumes/[^/]+/)");
  QRegularExpressionMatch match = regex.match(prefs.getSeratoLibraryPath());
  if (match.hasMatch()) pathPrefix = match.captured();
#endif
#ifdef Q_OS_WIN
  pathPrefix = prefs.getSeratoLibraryPath().left(3);
#endif
  if (crate->open(QIODevice::ReadOnly)) {
    SeratoDataStream instr;
    QStringList resultStrings = instr.readCrate(crate, (sub ? SeratoDataStream::SUBCRATE : SeratoDataStream::SMARTCRATE));
    for (int i = 0; i < (signed)resultStrings.size(); i++) {
      results.push_back(QUrl::fromLocalFile(pathPrefix + resultStrings[i]));
    }
    crate->close();
  }
  delete crate;
  qDebug(READ_PLAYLIST_END, SERATO, results.size());
  return results;
}

QList<QUrl> ExternalPlaylistProvider::readITunesStandalonePlaylist(const QString& playlistPath) {
  QMutexLocker locker(&externalPlaylistMutex);
  QFile xmlFile(playlistPath);
  QList<ExternalPlaylist> results;

  if (!xmlFile.open(QIODevice::ReadOnly)) {

    qDebug(READ_STANDALONE_FAIL, ITUNES, playlistPath.toUtf8().constData());

  } else {

    qDebug(READ_STANDALONE_BEGIN, ITUNES, playlistPath.toUtf8().constData());
    results = readPlaylistsFromITunesFile(xmlFile);
    qDebug(READ_STANDALONE_END, ITUNES, results.size());
  }

  return results.first().tracks;
}

QList<QUrl> ExternalPlaylistProvider::readM3uStandalonePlaylist(const QString& m3uPath) {
  QList<QUrl> results;


  QFile m3uFile(m3uPath);

  if (!m3uFile.open(QIODevice::ReadOnly)) {
    qDebug(READ_STANDALONE_FAIL, M3U, m3uPath.toUtf8().constData());
    return results;
  }

  qDebug(READ_STANDALONE_BEGIN, M3U, m3uPath.toUtf8().constData());

  QTextStream m3uTextStream(&m3uFile);
  QString m3uChar;
  QString m3uLine;
  // M3U files break with ch10/13, and comment with ch35.
  // QTextStream.readLine doesn't work, so we do it a char at a time
  while (!(m3uChar = m3uTextStream.read(1)).isNull()) {
    int chVal = int(m3uChar[0].toLatin1());
    if (chVal == 13 || chVal == 10) {
      if (m3uLine.length() > 0 && int(m3uLine[0].toLatin1()) != 35) {
        results.push_back(QUrl(m3uLine));
      }
      m3uLine = "";
    } else {
      m3uLine += m3uChar;
    }
  }

  qDebug(READ_STANDALONE_END, M3U, results.size());
  return results;
}

QUrl ExternalPlaylistProvider::fixITunesAddressing(const QString& address) {
  QString addressCopy = address;
  addressCopy = addressCopy.replace(QString("file://localhost"), QString(""));
  addressCopy = addressCopy.replace(QString("file://"), QString(""));
  addressCopy = QUrl::fromPercentEncoding(addressCopy.toUtf8().constData());
  qDebug("Fixed iTunes address from %s to %s", address.toUtf8().constData(), addressCopy.toUtf8().constData());
  return QUrl::fromLocalFile(addressCopy);
}

QUrl ExternalPlaylistProvider::fixTraktorAddressing(const QString& address) {
  QString addressCopy = address;
  addressCopy = addressCopy.replace(QString("/:"), QString("/"));
  addressCopy = addressCopy.prepend("/Volumes/");
  qDebug("Fixed Traktor address from %s to %s", address.toUtf8().constData(), addressCopy.toUtf8().constData());
  return QUrl::fromLocalFile(addressCopy);
}
