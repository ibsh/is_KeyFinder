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

// No longer a singleton once GUI implemented; made sense to keep one per window

class Preferences{
public:
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
  bool getOffsetToC() const;
  char getTemporalWindow() const;
  char getHcdf() const;
  char getSimilarityMeasure() const;
  char getTagFormat() const;
  int getHopSize() const;
  int getFftFrameSize() const;
  int getOctaves() const;
  int getBpo() const;
  int getDFactor() const;
  int getToneProfile() const;
  int getHcdfPeakPickingNeighbours() const;
  int getHcdfArbitrarySegments() const;
  int getHcdfGaussianSize() const;
  int getTuningMethod() const;
  float getHcdfGaussianSigma() const;
  float getStartingFreqA() const;
  float getBinFreq(int) const;
  float getLastFreq() const;
  float getDirectSkStretch() const;
  float getDetunedBandWeight() const;
  QString getITunesLibraryPath() const;
  QString getTraktorLibraryPath() const;
  QString getSeratoLibraryPath() const;
  QList<float> getCustomToneProfile() const;
  QStringList getCustomKeyCodes() const;

  // setters
  void setWriteTagsAutomatically(bool);
  void setParallelBatchJobs(bool);
  void setWriteToTagComment(bool);
  void setWriteToTagGrouping(bool);
  void setWriteToTagKey(bool);
  void setSkipFilesWithExistingTags(bool);
  void setOffsetToC(bool);
  void setTemporalWindow(char);
  void setHcdf(char);
  void setSimilarityMeasure(char);
  void setTagFormat(char);
  void setHopSize(int);
  void setFftFrameSize(int);
  void setOctaves(int);
  void setBps(int);
  void setDFactor(int);
  void setToneProfile(int);
  void setHcdfPeakPickingNeighbours(int);
  void setHcdfArbitrarySegments(int);
  void setHcdfGaussianSize(int);
  void setTuningMethod(int);
  void setHcdfGaussianSigma(float);
  void setStartingFreqA(float);
  void setDirectSkStretch(float);
  void setDetunedBandWeight(float);
  void setITunesLibraryPath(const QString&);
  void setTraktorLibraryPath(const QString&);
  void setSeratoLibraryPath(const QString&);
  void setCustomToneProfile(const QList<float>&);
  void setCustomKeyCodes(const QStringList&);

  void save();

  // not necessarily related to user preferences, here for convenience
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
  bool offsetToC;
  char temporalWindow;
  char hcdf;
  char similarityMeasure;
  char tagFormat;
  int hopSize;
  int fftFrameSize;
  int octaves;
  int bps;
  int dFactor;
  int toneProfile;
  int hcdfPeakPickingNeighbours;
  int hcdfArbitrarySegments;
  int hcdfGaussianSize;
  int tuningMethod;
  float hcdfGaussianSigma;
  float stFreq;
  float directSkStretch;
  float detunedBandWeight;
  QString iTunesLibraryPath;
  QString traktorLibraryPath;
  QString seratoLibraryPath;
  QList<float> customToneProfile;
  QStringList customKeyCodes;
  QList<float> binFreqs;
  QStringList defaultKeyCodes;
  QList<QColor> keyColours;
  void generateBinFreqs();
};

#endif
