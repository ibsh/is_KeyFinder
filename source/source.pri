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

HEADERS  += \
  $$PWD/_VERSION.h \
  $$PWD/asyncfileobject.h \
  $$PWD/asynckeyprocess.h \
  $$PWD/asynckeyresult.h \
  $$PWD/asyncmetadatareadprocess.h \
  $$PWD/asyncmetadatareadresult.h \
  $$PWD/avfilemetadata.h \
  $$PWD/avfilemetadatafactory.h \
  $$PWD/decoderlibav.h \
  $$PWD/guiabout.h \
  $$PWD/guibatch.h \
  $$PWD/guimenuhandler.h \
  $$PWD/guiprefs.h \
  $$PWD/metadatafilename.h \
  $$PWD/metadatawriteresult.h \
  $$PWD/os_windows.h \
  $$PWD/preferences.h \
  $$PWD/settingswrapper.h \
  $$PWD/strings.h \
    $$PWD/externalplaylistprovider.h \
    $$PWD/externalplaylistproviderserato.h

SOURCES += \
  $$PWD/asynckeyprocess.cpp \
  $$PWD/asyncmetadatareadprocess.cpp \
  $$PWD/avfilemetadata.cpp \
  $$PWD/avfilemetadatafactory.cpp \
  $$PWD/decoderlibav.cpp \
  $$PWD/guiabout.cpp \
  $$PWD/guibatch.cpp \
  $$PWD/guimenuhandler.cpp \
  $$PWD/guiprefs.cpp \
  $$PWD/metadatafilename.cpp \
  $$PWD/os_windows.cpp \
  $$PWD/preferences.cpp \
  $$PWD/settingswrapper.cpp \
  $$PWD/strings.cpp \
    $$PWD/externalplaylistprovider.cpp \
    $$PWD/externalplaylistproviderserato.cpp
