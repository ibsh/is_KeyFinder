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

#include "preferences.h"

Preferences::Preferences(SettingsWrapper* s) {
  settings = (s == NULL ? new SettingsWrapperQt() : s);
  load();

  // default key codes
  defaultKeyCodes.push_back("A");   defaultKeyCodes.push_back("Am");
  defaultKeyCodes.push_back("Bb");  defaultKeyCodes.push_back("Bbm");
  defaultKeyCodes.push_back("B");   defaultKeyCodes.push_back("Bm");
  defaultKeyCodes.push_back("C");   defaultKeyCodes.push_back("Cm");
  defaultKeyCodes.push_back("Db");  defaultKeyCodes.push_back("Dbm");
  defaultKeyCodes.push_back("D");   defaultKeyCodes.push_back("Dm");
  defaultKeyCodes.push_back("Eb");  defaultKeyCodes.push_back("Ebm");
  defaultKeyCodes.push_back("E");   defaultKeyCodes.push_back("Em");
  defaultKeyCodes.push_back("F");   defaultKeyCodes.push_back("Fm");
  defaultKeyCodes.push_back("Gb");  defaultKeyCodes.push_back("Gbm");
  defaultKeyCodes.push_back("G");   defaultKeyCodes.push_back("Gm");
  defaultKeyCodes.push_back("Ab");  defaultKeyCodes.push_back("Abm");
  defaultKeyCodes.push_back("...");
  // key colours for display
  int a = 127;
  int b = 191;
  int c = 223;
  int d = 255;
  keyColours.push_back(qRgb(c,d,a)); keyColours.push_back(qRgb(b,c,a));
  keyColours.push_back(qRgb(d,a,d)); keyColours.push_back(qRgb(c,a,c));
  keyColours.push_back(qRgb(a,d,c)); keyColours.push_back(qRgb(a,c,b));
  keyColours.push_back(qRgb(d,a,a)); keyColours.push_back(qRgb(c,a,a));
  keyColours.push_back(qRgb(a,c,d)); keyColours.push_back(qRgb(a,b,c));
  keyColours.push_back(qRgb(d,d,a)); keyColours.push_back(qRgb(c,c,a));
  keyColours.push_back(qRgb(c,a,d)); keyColours.push_back(qRgb(b,a,c));
  keyColours.push_back(qRgb(a,d,a)); keyColours.push_back(qRgb(a,c,a));
  keyColours.push_back(qRgb(d,a,c)); keyColours.push_back(qRgb(c,a,b));
  keyColours.push_back(qRgb(a,d,d)); keyColours.push_back(qRgb(a,c,c));
  keyColours.push_back(qRgb(d,c,a)); keyColours.push_back(qRgb(c,b,a));
  keyColours.push_back(qRgb(a,a,c)); keyColours.push_back(qRgb(a,a,c));
  keyColours.push_back(qRgb(a,a,a));
}

Preferences::Preferences(const Preferences &that) {
  settings = new SettingsWrapperQt();
  copy(that);
}

Preferences::~Preferences() {
  delete settings;
}


void Preferences::copy(const Preferences &that) {
  writeToFilesAutomatically = that.writeToFilesAutomatically;
  parallelBatchJobs         = that.parallelBatchJobs;
  skipFilesWithExistingTags = that.skipFilesWithExistingTags;
  applyFileExtensionFilter  = that.applyFileExtensionFilter;
  metadataWriteTitle        = that.metadataWriteTitle;
  metadataWriteArtist       = that.metadataWriteArtist;
  metadataWriteAlbum        = that.metadataWriteAlbum;
  metadataWriteComment      = that.metadataWriteComment;
  metadataWriteGrouping     = that.metadataWriteGrouping;
  metadataWriteKey          = that.metadataWriteKey;
  metadataWriteFilename     = that.metadataWriteFilename;
  metadataFormat            = that.metadataFormat;
  maxDuration               = that.maxDuration;
  iTunesLibraryPath         = that.iTunesLibraryPath;
  traktorLibraryPath        = that.traktorLibraryPath;
  seratoLibraryPath         = that.seratoLibraryPath;
  metadataDelimiter         = that.metadataDelimiter;
  customKeyCodes            = that.customKeyCodes;
  filterFileExtensions      = that.filterFileExtensions;
  batchWindowState          = that.batchWindowState;
  batchWindowGeometry       = that.batchWindowGeometry;
  batchWindowSplitterState  = that.batchWindowSplitterState;
}

