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
#include <QDebug>
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

#include "keyfinder/parameters.h"
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

class Preferences{
public:
  KeyFinder::Parameters core;
  Preferences();
  Preferences& operator=(const Preferences&);
  void save();

  // accessors
  bool getWriteToFilesAutomatically() const;
  bool getParallelBatchJobs() const;
  metadata_write_t getMetadataWriteComment() const;
  metadata_write_t getMetadataWriteGrouping() const;
  metadata_write_t getMetadataWriteKey() const;
  metadata_write_t getMetadataWriteFilename() const;
  metadata_format_t getMetadataFormat() const;
  bool getSkipFilesWithExistingTags() const;
  int getMaxDuration() const;
  int getDFactor() const;
  QString getITunesLibraryPath() const;
  QString getTraktorLibraryPath() const;
  QString getSeratoLibraryPath() const;
  QString getMetadataDelimiter() const;
  QStringList getCustomKeyCodes() const;
  QByteArray getBatchWindowState() const;
  QByteArray getBatchWindowGeometry() const;
  QByteArray getBatchWindowSplitterState() const;

  // accessors tied to KeyFinder::Parameters object
  bool getOffsetToC() const;
  KeyFinder::temporal_window_t getTemporalWindow() const;
  KeyFinder::segmentation_t getSegmentation() const;
  KeyFinder::similarity_measure_t getSimilarityMeasure() const;
  KeyFinder::tuning_method_t getTuningMethod() const;
  KeyFinder::tone_profile_t getToneProfile() const;
  unsigned int getHopSize() const;
  unsigned int getFftFrameSize() const;
  unsigned int getOctaves() const;
  unsigned int getBpo() const;
  unsigned int getArbitrarySegments() const;
  unsigned int getHcdfPeakPickingNeighbours() const;
  unsigned int getHcdfGaussianSize() const;
  float getHcdfGaussianSigma() const;
  float getStartingFreqA() const;
  float getLastFreq() const;
  float getDirectSkStretch() const;
  float getDetunedBandWeight() const;
  std::vector<float> getCustomToneProfile() const;

  // mutators
  void setWriteToFilesAutomatically(bool);
  void setParallelBatchJobs(bool);
  void setMetadataWriteComment(metadata_write_t);
  void setMetadataWriteGrouping(metadata_write_t);
  void setMetadataWriteKey(metadata_write_t);
  void setMetadataWriteFilename(metadata_write_t);
  void setMetadataFormat(metadata_format_t);
  void setSkipFilesWithExistingTags(bool);
  void setMaxDuration(int);
  void setDFactor(int);
  void setITunesLibraryPath(const QString&);
  void setTraktorLibraryPath(const QString&);
  void setSeratoLibraryPath(const QString&);
  void setMetadataDelimiter(const QString&);
  void setCustomKeyCodes(const QStringList&);
  void setBatchWindowState(const QByteArray&);
  void setBatchWindowGeometry(const QByteArray&);
  void setBatchWindowSplitterState(const QByteArray&);

  // mutators tied to KeyFinder::Parameters object
  void setOffsetToC(bool);
  void setTemporalWindow(KeyFinder::temporal_window_t);
  void setSegmentation(KeyFinder::segmentation_t);
  void setSimilarityMeasure(KeyFinder::similarity_measure_t);
  void setTuningMethod(KeyFinder::tuning_method_t);
  void setToneProfile(KeyFinder::tone_profile_t);
  void setHopSize(unsigned int);
  void setFftFrameSize(unsigned int);
  void setOctaves(unsigned int);
  void setBps(unsigned int);
  void setArbitrarySegments(unsigned int);
  void setHcdfPeakPickingNeighbours(unsigned int);
  void setHcdfGaussianSize(unsigned int);
  void setHcdfGaussianSigma(float);
  void setStartingFreqA(float);
  void setDirectSkStretch(float);
  void setDetunedBandWeight(float);
  void setCustomToneProfile(const std::vector<float>&);

  // not necessarily related to user preferences, just here for convenience
  QString getKeyCode(int) const;
  QStringList getKeyCodeList() const;
  QColor getKeyColour(int) const;
  void setImageColours(QImage&, int) const;

private:
  GuiStrings* strings;

  bool writeToFilesAutomatically;
  bool parallelBatchJobs;
  metadata_write_t metadataWriteComment;
  metadata_write_t metadataWriteGrouping;
  metadata_write_t metadataWriteKey;
  metadata_write_t metadataWriteFilename;
  metadata_format_t metadataFormat;
  bool skipFilesWithExistingTags;
  int maxDuration;
  int dFactor;
  QString iTunesLibraryPath;
  QString traktorLibraryPath;
  QString seratoLibraryPath;
  QString metadataDelimiter;
  QStringList customKeyCodes;
  QStringList defaultKeyCodes;
  QList<QColor> keyColours;
  QByteArray batchWindowState;
  QByteArray batchWindowGeometry;
  QByteArray batchWindowSplitterState;
};

#endif
