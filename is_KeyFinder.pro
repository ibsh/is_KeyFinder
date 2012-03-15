# Copyright 2011 Ibrahim Sha'ath

# This file is part of KeyFinder.

# KeyFinder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# KeyFinder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.


QT += core
QT += gui
QT += xml
QT += xmlpatterns
QT += network
TEMPLATE = app
TARGET = KeyFinder

SOURCES += \
    decoderlibav.cpp \
    downsamplerlibsrc.cpp \
    downsamplerib.cpp \
    preferences.cpp \
    decoder.cpp \
    downsampler.cpp \
    metadatataglib.cpp \
    main.cpp \
    guiabout.cpp \
    guibatch.cpp \
    guidetail.cpp \
    guimenuhandler.cpp \
    guiprefs.cpp \
    keyfinderapplication.cpp \
    asynckeyprocess.cpp \
    asyncmetadatareadprocess.cpp \
    os_windows.cpp \
    asyncfileobject.cpp \
    externalplaylist.cpp \
    externalplaylistserato.cpp

HEADERS  += \
    decoderlibav.h \
    decoder.h \
    chromagram.h \
    downsamplerlibsrc.h \
    downsamplerib.h \
    downsampler.h \
    windowfunctions.h \
    toneprofiles.h \
    spectrumanalyser.h \
    preferences.h \
    spectrumanalyserfactory.h \
    exception.h \
    hcdf.h \
    keyclassifier.h \
		metadatataglib.h \
    hcdfharte.h \
    hcdfcosine.h \
		hcdfnone.h \
    fftppdirectsk.h \
    spectrumanalyserfftw.h \
		fftpp.h \
		hcdfarbitrary.h \
    audiostream.h \
    guiabout.h \
    guibatch.h \
    guidetail.h \
    guimenuhandler.h \
    guiprefs.h \
    keyfinderapplication.h \
    asynckeyprocess.h \
    asynckeyresult.h \
    asyncmetadatareadprocess.h \
    asyncmetadatareadresult.h \
    os_windows.h \
    _VERSION.h \
    asyncfileobject.h \
    externalplaylist.h \
    externalplaylistserato.h

FORMS += \
    detailwindow.ui \
    batchwindow.ui \
    aboutdialog.ui \
    prefsdialog.ui

OTHER_FILES += \
		README \
    win32.rc \
    Info.plist

UI_DIR = ui

# for libav
QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS

RESOURCES += resources.qrc

unix|macx{
  LIBS += -lkeyfinder
  LIBS += -lavutil
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lfftw3
  LIBS += -lsamplerate
  LIBS += -ltag
  LIBS += -lxerces-c
  LIBS += -lxqilla

  ICON = is_KeyFinder.icns
  QMAKE_INFO_PLIST = Info.plist
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
  LIBS += -lkeyfinder
	LIBS += -lavcodec
	LIBS += -lavformat
	LIBS += -lavutil
	LIBS += -lsamplerate-0
	LIBS += -lfftw3-3

	INCLUDEPATH += $$PWD/../Desktop/taglib-1.7
	INCLUDEPATH += $$PWD/../Desktop/taglib-1.7/include
	DEPENDPATH += $$PWD/../Desktop/taglib-1.7/taglib
	LIBS += -L$$PWD/../Desktop/taglib-1.7/taglib -llibtag

	OTHER_FILES += is_KeyFinder.ico
	RC_FILE = win32.rc
}