Preferences& Preferences::operator=(const Preferences& that) {
  if (this != &that) {
    copy(that);
  }
  return *this;
}

bool Preferences::equivalentTo(const Preferences& that) const {
  if (writeToFilesAutomatically != that.writeToFilesAutomatically) return false;
  if (parallelBatchJobs         != that.parallelBatchJobs)         return false;
  if (skipFilesWithExistingTags != that.skipFilesWithExistingTags) return false;
  if (applyFileExtensionFilter  != that.applyFileExtensionFilter)  return false;
  if (metadataWriteTitle        != that.metadataWriteTitle)        return false;
  if (metadataWriteArtist       != that.metadataWriteArtist)       return false;
  if (metadataWriteAlbum        != that.metadataWriteAlbum)        return false;
  if (metadataWriteComment      != that.metadataWriteComment)      return false;
  if (metadataWriteGrouping     != that.metadataWriteGrouping)     return false;
  if (metadataWriteKey          != that.metadataWriteKey)          return false;
  if (metadataWriteFilename     != that.metadataWriteFilename)     return false;
  if (metadataFormat            != that.metadataFormat)            return false;
  if (maxDuration               != that.maxDuration)               return false;
  if (iTunesLibraryPath         != that.iTunesLibraryPath)         return false;
  if (traktorLibraryPath        != that.traktorLibraryPath)        return false;
  if (seratoLibraryPath         != that.seratoLibraryPath)         return false;
  if (metadataDelimiter         != that.metadataDelimiter)         return false;
  if (customKeyCodes            != that.customKeyCodes)            return false;
  if (filterFileExtensions      != that.filterFileExtensions)      return false;
  if (batchWindowState          != that.batchWindowState)          return false;
  if (batchWindowGeometry       != that.batchWindowGeometry)       return false;
  if (batchWindowSplitterState  != that.batchWindowSplitterState)  return false;
  return true;
}

