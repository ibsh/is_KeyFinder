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

#include "metadatafilename.h"

QStringList writeKeyToFilename(const QString& filename, KeyFinder::key_t key, const Preferences & prefs) {
  QString dataToWrite = prefs.getKeyCode(key);
  QFile file(filename);
  QString path = file.fileName().left(file.fileName().lastIndexOf("/") + 1);
  QString extn = file.fileName().mid(file.fileName().lastIndexOf("."));
  QString name = file.fileName().mid(file.fileName().lastIndexOf("/") + 1);
  name = name.left(name.length() - extn.length());
  QStringList written;
  QString newName = prefs.newString(dataToWrite, name, METADATA_CHARLIMIT_FILENAME, prefs.getMetadataWriteFilename());
  if (newName != "") {
    name = newName;
  }
  if (file.rename(path + name + extn)) {
    written << path << name << extn;
  }
  return written;
}
