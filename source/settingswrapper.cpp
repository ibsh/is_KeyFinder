#include "settingswrapper.h"

void SettingsWrapperQt::beginGroup(const QString& g) {
  priv.beginGroup(g);
}

void SettingsWrapperQt::endGroup() {
  priv.endGroup();
}

QVariant SettingsWrapperQt::value(
  const QString &key, const QVariant &defaultValue
) const {
  return priv.value(key, defaultValue);
}

void SettingsWrapperQt::setValue(const QString &key, const QVariant &value) {
  priv.setValue(key, value);
}

QStringList SettingsWrapperQt::allKeys() const {
  return priv.allKeys();
}
