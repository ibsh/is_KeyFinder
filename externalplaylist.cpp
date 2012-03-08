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

#include "externalplaylist.h"

QMutex externalPlaylistMutex;

ExternalPlaylistObject::ExternalPlaylistObject(const QString& n, const QString& s){
  name = n;
  source = s;
}

QList<ExternalPlaylistObject> ExternalPlaylist::readLibrary(const Preferences& prefs){
  QList<ExternalPlaylistObject> playlists;
  playlists << readPlaylistsFromITunesLibrary(prefs);
  playlists << readPlaylistsFromTraktorLibrary(prefs);
  playlists << readPlaylistsFromSeratoLibrary(prefs);
  return playlists;
}

QList<ExternalPlaylistObject> ExternalPlaylist::readPlaylistsFromITunesLibrary(const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QList<ExternalPlaylistObject> results;

  QStringList defaultPlaylists;
  defaultPlaylists << "Library";
  defaultPlaylists << "Music";
  defaultPlaylists << "Movies";
  defaultPlaylists << "Films";
  defaultPlaylists << "TV Shows";
  defaultPlaylists << "TV Programmes";
  defaultPlaylists << "Podcasts";
  defaultPlaylists << "Books";
  defaultPlaylists << "Purchased";
  defaultPlaylists << "Genius";
  defaultPlaylists << "iTunes DJ";

#ifdef Q_OS_WIN
  // QXmlQuery on Windows
  QStringList resultStrings;

  QFile xmlFile(prefs.getITunesLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  QString xPath;
  xPath += "doc($inputDocument)/plist/dict";
  xPath += "/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict/string[preceding-sibling::key[1]='Name']/string(text())";

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&resultStrings);
  xmlFile.close();

  for(int i=0; i<(signed)resultStrings.size(); i++){
    if(defaultPlaylists.contains(resultStrings[i]))
      continue;
    ExternalPlaylistObject o(resultStrings[i], SOURCE_ITUNES);
    results.push_back(o);
  }
#else
  // XQilla on Mac
  std::string xPath;
  xPath += "plist/dict/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict/string[preceding-sibling::key[1]='Name']/string(text())";

  XQilla xqilla;
  AutoDelete<XQQuery> xQuery(xqilla.parse(X(xPath.c_str())));

  AutoDelete<DynamicContext> xQueryContext(xQuery->createDynamicContext());
  /*
   * Alright, this is bullshit.
   * Something in the sequence allocation crashes without network access, presumably
   * due to schema validation which I cannot apparently disable
   * So get QXmlQuery working in subthreads asap.
   */
  Sequence seq;
  try{
    seq = xQueryContext->resolveDocument(X(prefs.getITunesLibraryPath().toLocal8Bit().data()));
  }catch(...){
    return results;
  }
  if(!seq.isEmpty() && seq.first()->isNode()) {
    xQueryContext->setContextItem(seq.first());
    xQueryContext->setContextPosition(1);
    xQueryContext->setContextSize(1);
  }

  Result xQueryResults = xQuery->execute(xQueryContext);
  Item::Ptr item;
  while(item = xQueryResults->next(xQueryContext)) {
    QString name = QString::fromUtf8(UTF8(item->asString(xQueryContext)));
    if(defaultPlaylists.contains(name))
      continue;
    ExternalPlaylistObject o(name, SOURCE_ITUNES);
    results.push_back(o);
  }
#endif
  return results;
}

QList<ExternalPlaylistObject> ExternalPlaylist::readPlaylistsFromTraktorLibrary(const Preferences& prefs){
  QStringList defaultPlaylists;
  defaultPlaylists << "_LOOPS";
  defaultPlaylists << "_RECORDINGS";
  defaultPlaylists << "Preparation";

  QMutexLocker locker(&externalPlaylistMutex);
  QStringList resultStrings;
  QList<ExternalPlaylistObject> results;

  QFile xmlFile(prefs.getTraktorLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  QString xPath;
  xPath += "doc($inputDocument)//NODE[@TYPE='PLAYLIST']/@NAME/string(.)";

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&resultStrings);
  xmlFile.close();

  for(int i=0; i<(signed)resultStrings.size(); i++){
    if(defaultPlaylists.contains(resultStrings[i]))
      continue;
    ExternalPlaylistObject o(resultStrings[i], SOURCE_TRAKTOR);
    results.push_back(o);
  }
  return results;
}

