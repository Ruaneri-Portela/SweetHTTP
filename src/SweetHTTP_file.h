#ifndef HTTP_FILE
#define HTTP_FILE 
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>
#include <windows.h>

typedef HANDLE HTTP_file;

#define HTTP_FILE_INVALID INVALID_HANDLE_VALUE
#define HTTP_FILE_INVALID_SIZE -1

HTTP_file HTTP_fileOpen(const wchar_t* filePath);

bool HTTP_fileClose(HTTP_file file);

bool HTTP_fileRead(HTTP_file file, int64_t size, int32_t* readed, char* destination);

bool HTTP_fileSeek(HTTP_file file, int64_t position);

int64_t HTTP_fileSize(HTTP_file file);

bool HTTP_fileIsDirectory(const wchar_t* path);

bool HTTP_fileIsFile(const wchar_t* path);
#endif