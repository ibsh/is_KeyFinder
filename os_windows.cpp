#include "os_windows.h"

char* utf16_to_utf8(const wchar_t* input){
  char* buffer;
  int buffSize, result;
  buffSize = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
  buffer = (char*) malloc(sizeof(char) * buffSize);
  result = WideCharToMultiByte(CP_UTF8, 0, input, -1, buffer, buffSize, NULL, NULL);
  return (result > 0 && result <= buffSize) ? buffer : NULL;
}
