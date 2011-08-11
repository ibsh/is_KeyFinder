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
		detailwindow.h \
		batchwindow.h \
		aboutdialog.h \
		visuals.h \
		prefsdialog.h \
		mainmenuhandler.h
	SOURCES += \
		main.cpp \
		detailwindow.cpp \
		batchwindow.cpp \
		aboutdialog.cpp \
		visuals.cpp \
		prefsdialog.cpp \
		mainmenuhandler.cpp
	FORMS += \
		detailwindow.ui \
		batchwindow.ui \
		aboutdialog.ui \
		prefsdialog.ui
}

SOURCES += \
    decoderlibsndfile.cpp \
    decoderlibav.cpp \
    constantqpostproc.cpp \
    chromagram.cpp \
    audiobuffer.cpp \
    goertzelanalyser.cpp \
    fftwanalyser.cpp \
    fftpostprocessor.cpp \
    downsamplerlibsrc.cpp \
    downsamplerib.cpp \
    directskpostproc.cpp \
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
    metadata.cpp \
		hcdf.cpp \
    hcdfharte.cpp \
    hcdfcosine.cpp \
		hcdfnone.cpp

HEADERS  += decoderlibsndfile.h \
    decoderlibav.h \
    decoder.h \
    constantqpostproc.h \
    chromagram.h \
    audiobuffer.h \
    goertzelanalyser.h \
    fftwanalyser.h \
    fftpostprocessor.h \
    downsamplerlibsrc.h \
    downsamplerib.h \
    downsampler.h \
    directskpostproc.h \
    windowfunctions.h \
    toneprofiles.h \
    spectrumanalyser.h \
    preferences.h \
    monaural.h \
    spectrumanalyserfactory.h \
    exception.h \
    hcdf.h \
    keyclassifier.h \
    metadata.h \
		metadatataglib.h \
    hcdfharte.h \
    hcdfcosine.h \
		hcdfnone.h

OTHER_FILES += \
		is_KeyFinder.pro.user

INCLUDEPATH += /usr/local/include/taglib

LIBS += -lfftw3 -lsndfile -lavcodec -lavformat -lsamplerate -ltag -lavutil

ICON = is_KeyFinder.icns

UI_DIR = ../is_KeyFinder-build-desktop/

RESOURCES += \
    resources.qrc