void Preferences::load() {

  // ========================== Custom Key Codes ===============================

  settings->beginGroup("customKeyCodes");
  customKeyCodes = QStringList();
  customKeyCodes.push_back(settings->value("A",    "").toString());
  customKeyCodes.push_back(settings->value("Am",   "").toString());
  customKeyCodes.push_back(settings->value("Bb",   "").toString());
  customKeyCodes.push_back(settings->value("Bbm",  "").toString());
  customKeyCodes.push_back(settings->value("B",    "").toString());
  customKeyCodes.push_back(settings->value("Bm",   "").toString());
  customKeyCodes.push_back(settings->value("C",    "").toString());
  customKeyCodes.push_back(settings->value("Cm",   "").toString());
  customKeyCodes.push_back(settings->value("Db",   "").toString());
  customKeyCodes.push_back(settings->value("Dbm",  "").toString());
  customKeyCodes.push_back(settings->value("D",    "").toString());
  customKeyCodes.push_back(settings->value("Dm",   "").toString());
  customKeyCodes.push_back(settings->value("Eb",   "").toString());
  customKeyCodes.push_back(settings->value("Ebm",  "").toString());
  customKeyCodes.push_back(settings->value("E",    "").toString());
  customKeyCodes.push_back(settings->value("Em",   "").toString());
  customKeyCodes.push_back(settings->value("F",    "").toString());
  customKeyCodes.push_back(settings->value("Fm",   "").toString());
  customKeyCodes.push_back(settings->value("Gb",   "").toString());
  customKeyCodes.push_back(settings->value("Gbm",  "").toString());
  customKeyCodes.push_back(settings->value("G",    "").toString());
  customKeyCodes.push_back(settings->value("Gm",   "").toString());
  customKeyCodes.push_back(settings->value("Ab",   "").toString());
  customKeyCodes.push_back(settings->value("Abm",  "").toString());
  customKeyCodes.push_back(settings->value("SLNC", "").toString());
  settings->endGroup();

  // ============================ Metadata =================================

  settings->beginGroup("tags");
  metadataFormat = (metadata_format_t)settings->value("metadataFormat", METADATA_FORMAT_KEYS).toInt();
  metadataWriteTitle = (metadata_write_t)settings->value("metadataWriteTitle", METADATA_WRITE_NONE).toInt();
  metadataWriteArtist = (metadata_write_t)settings->value("metadataWriteArtist", METADATA_WRITE_NONE).toInt();
  metadataWriteAlbum = (metadata_write_t)settings->value("metadataWriteAlbum", METADATA_WRITE_NONE).toInt();
  metadataWriteComment = (metadata_write_t)settings->value("metadataWriteComment", METADATA_WRITE_PREPEND).toInt();
  metadataWriteGrouping = (metadata_write_t)settings->value("metadataWriteGrouping", METADATA_WRITE_NONE).toInt();
  metadataWriteKey = (metadata_write_t)settings->value("metadataWriteKey", METADATA_WRITE_NONE).toInt();
  metadataWriteFilename = (metadata_write_t)settings->value("metadataWriteFilename", METADATA_WRITE_NONE).toInt();
  metadataDelimiter = settings->value("metadataDelimiter", " - ").toString();
  writeToFilesAutomatically = settings->value("writeToFilesAutomatically", false).toBool();
  settings->endGroup();

  // =========================== Batch jobs ================================

  settings->beginGroup("batch");
  parallelBatchJobs = settings->value("parallelBatchJobs", true).toBool();
  skipFilesWithExistingTags = settings->value("skipFilesWithExistingTags", false).toBool();
  applyFileExtensionFilter = settings->value("applyFileExtensionFilter", false).toBool();
  maxDuration = settings->value("maxDuration", 60).toInt();
  QStringList defaultFilterFileExtensions;
  defaultFilterFileExtensions << "mp3" << "m4a" << "mp4" << "wma";
  defaultFilterFileExtensions << "flac" << "aif" << "aiff" << "wav";
  filterFileExtensions = (settings->value("filterFileExtensions", defaultFilterFileExtensions).toStringList());
  settings->endGroup();

  // ============================= Library ==================================

#ifdef Q_OS_WIN
  QString iTunesLibraryPathDefault = QDir::homePath() + "/My Music/iTunes/iTunes Music Library.xml";
  QString traktorLibraryPathDefault = QDir::homePath() + "/My Documents/Native Instruments/Traktor 2.7.1/collection.nml";
  QString seratoLibraryPathDefault = QDir::homePath() + "/My Music/_Serato_/database V2";
#else
  QString iTunesLibraryPathDefault = QDir::homePath() + "/Music/iTunes/iTunes Music Library.xml";
  QString traktorLibraryPathDefault = QDir::homePath() + "/Documents/Native Instruments/Traktor 2.7.1/collection.nml";
  QString seratoLibraryPathDefault = QDir::homePath() + "/Music/_Serato_/database V2";
#endif

  settings->beginGroup("library");
  iTunesLibraryPath = settings->value("iTunesLibraryPath", iTunesLibraryPathDefault).toString();
  traktorLibraryPath = settings->value("traktorLibraryPath", traktorLibraryPathDefault).toString();
  seratoLibraryPath = settings->value("seratoLibraryPath", seratoLibraryPathDefault).toString();
  settings->endGroup();

  // ============================= UIState ==================================

  settings->beginGroup("uistate");
  batchWindowState = settings->value("batchWindowState", QByteArray()).toByteArray();
  batchWindowGeometry = settings->value("batchWindowGeometry", QByteArray()).toByteArray();
  batchWindowSplitterState = settings->value("batchWindowSplitterState", QByteArray()).toByteArray();
  settings->endGroup();

}

