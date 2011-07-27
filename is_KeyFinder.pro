#-------------------------------------------------
#
# Project created by QtCreator 2011-06-18T10:18:36
#
#-------------------------------------------------

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
		test_downsamplerib.cpp
}else{
	# RELEASE CONFIGURATION
	QT += gui
	TARGET = KeyFinder
	HEADERS += \
		detailwindow.h \
		batchwindow.h \
		visuals.h
	SOURCES += \
		main.cpp \
		detailwindow.cpp \
		batchwindow.cpp \
		visuals.cpp
	FORMS += detailwindow.ui batchwindow.ui
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
    logbinspostproc.cpp \
    windowfunctions.cpp \
    toneprofiles.cpp \
    spectrumanalyser.cpp \
    preferences.cpp \
    spectrumanalyserfactory.cpp \
    exception.cpp \
    monaural.cpp \
    decoder.cpp \
    downsampler.cpp \
    keyclassifier.cpp \
    metadatataglib.cpp \
    metadata.cpp \
		hcdf.cpp

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
    logbinspostproc.h \
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

OTHER_FILES += \
		is_KeyFinder.pro.user

INCLUDEPATH += /usr/local/include/taglib

LIBS += -lfftw3 -lsndfile -lavcodec -lavformat -lsamplerate -lavutil -ltag

ICON = is_KeyFinder.icns
