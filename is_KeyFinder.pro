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
# qt               5.4.1
# libkeyfinder     2.2.1
#  \-> fftw        3.3.4
# libav            0.7.7
# taglib           1.10

QT += \
  core \
  gui \
  widgets \
  concurrent \
  network

TEMPLATE = app
DEPENDPATH += .

CONFIG += c++11

include(./source/source.pri)
include(./forms/forms.pri)

CONFIG(test) {
  TARGET = KeyFinderTests
  include(./test/test.pri)
  SOURCES += $$PWD/test/main.cpp
  CONFIG += console
  CONFIG -= app_bundle
  LIBS += -lgtest
} else {
  TARGET = KeyFinder
  SOURCES += $$PWD/source/main.cpp
  include(./resources/resources.pri)
  UI_DIR = ui
}

OTHER_FILES += README.md

QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS # for libav

unix|macx {
  LIBS += -L/usr/local/lib
  LIBS += -lkeyfinder
  LIBS += -lavcodec
  LIBS += -lavformat
  LIBS += -lavutil
  LIBS += -lswresample
  LIBS += -ltag
  LIBS += -lz
}

macx {
  # Homebrew lives in /opt/homebrew on Apple Silicon and /usr/local on Intel.
  # Ask brew for the real prefix so headers/libs are found on both (and on
  # the arm64 GitHub Actions runners); fall back to /usr/local if brew is absent.
  BREW_PREFIX = $$system(brew --prefix 2>/dev/null)
  isEmpty(BREW_PREFIX): BREW_PREFIX = /usr/local

  INCLUDEPATH += $$BREW_PREFIX/include
  DEPENDPATH  += $$BREW_PREFIX/lib
  LIBS        += -L$$BREW_PREFIX/lib
  LIBS += -stdlib=libc++
  QMAKE_CXXFLAGS += -stdlib=libc++


  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
  # Do not pin QMAKE_MAC_SDK to a specific version (e.g. macosx10.13): that SDK
  # is not installed on modern runners and qmake aborts with
  # "Could not resolve SDK Path". Letting it default uses whatever SDK ships.
  # Build the toolchain's native arch (arm64 on Apple Silicon) rather than
  # forcing x86_64, which would mismatch the arm64 Homebrew libraries.
}

win32 {
  QT += xml xmlpatterns

  # Keep intermediate build artifacts (.o, moc_*, qrc_*, ui_*) out of the
  # packaged output: they go under obj/, while the final exe lands in
  # dist/<config>/ so the deploy+zip step bundles only runtime files.
  OBJECTS_DIR = obj
  MOC_DIR     = obj
  RCC_DIR     = obj
  UI_DIR      = obj
  CONFIG(debug, debug|release) {
    DESTDIR = dist/debug
  } else {
    DESTDIR = dist/release
  }

  msys2 {
    # MSYS2 MINGW64 (qmake CONFIG+=msys2): headers/libs live in the MINGW prefix,
    # which is already on the default gcc search path, and use unix-style lib names.
    LIBS += -lkeyfinder
    LIBS += -lavcodec
    LIBS += -lavformat
    LIBS += -lavutil
    LIBS += -lswresample
    LIBS += -ltag
    LIBS += -lz
  } else {
    # Legacy hand-rolled 32-bit MinGW prefix.
    INCLUDEPATH += C:/mingw32/local/include
    DEPENDPATH += C:/mingw32/local/bin
    LIBS += -LC:/mingw32/local/bin
    LIBS += -lkeyfinder0
    LIBS += -lavcodec
    LIBS += -lavformat
    LIBS += -lavutil
    LIBS += -lswresample
    LIBS += -llibtag
    LIBS += -LC:/mingw32/local/lib
  }
}

unix {
  QT += xml xmlpatterns
  target.path = $$[QT_INSTALL_PREFIX]/bin
  INSTALLS += target
}

TRANSLATIONS = \
  $$PWD/translations/is_keyfinder_en_GB.ts \
  $$PWD/translations/is_keyfinder_bg.ts \
  $$PWD/translations/is_keyfinder_da.ts \
  $$PWD/translations/is_keyfinder_de.ts \
  $$PWD/translations/is_keyfinder_el.ts \
  $$PWD/translations/is_keyfinder_en_US.ts \
  $$PWD/translations/is_keyfinder_es.ts \
  $$PWD/translations/is_keyfinder_fr.ts \
  $$PWD/translations/is_keyfinder_he.ts \
  $$PWD/translations/is_keyfinder_hr.ts \
  $$PWD/translations/is_keyfinder_it.ts \
  $$PWD/translations/is_keyfinder_nl.ts \
  $$PWD/translations/is_keyfinder_pl.ts \
  $$PWD/translations/is_keyfinder_pt_BR.ts \
  $$PWD/translations/is_keyfinder_pt_PT.ts \
  $$PWD/translations/is_keyfinder_ru.ts \
  $$PWD/translations/is_keyfinder_ru_RU.ts \
  $$PWD/translations/is_keyfinder_sv.ts \
  $$PWD/translations/is_keyfinder_tr.ts \
  $$PWD/translations/is_keyfinder_vi.ts \
  $$PWD/translations/is_keyfinder_zh_CN.ts
