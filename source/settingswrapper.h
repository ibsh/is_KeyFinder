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

#ifndef SETTINGSWRAPPER_H
#define SETTINGSWRAPPER_H

#include <QSettings>
#include <QStringList>

class SettingsWrapper {
public:
  virtual void beginGroup(const QString& group) = 0;
  virtual void endGroup() = 0;
  virtual QVariant value(const QString &key, const QVariant &defaultValue) const = 0;
  virtual void setValue(const QString& key, const QVariant& value) = 0;
  virtual QStringList allKeys() const = 0;
};

class SettingsWrapperQt : public SettingsWrapper {
public:
  virtual void beginGroup(const QString& group);
  virtual void endGroup();
  virtual QVariant value(const QString &key, const QVariant &defaultValue) const;
  virtual void setValue(const QString& key, const QVariant& value);
  virtual QStringList allKeys() const;
private:
  QSettings priv;
};

#endif // SETTINGSWRAPPER_H
