#*************************************************************************
#
# Copyright 2011-2013 Ibrahim Sha'ath
#
# This file is part of KeyFinder.
#
# KeyFinder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# KeyFinder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.
#
#*************************************************************************

# CURRENT DEPENDENCIES:
# qt               5.0.0
# libkeyfinder    0.10.0
#  |-> boost      1.52.0
#  \-> fftw        3.3.2
# libav            0.7.7
# taglib           1.8.0
#
# MAC ONLY:
# xerces-c         3.1.1
# xqilla           2.3.0

QT += \
    core \
    gui \
    widgets \
    concurrent \
    network

TEMPLATE = app
TARGET = KeyFinder
DEPENDPATH += .
UI_DIR = ui

include(./forms/forms.pri)
include(./resources/resources.pri)

HEADERS  += \
    _VERSION.h \
    asyncfileobject.h \
    asynckeyprocess.h \
    asynckeyresult.h \
    asyncmetadatareadprocess.h \
    asyncmetadatareadresult.h \
    decoderlibav.h \
    externalplaylist.h \
    externalplaylistserato.h \
    guiabout.h \
    guibatch.h \
    guidetail.h \
    guimenuhandler.h \
    guiprefs.h \
    libkeyfindersingleton.h \
    metadatafilename.h \
    metadatataglib.h \
    metadatawriteresult.h \
    os_windows.h \
    preferences.h \
    strings.h

SOURCES += \
    asyncfileobject.cpp \
    asynckeyprocess.cpp \
    asyncmetadatareadprocess.cpp \
    decoderlibav.cpp \
    externalplaylist.cpp \
    externalplaylistserato.cpp \
    guiabout.cpp \
    guibatch.cpp \
    guidetail.cpp \
    guimenuhandler.cpp \
    guiprefs.cpp \
    libkeyfindersingleton.cpp \
    main.cpp \
    metadatafilename.cpp \
    metadatataglib.cpp \
    os_windows.cpp \
    preferences.cpp \
    strings.cpp

OTHER_FILES += README

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS # for libav

unix|macx{
  LIBS += -lkeyfinder
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lavutil
  LIBS += -ltag
  LIBS += -lboost_system
  LIBS += -lboost_thread
}

macx{
  DEPENDPATH += /usr/local/lib
  INCLUDEPATH += /usr/local/include
  CONFIG -= ppc ppc64
  CONFIG += x86 x86_64

  LIBS += -lxerces-c
  LIBS += -lxqilla
}

win32{
  QT += xml xmlpatterns
  INCLUDEPATH += C:/mingw/local/include
  DEPENDPATH += C:/mingw/local/bin
  LIBS += -LC:/mingw/local/bin
  LIBS += -lkeyfinder0
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lavutil
  LIBS += -llibtag
  LIBS += -LC:/mingw/local/lib
  LIBS += -lboost_system-47-mt-1_52
  LIBS += -lboost_thread-47-mt-1_52
}

unix {
  QT += xml xmlpatterns
  target.path = $$[QT_INSTALL_PREFIX]/bin
  INSTALLS += target
}

TRANSLATIONS = \
  $$PWD/translations/is_keyfinder_en_GB.ts \
  $$PWD/translations/is_keyfinder_da.ts \
  $$PWD/translations/is_keyfinder_de.ts \
  $$PWD/translations/is_keyfinder_en_US.ts \
  $$PWD/translations/is_keyfinder_es.ts \
  $$PWD/translations/is_keyfinder_fr.ts \
  $$PWD/translations/is_keyfinder_he.ts \
  $$PWD/translations/is_keyfinder_hr.ts \
  $$PWD/translations/is_keyfinder_it.ts \
  $$PWD/translations/is_keyfinder_nl.ts \
  $$PWD/translations/is_keyfinder_pl.ts \
  $$PWD/translations/is_keyfinder_ru.ts \
  $$PWD/translations/is_keyfinder_sv.ts
