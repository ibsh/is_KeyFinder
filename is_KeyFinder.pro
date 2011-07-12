#-------------------------------------------------
#
# Project created by QtCreator 2011-06-18T10:18:36
#
#-------------------------------------------------

QT       += core gui
TEMPLATE = app
TARGET = KeyFinder

SOURCES += main.cpp\
		detailwindow.cpp \
    decoderlibsndfile.cpp \
    decoderlibav.cpp \
    constantqpostproc.cpp \
    chromagram.cpp \
    audiobuffer.cpp \
    hcdfharte.cpp \
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
    batchwindow.cpp \
    visuals.cpp \
    exception.cpp \
    monaural.cpp \
    decoder.cpp \
    downsampler.cpp \
    keyclassifier.cpp \
    metadatataglib.cpp \
    metadata.cpp

HEADERS  += detailwindow.h \
    decoderlibsndfile.h \
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
    batchwindow.h \
    visuals.h \
    exception.h \
    hcdf.h \
    keyclassifier.h \
    metadata.h \
    metadatataglib.h

FORMS += detailwindow.ui \
    batchwindow.ui

OTHER_FILES += \
		is_KeyFinder.pro.user

INCLUDEPATH += /usr/local/include/taglib

LIBS += -lfftw3 -lsndfile -lavcodec -lavformat -lsamplerate -lavutil -ltag

ICON = is_KeyFinder.icns
