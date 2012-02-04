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
#include <QSettings>
#include <QString>
#include <QImage>
#include <QColor>

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
	char getTemporalWindow() const;
	char getHcdf() const;
	char getSimilarityMeasure() const;
  char getTagFormat() const;
	int getHopSize() const;
	int getFftFrameSize() const;
	int getOctaves() const;
	int getBpo() const;
	int getOctaveOffset() const;
	int getDFactor() const;
	int getToneProfile() const;
	int getHcdfPeakPickingNeighbours() const;
	int getHcdfArbitrarySegments() const;
	int getHcdfGaussianSize() const;
	int getTuningMethod() const;
	float getHcdfGaussianSigma() const;
	float getBinFreq(int) const;
	float getLastFreq() const;
	float getDirectSkStretch() const;
	float getDetunedBandWeight() const;
  std::vector<float> getCustomToneProfile() const;
  std::vector<QString> getCustomKeyCodes() const;
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
	char temporalWindow;
	char hcdf;
	char similarityMeasure;
  char tagFormat;
	int hopSize;
	int fftFrameSize;
	int octaves;
	int bps;
	int octaveOffset;
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
	std::vector<float> customToneProfile;
	std::vector<QString> customKeyCodes;
	std::vector<float> binFreqs;
  std::vector<QString> defaultKeyCodes;
  std::vector<QColor> keyColours;
	void generateBinFreqs();
};

#endif