QList<ExternalPlaylistObject> ExternalPlaylist::readPlaylistsFromSeratoLibrary(const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QList<ExternalPlaylistObject> results;

  QString path = prefs.getSeratoLibraryPath();
  path = path.left(path.lastIndexOf("/")+1);

  QDir dir = QDir(path + QString("Subcrates"));
  QStringList filters;
  filters << "*.crate";
  QFileInfoList contents = dir.entryInfoList(filters);
  for(int i = 0; i<(signed)contents.size(); i++){
    ExternalPlaylistObject o(contents[i].baseName(), SOURCE_SERATO);
    o.name = o.name.replace(QString("%%"),QString("/"));
    results.push_back(o);
  }

  dir = QDir(path + QString("SmartCrates"));
  filters.clear();
  filters << "*.scrate";
  contents = dir.entryInfoList(filters);
  for(int i = 0; i<(signed)contents.size(); i++){
    ExternalPlaylistObject o(contents[i].baseName(), SOURCE_SERATO);
    o.name = o.name.replace(QString("%%"),QString("/"));
    results.push_back(o);
  }

  return results;
}

QList<QUrl> ExternalPlaylist::readLibraryPlaylist(const QString& name, const QString& source, const Preferences& prefs){
  if(source == SOURCE_ITUNES)
    return readITunesLibraryPlaylist(name, prefs);
  else if(source == SOURCE_TRAKTOR)
    return readTraktorLibraryPlaylist(name, prefs);
  else if(source == SOURCE_SERATO)
    return readSeratoLibraryPlaylist(name, prefs);
  else
    return QList<QUrl>();
}

QList<QUrl> ExternalPlaylist::readITunesLibraryPlaylist(const QString& playlistName, const Preferences& prefs){
  // TODO. Obviously using XQilla, in all its slowness, for this, is suboptimal.
  // Find out why recursive QXmlQueries don't like being subthreaded on the Mac.
  QMutexLocker locker(&externalPlaylistMutex);
  QList<QUrl> results;

#ifdef Q_OS_WIN
  // QXmlQuery on Windows
  QStringList resultStrings;

  QFile xmlFile(prefs.getITunesLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);
  xmlQuery.bindVariable("playlistName", QVariant(playlistName));

  QString xPath;
  xPath += "let $d := doc($inputDocument)/plist/dict ";
  xPath += "for $track in $d/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict[child::string[preceding-sibling::key[1]='Name'][1]=($playlistName)]";
  xPath += "/array/dict/integer[preceding-sibling::key[1]='Track ID'] ";
  xPath += "return $d/dict[preceding-sibling::key[1]='Tracks']";
  xPath += "/dict[preceding-sibling::key[1]=$track]";
  xPath += "/string[preceding-sibling::key[1]='Location']/string(text())";

  xmlQuery.setQuery(xPath);
  if(xmlQuery.evaluateTo(&resultStrings)){
    for(int i=0; i<(signed)resultStrings.size(); i++)
      results.push_back(fixITunesAddressing(resultStrings[i]));
  }

  xmlFile.close();
#else
  // XQilla on Mac
  std::string xPath;
  xPath += "let $d := plist/dict ";
  xPath += "for $track in $d/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict[child::string[preceding-sibling::key[1]='Name'][1]='";
  xPath += playlistName.toLocal8Bit().data();
  xPath += "']/array/dict/integer[preceding-sibling::key[1]='Track ID'] ";
  xPath += "return $d/dict[preceding-sibling::key[1]='Tracks']";
  xPath += "/dict[preceding-sibling::key[1]=$track]";
  xPath += "/string[preceding-sibling::key[1]='Location']/string(text())";

  XQilla xqilla;
  AutoDelete<XQQuery> xQuery(xqilla.parse(X(xPath.c_str())));

  AutoDelete<DynamicContext> xQueryContext(xQuery->createDynamicContext());
  Sequence seq;
  try{
    seq = xQueryContext->resolveDocument(X(prefs.getITunesLibraryPath().toLocal8Bit().data()));
  }catch(...){
    return results;
  }
  if(!seq.isEmpty() && seq.first()->isNode()) {
    xQueryContext->setContextItem(seq.first());
    xQueryContext->setContextPosition(1);
    xQueryContext->setContextSize(1);
  }

  Result xQueryResults = xQuery->execute(xQueryContext);
  Item::Ptr item;
  while(item = xQueryResults->next(xQueryContext)) {
    results.push_back(fixITunesAddressing(QString::fromUtf8(UTF8(item->asString(xQueryContext)))));
  }
#endif

  return results;
}

