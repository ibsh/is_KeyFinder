#*************************************************************************
#
# Copyright 2011 Ibrahim Sha'ath
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
# qt               4.8.2
# libkeyfinder     0.1.5
#  |-> boost      1.49.0
#  \-> fftw        3.3.2
# libav            0.7.6
# libsamplerate    0.1.8
# taglib           1.8.0
#
# MAC ONLY:
# xerces-c         3.1.1
# xqilla           2.3.0

QT += core
QT += gui
QT += xml
QT += xmlpatterns
QT += network
TEMPLATE = app
TARGET = KeyFinder

HEADERS  += \
    _VERSION.h \
    asyncfileobject.h \
    asynckeyprocess.h \
    asynckeyresult.h \
    asyncmetadatareadprocess.h \
    asyncmetadatareadresult.h \
    decoder.h \
    decoderlibav.h \
    downsampler.h \
    downsamplerib.h \
    downsamplerlibsrc.h \
    externalplaylist.h \
    externalplaylistserato.h \
    guiabout.h \
    guibatch.h \
    guidetail.h \
    guimenuhandler.h \
    guiprefs.h \
    libkeyfindersingleton.h \
    metadatataglib.h \
    os_windows.h \
    preferences.h \
    strings.h

SOURCES += \
    asyncfileobject.cpp \
    asynckeyprocess.cpp \
    asyncmetadatareadprocess.cpp \
    decoder.cpp \
    decoderlibav.cpp \
    downsampler.cpp \
    downsamplerib.cpp \
    downsamplerlibsrc.cpp \
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

UI_DIR = ui

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS # for libav

RESOURCES += resources.qrc

unix|macx{
  LIBS += -lkeyfinder
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lavutil
  LIBS += -lsamplerate
  LIBS += -ltag
  LIBS += -lxerces-c
  LIBS += -lxqilla

  ICON = is_KeyFinder.icns
  QMAKE_INFO_PLIST =
}

macx{
  DEPENDPATH += /usr/local/lib
  INCLUDEPATH += /usr/local/include
  CONFIG -= ppc ppc64
  CONFIG += x86 x86_64
}

win32{
	INCLUDEPATH += C:/MSYS/local/include
	DEPENDPATH += C:/MSYS/local/bin
	LIBS += -LC:/MSYS/local/bin
	LIBS += -lkeyfinder0
	LIBS += -lavcodec
	LIBS += -lavformat
	LIBS += -lavutil
	LIBS += -lsamplerate-0

	INCLUDEPATH += C:/MSYS/home/user/taglib-1.7.2
	INCLUDEPATH += C:/MSYS/home/user/taglib-1.7.2/include
	DEPENDPATH += C:/MSYS/home/user/taglib-1.7.2/taglib
	LIBS += -LC:/MSYS/home/user/taglib-1.7.2/taglib -llibtag

	OTHER_FILES += is_KeyFinder.ico
	RC_FILE = win32.rc
}

TRANSLATIONS = \
    is_keyfinder_en_GB.ts \
    is_keyfinder_en_US.ts \
    is_keyfinder_fr.ts \
    is_keyfinder_sv.ts