void Preferences::save() {

  settings->beginGroup("customKeyCodes");
  settings->setValue("A",    customKeyCodes[0]);   settings->setValue("Am",  customKeyCodes[1]);
  settings->setValue("Bb",   customKeyCodes[2]);   settings->setValue("Bbm", customKeyCodes[3]);
  settings->setValue("B",    customKeyCodes[4]);   settings->setValue("Bm",  customKeyCodes[5]);
  settings->setValue("C",    customKeyCodes[6]);   settings->setValue("Cm",  customKeyCodes[7]);
  settings->setValue("Db",   customKeyCodes[8]);   settings->setValue("Dbm", customKeyCodes[9]);
  settings->setValue("D",    customKeyCodes[10]);  settings->setValue("Dm",  customKeyCodes[11]);
  settings->setValue("Eb",   customKeyCodes[12]);  settings->setValue("Ebm", customKeyCodes[13]);
  settings->setValue("E",    customKeyCodes[14]);  settings->setValue("Em",  customKeyCodes[15]);
  settings->setValue("F",    customKeyCodes[16]);  settings->setValue("Fm",  customKeyCodes[17]);
  settings->setValue("Gb",   customKeyCodes[18]);  settings->setValue("Gbm", customKeyCodes[19]);
  settings->setValue("G",    customKeyCodes[20]);  settings->setValue("Gm",  customKeyCodes[21]);
  settings->setValue("Ab",   customKeyCodes[22]);  settings->setValue("Abm", customKeyCodes[23]);
  settings->setValue("SLNC", customKeyCodes[24]);
  settings->endGroup();

  settings->beginGroup("tags");
  settings->setValue("metadataFormat", metadataFormat);
  settings->setValue("metadataWriteTitle", metadataWriteTitle);
  settings->setValue("metadataWriteArtist", metadataWriteArtist);
  settings->setValue("metadataWriteAlbum", metadataWriteAlbum);
  settings->setValue("metadataWriteComment", metadataWriteComment);
  settings->setValue("metadataWriteGrouping", metadataWriteGrouping);
  settings->setValue("metadataWriteKey", metadataWriteKey);
  settings->setValue("metadataWriteFilename", metadataWriteFilename);
  settings->setValue("metadataDelimiter", metadataDelimiter);
  settings->setValue("writeToFilesAutomatically", writeToFilesAutomatically);
  settings->endGroup();

  settings->beginGroup("batch");
  settings->setValue("parallelBatchJobs", parallelBatchJobs);
  settings->setValue("skipFilesWithExistingTags", skipFilesWithExistingTags);
  settings->setValue("applyFileExtensionFilter", applyFileExtensionFilter);
  settings->setValue("maxDuration", maxDuration);
  settings->setValue("filterFileExtensions", filterFileExtensions);
  settings->endGroup();

  settings->beginGroup("library");
  settings->setValue("iTunesLibraryPath", iTunesLibraryPath);
  settings->setValue("traktorLibraryPath", traktorLibraryPath);
  settings->setValue("seratoLibraryPath", seratoLibraryPath);
  settings->endGroup();

  settings->beginGroup("uistate");
  settings->setValue("batchWindowState", batchWindowState);
  settings->setValue("batchWindowGeometry", batchWindowGeometry);
  settings->setValue("batchWindowSplitterState", batchWindowSplitterState);
  settings->endGroup();

}

bool              Preferences::getWriteToFilesAutomatically() const { return writeToFilesAutomatically; }
bool              Preferences::getParallelBatchJobs()         const { return parallelBatchJobs; }
bool              Preferences::getApplyFileExtensionFilter()  const { return applyFileExtensionFilter; }
metadata_write_t  Preferences::getMetadataWriteTitle()        const { return metadataWriteTitle; }
metadata_write_t  Preferences::getMetadataWriteArtist()       const { return metadataWriteArtist; }
metadata_write_t  Preferences::getMetadataWriteAlbum()        const { return metadataWriteAlbum; }
metadata_write_t  Preferences::getMetadataWriteComment()      const { return metadataWriteComment; }
metadata_write_t  Preferences::getMetadataWriteGrouping()     const { return metadataWriteGrouping; }
metadata_write_t  Preferences::getMetadataWriteKey()          const { return metadataWriteKey; }
metadata_write_t  Preferences::getMetadataWriteFilename()     const { return metadataWriteFilename; }
metadata_format_t Preferences::getMetadataFormat()            const { return metadataFormat; }
bool              Preferences::getSkipFilesWithExistingTags() const { return skipFilesWithExistingTags; }
int               Preferences::getMaxDuration()               const { return maxDuration; }
QString           Preferences::getITunesLibraryPath()         const { return iTunesLibraryPath; }
QString           Preferences::getTraktorLibraryPath()        const { return traktorLibraryPath; }
QString           Preferences::getSeratoLibraryPath()         const { return seratoLibraryPath; }
QString           Preferences::getMetadataDelimiter()         const { return metadataDelimiter; }
QStringList       Preferences::getCustomKeyCodes()            const { return customKeyCodes; }
QStringList       Preferences::getFilterFileExtensions()      const { return filterFileExtensions; }
QByteArray        Preferences::getBatchWindowState()          const { return batchWindowState; }
QByteArray        Preferences::getBatchWindowGeometry()       const { return batchWindowGeometry; }
QByteArray        Preferences::getBatchWindowSplitterState()  const { return batchWindowSplitterState; }

