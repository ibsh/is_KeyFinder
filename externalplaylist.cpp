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
  QStringList resultStrings;

  QStringList defaultPlaylists = GuiStrings::getInstance()->iTunesDefaultPlaylists();

#ifndef Q_OS_MAC
  // QXmlQuery on Windows

  QString xPath;
  xPath += "doc($inputDocument)/plist/dict";
  xPath += "/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict/string[preceding-sibling::key[1]='Name']/string(text())";

  resultStrings = qXmlQueryReadLibrary(prefs.getITunesLibraryPath(), xPath);

#else
  // XQilla on Mac

  QString xPath;
  xPath += "plist/dict/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict/string[preceding-sibling::key[1]='Name']/text()"; // likewise in Windows?

  resultStrings = xQillaReadLibrary(prefs.getITunesLibraryPath(), xPath);

#endif

  for(int i=0; i<(signed)resultStrings.size(); i++){
    if(defaultPlaylists.contains(resultStrings[i]))
      continue;
    ExternalPlaylistObject o(resultStrings[i], SOURCE_ITUNES);
    results.push_back(o);
  }

  return results;
}

QList<ExternalPlaylistObject> ExternalPlaylist::readPlaylistsFromTraktorLibrary(const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QList<ExternalPlaylistObject> results;
  QStringList resultStrings;

  QStringList defaultPlaylists = GuiStrings::getInstance()->traktorDefaultPlaylists();

#ifndef Q_OS_MAC
  // QXmlQuery on Windows

  QString xPath = "doc($inputDocument)//NODE[@TYPE='PLAYLIST']/@NAME/string(.)";
  resultStrings = qXmlQueryReadLibrary(prefs.getTraktorLibraryPath(), xPath);

#else
  // XQilla on Mac

  QString xPath = "//NODE[@TYPE='PLAYLIST']/@NAME";
  resultStrings = xQillaReadLibrary(prefs.getTraktorLibraryPath(), xPath);

#endif

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

  QDir dir = QDir(path + GuiStrings::getInstance()->seratoSubcratesDirName());
  QStringList filters;
  filters << "*.crate";
  QFileInfoList contents = dir.entryInfoList(filters);
  for(int i = 0; i<(signed)contents.size(); i++){
    ExternalPlaylistObject o(contents[i].baseName(), SOURCE_SERATO);
    o.name = o.name.replace(QString("%%"),QString("/"));
    results.push_back(o);
  }

  dir = QDir(path + GuiStrings::getInstance()->seratoSmartCratesDirName());
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
  QMutexLocker locker(&externalPlaylistMutex);
  QList<QUrl> results;
  QStringList resultStrings;

#ifndef Q_OS_MAC
  // QXmlQuery on Windows

  QString xPath;
  xPath += "let $d := doc($inputDocument)/plist/dict ";
  xPath += "for $track in $d/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict[child::string[preceding-sibling::key[1]='Name'][1]=($playlistName)]";
  xPath += "/array/dict/integer[preceding-sibling::key[1]='Track ID'] ";
  xPath += "return $d/dict[preceding-sibling::key[1]='Tracks']";
  xPath += "/dict[preceding-sibling::key[1]=$track]";
  xPath += "/string[preceding-sibling::key[1]='Location']/string(text())";

  resultStrings = qXmlQueryReadLibraryPlaylist(prefs.getITunesLibraryPath(), xPath, playlistName);

#else
  // XQilla on Mac

  QString xPath;
  xPath += "declare variable $playlistName external;";
  xPath += "let $d := plist/dict ";
  xPath += "for $track in $d/array[preceding-sibling::key[1]='Playlists']";
  xPath += "/dict[child::string[preceding-sibling::key[1]='Name'][1]=($playlistName)]";
  xPath += "/array/dict/integer[preceding-sibling::key[1]='Track ID'] ";
  xPath += "return $d/dict[preceding-sibling::key[1]='Tracks']";
  xPath += "/dict[preceding-sibling::key[1]=$track]";
  xPath += "/string[preceding-sibling::key[1]='Location']/string(text())";

  resultStrings = xQillaReadLibraryPlaylist(prefs.getITunesLibraryPath(), xPath, playlistName);

#endif

  for(int i=0; i<(signed)resultStrings.size(); i++)
    results.push_back(fixITunesAddressing(resultStrings[i]));

  return results;
}