QList<QUrl> ExternalPlaylist::readTraktorLibraryPlaylist(const QString& playlistName, const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QStringList resultStrings;
  QList<QUrl> results;

  QFile xmlFile(prefs.getTraktorLibraryPath());
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);
  xmlQuery.bindVariable("playlistName", QVariant(playlistName));

  QString xPath;
  xPath += "doc($inputDocument)//NODE[@TYPE='PLAYLIST' and @NAME=($playlistName)]";
  xPath += "/PLAYLIST[@TYPE='LIST']/ENTRY/PRIMARYKEY/@KEY/string(.)";

  xmlQuery.setQuery(xPath);
  if(xmlQuery.evaluateTo(&resultStrings))
    for(int i=0; i<(signed)resultStrings.size(); i++)
      results.push_back(fixTraktorAddressing(resultStrings[i]));

  xmlFile.close();
  return results;
}

QList<QUrl> ExternalPlaylist::readSeratoLibraryPlaylist(const QString& playlistName, const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QList<QUrl> results;
  QString path = prefs.getSeratoLibraryPath();
  path = path.left(path.lastIndexOf("/")+1);
  // is it a [sub]crate or a smartcrate?
  QString playlistNameCopy = playlistName;
  playlistNameCopy = playlistNameCopy.replace(QString("/"),QString("%%"));
  QFile* crate = new QFile(path + QString("Subcrates/") + playlistNameCopy + QString(".crate"));
  bool sub = true;
  if(!crate->exists()){
    delete crate;
    crate = new QFile(path + QString("SmartCrates/") + playlistNameCopy + QString(".scrate"));
    sub = false;
  }
  // Serato path stuff
  QString pathPrefix = "/";
  #ifdef Q_OS_WIN
    pathPrefix = prefs.getSeratoLibraryPath().left(3);
  #endif
  if(crate->open(QIODevice::ReadOnly)){
    SeratoDataStream instr;
    QStringList resultStrings = instr.readCrate(crate, (sub ? SeratoDataStream::SUBCRATE : SeratoDataStream::SMARTCRATE));
    for(int i = 0; i < (signed)resultStrings.size(); i++){
      results.push_back(QUrl::fromLocalFile(pathPrefix + resultStrings[i]));
    }
    crate->close();
  }
  delete crate;
  return results;
}

QList<QUrl> ExternalPlaylist::readITunesStandalonePlaylist(const QString& playlistPath){
  QMutexLocker locker(&externalPlaylistMutex);
  QStringList resultStrings;
  QList<QUrl> results;

  QFile xmlFile(playlistPath);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  QString xPath;
  xPath += "doc($inputDocument)/plist/dict";
  xPath += "/dict[preceding-sibling::key[1]='Tracks']";
  xPath += "/dict/string[preceding-sibling::key[1]='Location']/string(text())";

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&resultStrings);
  xmlFile.close();

  for(int i=0; i<(signed)resultStrings.size(); i++)
    results.push_back(fixITunesAddressing(resultStrings[i]));
  return results;
}

