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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGlobal>
#include <QDebug>
#include <QThreadPool>
#include <QString>
#include <QImage>
#include <QColor>
#include <QDir>

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>

#include "keyfinder/parameters.h"

#include "settingswrapper.h"
#include "strings.h"

enum metadata_format_t{
  METADATA_FORMAT_KEYS,
  METADATA_FORMAT_CUSTOM,
  METADATA_FORMAT_BOTH
};

enum metadata_write_t{
  METADATA_WRITE_NONE,
  METADATA_WRITE_PREPEND,
  METADATA_WRITE_APPEND,
  METADATA_WRITE_OVERWRITE
};

enum metadata_tag_t{
  METADATA_TAG_TITLE,
  METADATA_TAG_ARTIST,
  METADATA_TAG_ALBUM,
  METADATA_TAG_COMMENT,
  METADATA_TAG_GROUPING,
  METADATA_TAG_KEY
};
const unsigned int METADATA_TAG_T_COUNT = 6;

enum chromagram_colour_t{
  CHROMA_COLOUR_IZO,
  CHROMA_COLOUR_MONO,
  CHROMA_COLOUR_PRED,
  CHROMA_COLOUR_VBOY,
  CHROMA_COLOUR_HACK
};

const unsigned int METADATA_CHARLIMIT_KEY = 3;
const unsigned int METADATA_CHARLIMIT_FILENAME = 256;
const unsigned int METADATA_CHARLIMIT_OTHERS = 50;

class Preferences{
public:
  Preferences(SettingsWrapper* = NULL);
  Preferences(const Preferences& that);
  ~Preferences();
  Preferences& operator=(const Preferences&);
  bool equivalentTo(const Preferences&) const;
  void save();

  KeyFinder::Parameters core;

  // accessors
  bool getWriteToFilesAutomatically() const;
  bool getParallelBatchJobs() const;
  bool getSkipFilesWithExistingTags() const;
  bool getApplyFileExtensionFilter() const;
  metadata_write_t getMetadataWriteByTagEnum(metadata_tag_t) const;
  metadata_write_t getMetadataWriteTitle() const;
  metadata_write_t getMetadataWriteArtist() const;
  metadata_write_t getMetadataWriteAlbum() const;
  metadata_write_t getMetadataWriteComment() const;
  metadata_write_t getMetadataWriteGrouping() const;
  metadata_write_t getMetadataWriteKey() const;
  metadata_write_t getMetadataWriteFilename() const;
  metadata_format_t getMetadataFormat() const;
  int getMaxDuration() const;
  QString getITunesLibraryPath() const;
  QString getTraktorLibraryPath() const;
  QString getSeratoLibraryPath() const;
  QString getMetadataDelimiter() const;
  QStringList getCustomKeyCodes() const;
  QStringList getFilterFileExtensions() const;
  QByteArray getBatchWindowState() const;
  QByteArray getBatchWindowGeometry() const;
  QByteArray getBatchWindowSplitterState() const;

  // mutators
  void setWriteToFilesAutomatically(bool);
  void setParallelBatchJobs(bool);
  void setSkipFilesWithExistingTags(bool);
  void setApplyFileExtensionFilter(bool);
  void setMetadataWriteByTagEnum(metadata_tag_t, metadata_write_t);
  void setMetadataWriteTitle(metadata_write_t);
  void setMetadataWriteArtist(metadata_write_t);
  void setMetadataWriteAlbum(metadata_write_t);
  void setMetadataWriteComment(metadata_write_t);
  void setMetadataWriteGrouping(metadata_write_t);
  void setMetadataWriteKey(metadata_write_t);
  void setMetadataWriteFilename(metadata_write_t);
  void setMetadataFormat(metadata_format_t);
  void setMaxDuration(int);
  void setITunesLibraryPath(const QString&);
  void setTraktorLibraryPath(const QString&);
  void setSeratoLibraryPath(const QString&);
  void setMetadataDelimiter(const QString&);
  void setCustomKeyCodes(const QStringList&);
  void setFilterFileExtensions(const QStringList&);
  void setBatchWindowState(const QByteArray&);
  void setBatchWindowGeometry(const QByteArray&);
  void setBatchWindowSplitterState(const QByteArray&);

  // not necessarily related to user preferences, just here for convenience
  QString getKeyCode(KeyFinder::key_t) const;
  QStringList getKeyCodeList() const;
  QColor getKeyColour(KeyFinder::key_t) const;
  void setImageColours(QImage&, chromagram_colour_t) const;

  QString newString(
    const QString& newData,
    const QString& currentData,
    unsigned int charLimit,
    metadata_write_t write
  ) const;

private:
  SettingsWrapper* settings;
  void load();
  void copy(const Preferences& that);

  bool writeToFilesAutomatically;
  bool parallelBatchJobs;
  bool skipFilesWithExistingTags;
  bool applyFileExtensionFilter;
  metadata_write_t metadataWriteTitle;
  metadata_write_t metadataWriteArtist;
  metadata_write_t metadataWriteAlbum;
  metadata_write_t metadataWriteComment;
  metadata_write_t metadataWriteGrouping;
  metadata_write_t metadataWriteKey;
  metadata_write_t metadataWriteFilename;
  metadata_format_t metadataFormat;
  int maxDuration;
  QString iTunesLibraryPath;
  QString traktorLibraryPath;
  QString seratoLibraryPath;
  QString metadataDelimiter;
  QStringList customKeyCodes;
  QStringList defaultKeyCodes;
  QStringList filterFileExtensions;
  QList<QColor> keyColours;
  QByteArray batchWindowState;
  QByteArray batchWindowGeometry;
  QByteArray batchWindowSplitterState;

  int scaledSine(int, float, float, int, int) const;
};

#endif