QList<QUrl> ExternalPlaylist::readTraktorLibraryPlaylist(const QString& playlistName, const Preferences& prefs){
  QMutexLocker locker(&externalPlaylistMutex);
  QList<QUrl> results;
  QStringList resultStrings;

#ifndef Q_OS_MAC
  // QXmlQuery on Windows

  QString xPath;
  xPath += "doc($inputDocument)//NODE[@TYPE='PLAYLIST' and @NAME=($playlistName)]";
  xPath += "/PLAYLIST[@TYPE='LIST']/ENTRY/PRIMARYKEY/@KEY/string(.)";

  resultStrings = qXmlQueryReadLibraryPlaylist(prefs.getTraktorLibraryPath(), xPath, playlistName);

#else
  // XQilla on Mac

  QString xPath;
  xPath += "declare variable $playlistName external;";
  xPath += "//NODE[@TYPE='PLAYLIST' and @NAME=($playlistName)]";
  xPath += "/PLAYLIST[@TYPE='LIST']/ENTRY/PRIMARYKEY/@KEY/string(.)";

  resultStrings = xQillaReadLibraryPlaylist(prefs.getTraktorLibraryPath(), xPath, playlistName);

#endif

  for(int i=0; i<(signed)resultStrings.size(); i++)
    results.push_back(fixTraktorAddressing(resultStrings[i]));

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
  QFile* crate = new QFile(path + GuiStrings::getInstance()->seratoSubcratesDirName() + QString("/") + playlistNameCopy + QString(".crate"));
  bool sub = true;
  if(!crate->exists()){
    delete crate;
    crate = new QFile(path + GuiStrings::getInstance()->seratoSmartCratesDirName() + QString("/") + playlistNameCopy + QString(".scrate"));
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
  addressCopy = QUrl::fromPercentEncoding(addressCopy.toLocal8Bit().constData());
  return QUrl(QUrl::fromLocalFile(addressCopy));
}

QUrl ExternalPlaylist::fixTraktorAddressing(const QString& address){
  QString addressCopy = address;
  addressCopy = addressCopy.replace(QString("/:"), QString("/"));
  addressCopy = addressCopy.replace(QString("Macintosh HD"), QString(""));
  return QUrl(QUrl::fromLocalFile(addressCopy));
}

// ======================================= XQilla stuff ============================================

#ifndef Q_OS_MAC

QStringList ExternalPlaylist::qXmlQueryReadLibrary(const QString& libraryPath, const QString& xPath){

  QStringList results;

  QFile xmlFile(libraryPath);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&results);

  xmlFile.close();
  return results;

}

QStringList ExternalPlaylist::qXmlQueryReadLibraryPlaylist(const QString& libraryPath, const QString& xPath, const QString & playlistName){

  QStringList results;

  QFile xmlFile(libraryPath);
  if (!xmlFile.open(QIODevice::ReadOnly))
    return results;

  QXmlQuery xmlQuery;
  xmlQuery.bindVariable("inputDocument", &xmlFile);
  xmlQuery.bindVariable("playlistName", QVariant(playlistName));

  xmlQuery.setQuery(xPath);
  xmlQuery.evaluateTo(&results);

  xmlFile.close();
  return results;
}

#else