QList<QUrl> ExternalPlaylist::readM3uStandalonePlaylist(const QString& m3uPath){
  QList<QUrl> results;
  QFile m3uFile(m3uPath);
  if(!m3uFile.open(QIODevice::ReadOnly))
    return results;
  QTextStream m3uTextStream(&m3uFile);
  QString m3uChar;
  QString m3uLine;
  // M3U files break with ch10/13, and comment with ch35.
  // QTextStream.readLine doesn't work, so we do it a char at a time
  while(!(m3uChar = m3uTextStream.read(1)).isNull()){
    int chVal = int(m3uChar[0].toAscii());
    if(chVal == 13 || chVal == 10){
      if(m3uLine.length() > 0 && int(m3uLine[0].toAscii()) != 35){
        results.push_back(QUrl(m3uLine));
      }
      m3uLine = "";
    }else{
      m3uLine += m3uChar;
    }
  }
  return results;
}

QUrl ExternalPlaylist::fixITunesAddressing(const QString& address){
  QString addressCopy = address;
  addressCopy = addressCopy.replace(QString("file://localhost"), QString(""));
  addressCopy = QUrl::fromPercentEncoding(addressCopy.toLocal8Bit().data());
  return QUrl(QUrl::fromLocalFile(addressCopy));
}

QUrl ExternalPlaylist::fixTraktorAddressing(const QString& address){
  QString addressCopy = address;
  addressCopy = addressCopy.replace(QString("/:"), QString("/"));
  addressCopy = addressCopy.replace(QString("Macintosh HD"), QString(""));
  return QUrl(QUrl::fromLocalFile(addressCopy));
}

// ========================= Handle Serato crate files ===========================

QStringList SeratoDataStream::readCrate(QIODevice* device, CrateType ctype){
  QStringList results;
  // crate type specific
  QString version;
  QString type;
  if(ctype == SeratoDataStream::SUBCRATE){
    version = "81.0";
    type = "/Serato ScratchLive Crate";
  }else{
    version = "D1.0";
    type = "/Serato ScratchLive Smart Crate";
  }
  // open stream and read necessary headers
  strm = new QDataStream(device);
  readSingleByteString(4).toLocal8Bit().data();
  skipBytes(2);
  readDoubleByteString(version.size() * 2);
  readDoubleByteString(type.size() * 2);
  while(!strm->atEnd()){
    QString entryName = readSingleByteString(4);
    int length = readInt(4);
    QString entryData;
    if(entryName.left(4) == "otrk"){ // bizarre lengthening in Windows
      skipBytes(8);
      entryData = readDoubleByteString(length - 8);
      results.push_back(entryData);
    }else{
      entryData = readSingleByteString(length);
    }
  }
  delete strm;
  return results;
}

QString SeratoDataStream::readSingleByteString(int chars){
  char* data = new char[chars];
  strm->readRawData(data, chars);
  QString stringRead(data);
  delete[] data;
  return stringRead;
}

QString SeratoDataStream::readDoubleByteString(int chars){
  QString stringRead;
  for(int i=0; i<(signed)chars; i += 2){
    char* twoBytes = new char[2];
    strm->readRawData(twoBytes, 2);
    stringRead += QChar(twoBytes[1],twoBytes[0]);
    delete[] twoBytes;
  }
  return stringRead;
}

int SeratoDataStream::readInt(int bytes){
  int number = 0;
  for(int i = 0; i < bytes; i++){
    char* data = new char[1];
    strm->readRawData(data, 1);
    number *= 256;
    int n = (int)data[0];
    number += (n >= 0 ? n : n + 256);
    delete[] data;
  }
  return number;
}

void SeratoDataStream::skipBytes(int bytesToSkip){
  strm->skipRawData(bytesToSkip);
}
