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
  if(prefs.getMetadataWriteFilename() == METADATA_WRITE_PREPEND){
    if(name.left(dataToWrite.length()) == dataToWrite) return written;
    name = dataToWrite + prefs.getMetadataDelimiter() + name;
  }else if(prefs.getMetadataWriteFilename() == METADATA_WRITE_APPEND){
    if(name.right(dataToWrite.length()) == dataToWrite) return written;
    name = name + prefs.getMetadataDelimiter() + dataToWrite;
  }
  if(file.rename(path + name + extn)){
    written << path << name << extn;
  }
  return written;
}