void Preferences::setWriteToFilesAutomatically(bool autoWrite)     { writeToFilesAutomatically = autoWrite; }
void Preferences::setApplyFileExtensionFilter(bool apply)          { applyFileExtensionFilter = apply; }
void Preferences::setMetadataWriteTitle(metadata_write_t tit)      { metadataWriteTitle = tit; }
void Preferences::setMetadataWriteArtist(metadata_write_t art)     { metadataWriteArtist = art; }
void Preferences::setMetadataWriteAlbum(metadata_write_t alb)      { metadataWriteAlbum = alb; }
void Preferences::setMetadataWriteComment(metadata_write_t cmt)    { metadataWriteComment = cmt; }
void Preferences::setMetadataWriteGrouping(metadata_write_t grp)   { metadataWriteGrouping = grp; }
void Preferences::setMetadataWriteKey(metadata_write_t key)        { metadataWriteKey = key; }
void Preferences::setMetadataWriteFilename(metadata_write_t fn)    { metadataWriteFilename = fn; }
void Preferences::setSkipFilesWithExistingTags(bool skip)          { skipFilesWithExistingTags = skip; }
void Preferences::setMaxDuration(int max)                          { maxDuration = max; }
void Preferences::setMetadataFormat(metadata_format_t fmt)         { metadataFormat = fmt; }
void Preferences::setITunesLibraryPath(const QString& path)        { iTunesLibraryPath = path; }
void Preferences::setTraktorLibraryPath(const QString& path)       { traktorLibraryPath = path; }
void Preferences::setSeratoLibraryPath(const QString& path)        { seratoLibraryPath = path; }
void Preferences::setMetadataDelimiter(const QString & delim)      { metadataDelimiter = delim; }
void Preferences::setCustomKeyCodes(const QStringList& codes)      { customKeyCodes = codes; }
void Preferences::setFilterFileExtensions(const QStringList& exts) { filterFileExtensions = exts; }
void Preferences::setBatchWindowState(const QByteArray& a)         { batchWindowState = a; }
void Preferences::setBatchWindowGeometry(const QByteArray& a)      { batchWindowGeometry = a; }
void Preferences::setBatchWindowSplitterState(const QByteArray& a) { batchWindowSplitterState = a; }

metadata_write_t Preferences::getMetadataWriteByTagEnum(metadata_tag_t tag) const {
  switch (tag) {
    case METADATA_TAG_TITLE:
      return getMetadataWriteTitle();
    case METADATA_TAG_ARTIST:
      return getMetadataWriteArtist();
    case METADATA_TAG_ALBUM:
      return getMetadataWriteAlbum();
    case METADATA_TAG_COMMENT:
      return getMetadataWriteComment();
    case METADATA_TAG_GROUPING:
      return getMetadataWriteGrouping();
    case METADATA_TAG_KEY:
      return getMetadataWriteKey();
  }
  return METADATA_WRITE_NONE;
}

void Preferences::setMetadataWriteByTagEnum(metadata_tag_t tag, metadata_write_t val) {
  switch (tag) {
    case METADATA_TAG_TITLE:
      setMetadataWriteTitle(val);
      break;
    case METADATA_TAG_ARTIST:
      setMetadataWriteArtist(val);
      break;
    case METADATA_TAG_ALBUM:
      setMetadataWriteAlbum(val);
      break;
    case METADATA_TAG_COMMENT:
      setMetadataWriteComment(val);
      break;
    case METADATA_TAG_GROUPING:
      setMetadataWriteGrouping(val);
      break;
    case METADATA_TAG_KEY:
      setMetadataWriteKey(val);
      break;
  }
}

void Preferences::setParallelBatchJobs(bool parallel) {
  parallelBatchJobs = parallel;
  int numThreads = QThread::idealThreadCount();
  if (numThreads == -1 || parallel == false) {
    QThreadPool::globalInstance()->setMaxThreadCount(1);
  } else {
    QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
  }
}

QString Preferences::getKeyCode(KeyFinder::key_t k) const {
  if (k < 0 || k >= defaultKeyCodes.size()) {
    qDebug("Attempt to get name of out-of-bounds key (%d/%d)",k,(int)defaultKeyCodes.size());
    return "";
  }
  QString defaultCode = defaultKeyCodes[k];
  QString customCode = customKeyCodes[k];
  if (metadataFormat == METADATA_FORMAT_KEYS || customCode == "") {
    return defaultCode;
  } else if (metadataFormat == METADATA_FORMAT_CUSTOM) {
    return customCode;
  } else {
    return customCode + " " + defaultCode;
  }
}

