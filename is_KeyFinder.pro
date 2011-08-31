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
TEMPLATE = app

CONFIG(debug, debug|release){
	# DEBUG (UNIT TESTING) CONFIGURATION
	QT -= gui
	QT += testlib
	TARGET = keyfinder_test
	CONFIG += console
	CONFIG -= app_bundle
	HEADERS += unittest.h
	SOURCES += \
		test__main.cpp \
		test_audiobuffer.cpp \
		test_chromagram.cpp \
		test_decoderlibav.cpp \
		test_downsamplerib.cpp \
		test_downsamplerlibsrc.cpp \
		test_directskpostproc.cpp \
		test_constantqpostproc.cpp
}else{
	# RELEASE CONFIGURATION
	QT += gui
	TARGET = KeyFinder
	HEADERS += \
		guiabout.h \
		guibatch.h \
		guidetail.h \
		guimenuhandler.h \
		guiprefs.h \
		guivisuals.h
	SOURCES += \
		main.cpp \
		guiabout.cpp \
		guibatch.cpp \
		guidetail.cpp \
		guimenuhandler.cpp \
		guiprefs.cpp \
		guivisuals.cpp
	FORMS += \
		detailwindow.ui \
		batchwindow.ui \
		aboutdialog.ui \
		prefsdialog.ui
}

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
    monaural.cpp \
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
    fftppconstantq.cpp \
    fftppdirectsk.cpp \
    spectrumanalyserfftw.cpp \
		spectrumanalysergoertzel.cpp \
    hcdfarbitrary.cpp \
    audiostream.cpp

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
    monaural.h \
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
    fftppconstantq.h \
    spectrumanalyserfftw.h \
    spectrumanalysergoertzel.h \
		fftpp.h \
		hcdfarbitrary.h \
		audiostream.h
OTHER_FILES += \
		is_KeyFinder.pro.user \
    README

INCLUDEPATH += /usr/local/include/taglib

LIBS += -lfftw3 -lavcodec -lavformat -lsamplerate -ltag -lavutil

ICON = is_KeyFinder.icns

UI_DIR = ../is_KeyFinder-build-desktop/

RESOURCES += \
    resources.qrc
