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

#include "strings.h"


GuiStrings::GuiStrings(QObject *parent) : QObject(parent){
}

GuiStrings* GuiStrings::getInstance(){
  static GuiStrings instance;
  return &instance;
}

QString GuiStrings::appName() const{
  return "KeyFinder";
}

QString GuiStrings::appUrl() const{
  return "http://www.ibrahimshaath.co.uk/keyfinder/";
}

QString GuiStrings::delim() const{
  return " - ";
}

QString GuiStrings::notApplicable() const{
  return tr("N/A");
}

QStringList GuiStrings::iTunesDefaultPlaylists() const{
  QStringList defaultPlaylists;
  defaultPlaylists << tr("Library");
  defaultPlaylists << tr("Music");
  defaultPlaylists << tr("Movies");
  defaultPlaylists << tr("Films");
  defaultPlaylists << tr("TV Shows");
  defaultPlaylists << tr("TV Programmes");
  defaultPlaylists << tr("Podcasts");
  defaultPlaylists << tr("Books");
  defaultPlaylists << tr("Purchased");
  defaultPlaylists << tr("Genius");
  defaultPlaylists << tr("iTunes DJ");
  return defaultPlaylists;
}

QStringList GuiStrings::traktorDefaultPlaylists() const{
  QStringList defaultPlaylists;
  defaultPlaylists << tr("_LOOPS");
  defaultPlaylists << tr("_RECORDINGS");
  defaultPlaylists << tr("Preparation");
  return defaultPlaylists;
}

QString GuiStrings::seratoSmartCratesDirName() const{
  return tr("SmartCrates");
}

QString GuiStrings::seratoSubcratesDirName() const{
  return tr("Subcrates");
}

QString GuiStrings::libsrcError() const{
  return tr("Error in LibSRC sample rate conversion");
}

QString GuiStrings::libavCouldNotOpenFile(int n) const{
  return tr("Could not open audio file (%1)").arg(QString::number(n));
}

QString GuiStrings::libavCouldNotFindStreamInformation() const{
  return tr("Could not find stream information");
}

QString GuiStrings::libavCouldNotFindAudioStream() const{
  return tr("Could not find an audio stream");
}

QString GuiStrings::libavUnsupportedCodec() const{
  return tr("Audio stream has unsupported codec");
}

QString GuiStrings::libavCouldNotOpenCodec(const char* name, int result) const{
  return tr("Could not open audio codec %1 (%2)").arg(name).arg(QString::number(result));
}

QString GuiStrings::libavCouldNotCreateResampleContext() const{
  return tr("Could not create ReSampleContext");
}

QString GuiStrings::libavCouldNotResample() const{
  return tr("Could not resample");
}

QString GuiStrings::libavTooManyBadPackets(int n) const{
  return tr("%n bad packet(s)", "", n);
}

QString GuiStrings::durationExceedsPreference(int mins, int secs, int max) const{
  return tr("Duration (%1:%2) exceeds specified maximum (%3:00)")
    .arg(QString::number(mins), 2, '0')
    .arg(QString::number(secs), 2, '0')
    .arg(QString::number(max), 2, '0');
}
