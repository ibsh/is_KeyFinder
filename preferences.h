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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGlobal>
#include <QThreadPool>
#include <QSettings>
#include <QString>
#include <QImage>
#include <QColor>
#include <QDir>

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstring>

#include "libkeyfinder/parameters.h"

enum tag_format_t{
  TAG_FORMAT_KEYS,
  TAG_FORMAT_CUSTOM,
  TAG_FORMAT_BOTH
};

class Preferences{
public:
  KeyFinder::Parameters core;
  Preferences();
  Preferences& operator=(const Preferences&);
  bool equivalentSpectralAnalysis(const Preferences&) const;
  // getters
  bool getWriteTagsAutomatically() const;
  bool getParallelBatchJobs() const;
  bool getWriteToTagComment() const;
  bool getWriteToTagGrouping() const;
  bool getWriteToTagKey() const;
  bool getSkipFilesWithExistingTags() const;
  tag_format_t getTagFormat() const;
  int getDFactor() const;
  QString getITunesLibraryPath() const;
  QString getTraktorLibraryPath() const;
  QString getSeratoLibraryPath() const;
  QStringList getCustomKeyCodes() const;
  QByteArray getBatchWindowState() const;
  QByteArray getBatchWindowGeometry() const;
  QByteArray getBatchWindowSplitterState() const;

  // setters
  void setWriteTagsAutomatically(bool);
  void setParallelBatchJobs(bool);
  void setWriteToTagComment(bool);
  void setWriteToTagGrouping(bool);
  void setWriteToTagKey(bool);
  void setSkipFilesWithExistingTags(bool);
  void setTagFormat(tag_format_t);
  void setDFactor(int);
  void setITunesLibraryPath(const QString&);
  void setTraktorLibraryPath(const QString&);
  void setSeratoLibraryPath(const QString&);
  void setCustomKeyCodes(const QStringList&);
  void setBatchWindowState(const QByteArray&);
  void setBatchWindowGeometry(const QByteArray&);
  void setBatchWindowSplitterState(const QByteArray&);

  void save();

  // not necessarily related to user preferences, just here for convenience
  QString getKeyCode(int) const;
  QColor getKeyColour(int) const;
  void setImageColours(QImage&, int) const;

private:
  bool writeTagsAutomatically;
  bool parallelBatchJobs;
  bool writeToTagComment;
  bool writeToTagGrouping;
  bool writeToTagKey;
  bool skipFilesWithExistingTags;
  tag_format_t tagFormat;
  int dFactor;
  QString iTunesLibraryPath;
  QString traktorLibraryPath;
  QString seratoLibraryPath;
  QStringList customKeyCodes;
  QStringList defaultKeyCodes;
  QList<QColor> keyColours;
  QByteArray batchWindowState;
  QByteArray batchWindowGeometry;
  QByteArray batchWindowSplitterState;
};

#endif