QStringList Preferences::getKeyCodeList() const {
  QStringList output;
  for (int i = 0; i < defaultKeyCodes.size(); i++) {
    output << getKeyCode((KeyFinder::key_t)i);
  }
  return output;
}

QColor Preferences::getKeyColour(KeyFinder::key_t k) const {
  if (k >= keyColours.size()) {
    qDebug("Attempt to get colour of out-of-bounds key (%d/%d)",k,(int)keyColours.size());
    return qRgb(0,0,0);
  }
  return keyColours[k];
}

void Preferences::setImageColours(QImage& image, chromagram_colour_t scheme) const {
  if (scheme == CHROMA_COLOUR_IZO) {
    for (int i=0; i<65; i++) {
      image.setColor(i,
                     qRgb(
                       scaledSine(i, 0.055, +4.50, 150, 130),
                       scaledSine(i, 0.043, -1.80, 150, 150),
                       scaledSine(i, 0.022, +4.70, 256, 270)
                       )
                     );
    }
  } else if (scheme == CHROMA_COLOUR_MONO) {
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(i*4,i*4,i*4));
    }
    image.setColor(64,qRgb(255,255,255));
  } else if (scheme == CHROMA_COLOUR_PRED) {
    for (int i=0; i<65; i++) {
      image.setColor(i,
                     qRgb(
                       scaledSine(i, 0.077, -2.30, 320, 0),
                       scaledSine(i, 0.077, -1.05, 320, 0),
                       scaledSine(i, 0.077, +0.20, 320, 0)
                       )
                     );
    }
  } else if (scheme == CHROMA_COLOUR_VBOY) {
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(i*4,0,0));
    }
    image.setColor(64,qRgb(255,0,0));
  } else {
    // Hack by default
    for (int i=0; i<64; i++) {
      image.setColor(i,qRgb(0,i*4,0));
    }
    image.setColor(64,qRgb(0,255,0));
  }
}

int Preferences::scaledSine(int index, double xScale, double xOffset, int yScale, int yOffset) const {
  int result = yScale * sin((index * xScale) + xOffset) + yOffset;
  if (result <   0) return 0;
  if (result > 255) return 255;
  return result;
}

// Used to determine whether metadata writing is necessary, and what to write
// when concatenating / overwriting cases. Returns an empty string if no write
// necessary
QString Preferences::newString(
    const QString &newData,
    const QString &currentData,
    unsigned int charLimit,
    metadata_write_t write
    ) const {
  QString empty;
  // validate
  if (write == METADATA_WRITE_NONE) {
    return empty;
  }
  // if newData is an empty string, check against all possible key codes
  QStringList dataToCheck;
  if (newData.isEmpty()) {
    dataToCheck = getKeyCodeList();
  }
  else dataToCheck.push_back(newData);
  // check
  QStringList::iterator iter;
  for (iter = dataToCheck.begin(); iter != dataToCheck.end(); iter++) {
    if (charLimit) {
      *iter = (*iter).left(charLimit);
    }
    if (write == METADATA_WRITE_OVERWRITE && currentData == *iter) {
      return empty;
    } else if (write == METADATA_WRITE_PREPEND && currentData.left((*iter).length()) == *iter && stringIsNotAlphaNumeric(currentData.mid((*iter).length(), 1))) {
      return empty;
    } else if (write == METADATA_WRITE_APPEND && currentData.right((*iter).length()) == *iter && stringIsNotAlphaNumeric(currentData.mid(currentData.length() - (*iter).length() - 1, 1))) {
      return empty;
    }
  }
  // check failed; determine what to write
  if (write == METADATA_WRITE_OVERWRITE) {
    return dataToCheck.first();
  }
  if (write == METADATA_WRITE_PREPEND) {
    if (currentData == empty) {
      return dataToCheck.first();
    }
    return dataToCheck.first() + getMetadataDelimiter() + currentData;
  }
  if (write == METADATA_WRITE_APPEND) {
    if (currentData == empty) {
      return dataToCheck.first();
    }
    return currentData + getMetadataDelimiter() + dataToCheck.first();
  }
  // shouldn't get here
  return empty;
}

bool Preferences::stringIsNotAlphaNumeric(const QString &string) const {
  QRegExp regex("^[^a-zA-Z0-9]*$");
  return regex.exactMatch(string);
}