QStringList ExternalPlaylist::xQillaReadLibrary(const QString& libraryPath, const QString& xPath){

  QStringList results;

  // Initialise Xerces-C and XQilla using XQillaPlatformUtils
  try{
    XQillaPlatformUtils::initialize();
  }catch (const xercesc_3_1::XMLException& eXerces){
    qDebug("Xerces exception message: %s", UTF8(eXerces.getMessage()));
    return results;
  }
  // Get the XQilla DOMImplementation object
  xercesc_3_1::DOMImplementation *xqillaImplementation = xercesc_3_1::DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));
  try{
    // Create a DOMLSParser object
    AutoDelete<xercesc_3_1::DOMLSParser> parser(xqillaImplementation->createLSParser(xercesc_3_1::DOMImplementationLS::MODE_SYNCHRONOUS, 0));
    parser->getDomConfig()->setParameter(xercesc_3_1::XMLUni::fgXercesLoadExternalDTD, false);
    // Parse a DOMDocument
    xercesc_3_1::DOMDocument *document = parser->parseURI(X(libraryPath.toLocal8Bit().constData()));
    if(document == 0) {
      throw XQillaException(99,X("No library file found"));
    }
    // Parse XPath
    AutoRelease<xercesc_3_1::DOMXPathExpression> expression(document->createExpression(X(xPath.toLocal8Bit().constData()), 0));
    // Execute query
    AutoRelease<xercesc_3_1::DOMXPathResult> result(expression->evaluate(document, xercesc_3_1::DOMXPathResult::ITERATOR_RESULT_TYPE, 0));
    // Iterate over the results
    while(result->iterateNext())
      results.push_back(QString::fromUtf8(UTF8(result->getNodeValue()->getTextContent())));

  }catch(XQillaException &e){
    qDebug("XQillaException: %s", UTF8(e.getString()));
  }catch(...){
    qDebug("Unspecified exception with XQilla");
  }

  // Terminate Xerces-C and XQilla using XQillaPlatformUtils
  XQillaPlatformUtils::terminate();
  return results;
}

QStringList ExternalPlaylist::xQillaReadLibraryPlaylist(const QString& libraryPath, const QString& xPath, const QString & playlistName){

  QStringList results;

  // Initialise Xerces-C and XQilla using XQillaPlatformUtils
  try{
    XQillaPlatformUtils::initialize();
  }catch (const xercesc_3_1::XMLException& eXerces){
    qDebug("Xerces exception message: %s", UTF8(eXerces.getMessage()));
    return results;
  }

  // Get the XQilla DOMImplementation object
  xercesc_3_1::DOMImplementation *xqillaImplementation = xercesc_3_1::DOMImplementationRegistry::getDOMImplementation(X("XPath2 3.0"));

  try{
    // Create a DOMLSParser object
    AutoDelete<xercesc_3_1::DOMLSParser> parser(xqillaImplementation->createLSParser(xercesc_3_1::DOMImplementationLS::MODE_SYNCHRONOUS, 0));
    // disable remote DTD resolution
    parser->getDomConfig()->setParameter(xercesc_3_1::XMLUni::fgXercesLoadExternalDTD, false);
    // Parse the XML document
    xercesc_3_1::DOMDocument *document = parser->parseURI(X(libraryPath.toLocal8Bit().constData()));
    if(document == 0) {
      throw XQillaException(99,X("No library file found"));
    }
    // now set up a simple API query and context to evaluate the parsed document
    XQilla xqilla;
    AutoDelete<XQQuery> xQuery(xqilla.parse(X(xPath.toLocal8Bit().constData())));
    AutoDelete<DynamicContext> xQueryContext(xQuery->createDynamicContext());
    // grab document from xerces API
    XercesConfiguration xc;
    Node::Ptr node = xc.createNode(document, xQueryContext.get());
    // prepare context
    if(node && node->isNode()){
      xQueryContext->setContextItem(node);
      xQueryContext->setContextPosition(1);
      xQueryContext->setContextSize(1);
      // bind playlistName variable
      Item::Ptr value = xQueryContext->getItemFactory()->createString(X(playlistName.toLocal8Bit().constData()), xQueryContext.get());
      xQueryContext->setExternalVariable(X("playlistName"), value);
    }
    // execute query
    Result xQueryResults = xQuery->execute(xQueryContext);
    // iterate over results
    Item::Ptr item;
    while(item = xQueryResults->next(xQueryContext)) {
      results.push_back(QString::fromUtf8(UTF8(item->asString(xQueryContext))));
    }
  }catch(XQillaException &e){
    // DOM3 (xerces) API
    qDebug("XQillaException: %s", UTF8(e.getString()));
  }catch(XQException &e){
    // Simple API
    qDebug("XQillaException: %s", UTF8(e.getError()));
  }catch(...){
    qDebug("Unspecified exception with XQilla");
  }
  XQillaPlatformUtils::terminate();

  return results;
}

#endif
