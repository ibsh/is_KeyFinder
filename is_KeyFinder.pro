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
# libkeyfinder     0.2.2
#  |-> boost      1.49.0
#  \-> fftw        3.3.2
# libav            0.7.6
# taglib           1.8.0
#
# MAC ONLY:
# xerces-c         3.1.1
# xqilla           2.3.0

QT += \
    core \
    gui \
    widgets \
    xml \
    xmlpatterns \
    concurrent \
    network

TEMPLATE = app
TARGET = KeyFinder
UI_DIR = ui
RESOURCES += resources.qrc

HEADERS  += \
    _VERSION.h \
    asyncfileobject.h \
    asynckeyprocess.h \
    asynckeyresult.h \
    asyncmetadatareadprocess.h \
    asyncmetadatareadresult.h \
    decoder.h \
    decoderlibav.h \
    externalplaylist.h \
    externalplaylistserato.h \
    guiabout.h \
    guibatch.h \
    guidetail.h \
    guimenuhandler.h \
    guiprefs.h \
    libkeyfindersingleton.h \
    metadatataglib.h \
    metadatawriteresult.h \
    os_windows.h \
    preferences.h \
    strings.h

SOURCES += \
    asyncfileobject.cpp \
    asynckeyprocess.cpp \
    asyncmetadatareadprocess.cpp \
    decoder.cpp \
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
    metadatataglib.cpp \
    os_windows.cpp \
    preferences.cpp \
    strings.cpp

FORMS += \
    aboutdialog.ui \
    batchwindow.ui \
    detailwindow.ui \
    prefsdialog.ui

OTHER_FILES += \
		README \
    win32.rc

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS # for libav

unix|macx{
  LIBS += -lkeyfinder
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lavutil
  LIBS += -ltag
  LIBS += -lboost_system

  ICON = is_KeyFinder.icns
  QMAKE_INFO_PLIST =
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
	INCLUDEPATH += C:/MSYS/local/include
	DEPENDPATH += C:/MSYS/local/bin
	LIBS += -LC:/MSYS/local/bin
	LIBS += -lkeyfinder0
	LIBS += -lavcodec
	LIBS += -lavformat
	LIBS += -lavutil

	INCLUDEPATH += C:/MSYS/home/user/taglib-1.8
	INCLUDEPATH += C:/MSYS/home/user/taglib-1.8/include
	DEPENDPATH += C:/MSYS/home/user/taglib-1.8/taglib
	LIBS += -LC:/MSYS/home/user/taglib-1.8/taglib -llibtag

	OTHER_FILES += is_KeyFinder.ico
	RC_FILE = win32.rc
}

unix {
    target.path = $$[QT_INSTALL_PREFIX]/bin
    INSTALLS += target
}

TRANSLATIONS = \
    is_keyfinder_en_GB.ts \
    is_keyfinder_en_US.ts \
    is_keyfinder_fr.ts \
    is_keyfinder_sv.ts \
    is_keyfinder_da.ts \
    is_keyfinder_it.ts \
    is_keyfinder_pl.ts
