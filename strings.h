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

#ifndef STRINGS_H
#define STRINGS_H

#include <QObject>
#include <QString>
#include <QStringList>

/*

The purpose of this class is to avoid unnecessarily subclassing
QObject in other classes that require translated or common strings.

Don't translate anything that goes into the log file (qDebug / qCritical).

*/

class GuiStrings : public QObject{
  Q_OBJECT
public:
  static GuiStrings* getInstance();
  QString appName() const;
  QString appUrl() const;
  QString delim() const;
  QString notApplicable() const;
  QStringList iTunesDefaultPlaylists() const;
  QStringList traktorDefaultPlaylists() const;
  QString seratoSmartCratesDirName() const;
  QString seratoSubcratesDirName() const;
  QString libsrcError() const;
  QString libavCouldNotOpenFile(int) const;
  QString libavCouldNotFindStreamInformation() const;
  QString libavCouldNotFindAudioStream() const;
  QString libavUnsupportedCodec() const;
  QString libavCouldNotOpenCodec(const char*, int) const;
  QString libavCouldNotCreateResampleContext() const;
  QString libavCouldNotResample() const;
  QString libavTooManyBadPackets(int) const;
  QString durationExceedsPreference(int, int, int) const;

private:
  explicit GuiStrings(QObject *parent = 0);
  GuiStrings(GuiStrings const &);
  void operator=(GuiStrings const &);
};

#endif // STRINGS_H
