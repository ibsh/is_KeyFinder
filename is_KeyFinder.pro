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
#QT += xml
TEMPLATE = app
TARGET = KeyFinder

SOURCES += \
    decoderlibav.cpp \
    chromagram.cpp \
    downsamplerlibsrc.cpp \
    downsamplerib.cpp \
    windowfunctions.cpp \
    toneprofiles.cpp \
    spectrumanalyser.cpp \
    preferences.cpp \
    spectrumanalyserfactory.cpp \
    decoder.cpp \
    downsampler.cpp \
    keyclassifier.cpp \
    metadatataglib.cpp \
		hcdf.cpp \
    hcdfharte.cpp \
    hcdfcosine.cpp \
		hcdfnone.cpp \
    keyfinderworkerthread.cpp \
    fftpp.cpp \
    fftppdirectsk.cpp \
    spectrumanalyserfftw.cpp \
    hcdfarbitrary.cpp \
    audiostream.cpp \
    main.cpp \
    guiabout.cpp \
    guibatch.cpp \
    guidetail.cpp \
    guimenuhandler.cpp \
    guiprefs.cpp \
    guivisuals.cpp

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
    keyfinderworkerthread.h \
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
    guivisuals.h

FORMS += \
    detailwindow.ui \
    batchwindow.ui \
    aboutdialog.ui \
    prefsdialog.ui

OTHER_FILES += \
		is_KeyFinder.pro.user \
		README \
    win32.rc

UI_DIR = ../is_KeyFinder-build-desktop/

RESOURCES += resources.qrc

mac{
	LIBS += -lfftw3 -lavcodec -lavformat -lsamplerate -ltag -lavutil
	INCLUDEPATH += /usr/local/include/taglib
	ICON = is_KeyFinder.icns
  QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.6.sdk
}

win32{
	INCLUDEPATH += $$PWD/../Desktop/fftw-3.3-dll32
	DEPENDPATH += $$PWD/../Desktop/fftw-3.3-dll32
	LIBS += -L$$PWD/../Desktop/fftw-3.3-dll32 -llibfftw3-3

	INCLUDEPATH += $$PWD/../Desktop/libav-win32-0.7.2/usr/include
	DEPENDPATH += $$PWD/../Desktop/libav-win32-0.7.2/usr/bin
	LIBS += -L$$PWD/../Desktop/libav-win32-0.7.2/usr/bin -lavcodec
	LIBS += -lavformat
	LIBS += -lavutil

	INCLUDEPATH += $$PWD/../Desktop/libsamplerate-0.1.8
	INCLUDEPATH += $$PWD/../Desktop/libsamplerate-0.1.8/src
	DEPENDPATH += $$PWD/../Desktop/libsamplerate-0.1.8
	LIBS += -L$$PWD/../Desktop/libsamplerate-0.1.8/ -llibsamplerate-0


	INCLUDEPATH += $$PWD/../Desktop/taglib-1.7
	INCLUDEPATH += $$PWD/../Desktop/taglib-1.7/include
	DEPENDPATH += $$PWD/../Desktop/taglib-1.7/taglib
	LIBS += -L$$PWD/../Desktop/taglib-1.7/taglib -llibtag

	OTHER_FILES += \
		is_KeyFinder.ico
	RC_FILE = win32.rc
}

















