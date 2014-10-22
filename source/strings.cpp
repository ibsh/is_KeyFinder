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

#include "strings.h"


GuiStrings::GuiStrings(QObject *parent) : QObject(parent) {
}

GuiStrings* GuiStrings::getInstance() {
  static GuiStrings instance;
  return &instance;
}

QString GuiStrings::appName() const {
  return "KeyFinder";
}

QString GuiStrings::appUrl() const {
  return "http://www.ibrahimshaath.co.uk/keyfinder/";
}

QString GuiStrings::delim() const {
  return " - ";
}

QString GuiStrings::notApplicable() const {
  //: Status of an individual file in the Batch window
  return tr("N/A");
}

QString GuiStrings::preferences() const {
  //: Main menu option
  return QString(QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU","Preferences..."));
}

QString GuiStrings::about() const {
  //: Main menu option and title of About window, contains the app name at %1
  return QString(QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU","About %1"));
}

QStringList GuiStrings::iTunesDefaultPlaylists() const {
  QStringList defaultPlaylists;
  //: Name of the main iTunes library (to exclude from the Batch window)
  defaultPlaylists << tr("Library");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Music");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Movies");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Films");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("TV Shows");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("TV Programmes");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Podcasts");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Books");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Purchased");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Genius");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("iTunes DJ");
  //: Name of a default iTunes playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Audiobooks");
  return defaultPlaylists;
}

QStringList GuiStrings::traktorDefaultPlaylists() const {
  QStringList defaultPlaylists;
  //: Name of a default Traktor playlist (to exclude from the Batch window)
  defaultPlaylists << tr("_LOOPS");
  //: Name of a default Traktor playlist (to exclude from the Batch window)
  defaultPlaylists << tr("_RECORDINGS");
  //: Name of a default Traktor playlist (to exclude from the Batch window)
  defaultPlaylists << tr("Preparation");
  return defaultPlaylists;
}

QString GuiStrings::seratoSmartCratesDirName() const {
  //: Name of the Smart Crates subdirectory in Serato
  return tr("SmartCrates");
}

QString GuiStrings::seratoSubcratesDirName() const {
  //: Name of the Subcrates subdirectory in Serato
  return tr("Subcrates");
}

QString GuiStrings::libsrcError() const {
  //: Status of an individual file in the Batch window
  return tr("Error in LibSRC sample rate conversion");
}

QString GuiStrings::libavCouldNotOpenFile(int n) const {
  //: Status of an individual file in the Batch window
  return tr("Could not open audio file (%1)").arg(QString::number(n));
}

QString GuiStrings::libavCouldNotFindStreamInformation() const {
  //: Status of an individual file in the Batch window
  return tr("Could not find stream information");
}

QString GuiStrings::libavCouldNotFindAudioStream() const {
  //: Status of an individual file in the Batch window
  return tr("Could not find an audio stream");
}

QString GuiStrings::libavUnsupportedCodec() const {
  //: Status of an individual file in the Batch window
  return tr("Audio stream has unsupported codec");
}

QString GuiStrings::libavCouldNotOpenCodec(const char* name, int result) const {
  //: Status of an individual file in the Batch window
  return tr("Could not open audio codec %1 (%2)").arg(name).arg(QString::number(result));
}

QString GuiStrings::libavCouldNotCreateResampleContext() const {
  //: Status of an individual file in the Batch window
  return tr("Could not create ReSampleContext");
}

QString GuiStrings::libavCouldNotResample() const {
  //: Status of an individual file in the Batch window
  return tr("Could not resample");
}

QString GuiStrings::libavTooManyBadPackets(int n) const {
  //: Status of an individual file in the Batch window
  return tr("%n bad packet(s)", "", n);
}

QString GuiStrings::durationExceedsPreference(int mins, int secs, int max) const {
  //: Status of an individual file in the Batch window; includes minutes and seconds at %1:%2, and the max number of minutes at %3
  return tr("Duration (%1:%2) exceeds specified maximum (%3:00)")
    .arg(QString::number(mins), 2, '0')
    .arg(QString::number(secs), 2, '0')
    .arg(QString::number(max),  2, '0');
}
