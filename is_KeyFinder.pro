#-------------------------------------------------
#
# Project created by QtCreator 2011-06-18T10:18:36
#
#-------------------------------------------------

QT       += core gui

TARGET = KeyFinder
TEMPLATE = app
ICON = KeyFinder.icns


SOURCES += main.cpp\
				detailwindow.cpp \
    decoderlibsndfile.cpp \
    decoderlibav.cpp \
    constantqpostproc.cpp \
    chromagram.cpp \
    basicmono.cpp \
    audiobuffer.cpp \
    hcdfharte.cpp \
    harmonicclassifier.cpp \
    goertzelanalyser.cpp \
    fftwanalyser.cpp \
    fftpostprocessor.cpp \
    fatalexception.cpp \
    downsamplerlibsrc.cpp \
    downsamplerib.cpp \
    downsamplerdiscard.cpp \
    directskpostproc.cpp \
    logbinspostproc.cpp \
    windowfunctions.cpp \
    toneprofiles.cpp \
    spectrumanalyser.cpp \
    preferences.cpp \
    spectrumanalyserfactory.cpp

HEADERS  += detailwindow.h \
    decoderlibsndfile.h \
    decoderlibav.h \
    decoder.h \
    constantqpostproc.h \
    chromagram.h \
    basicmono.h \
    audiobuffer.h \
    hcdfharte.h \
    harmonicclassifier.h \
    goertzelanalyser.h \
    fftwanalyser.h \
    fftpostprocessor.h \
    fatalexception.h \
    downsamplerlibsrc.h \
    downsamplerib.h \
    downsamplerdiscard.h \
    downsampler.h \
    directskpostproc.h \
    logbinspostproc.h \
    windowfunctions.h \
    toneprofiles.h \
    spectrumanalyser.h \
    preferences.h \
    monaural.h \
    spectrumanalyserfactory.h

FORMS    += detailwindow.ui

OTHER_FILES += \
    is_KeyFinder.pro.user

LIBS += -lfftw3 -lsndfile -lavcodec -lavformat -lsamplerate
