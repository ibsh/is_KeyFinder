/*************************************************************************

  Copyright 2011-2015 Ibrahim Sha'ath

  This file is part of KeyFinder.

  KeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  KeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with KeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "os_windows.h"

#ifdef Q_OS_WIN

char* utf16_to_utf8(const wchar_t* input) {
  char* buffer;
  int buffSize, result;
  buffSize = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
  buffer = (char*) malloc(sizeof(char) * buffSize);
  result = WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer, buffSize, NULL, NULL);
  return (result > 0 && result <= buffSize) ? buffer : NULL;
}

#else

char* utf16_to_utf8(const wchar_t* /*input*/) {
  return NULL;
}

#endif
