#include "SweetHTTP_file.h"

HTTP_file HTTP_fileOpen(const wchar_t* filePath) {
    return CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool HTTP_fileClose(HTTP_file file) {
    return CloseHandle(file);
}

bool HTTP_fileRead(HTTP_file file, int64_t size, int32_t* readed, char* destination) {
    return ReadFile(file, destination, size, (LPDWORD)readed, NULL);
}

bool HTTP_fileSeek(HTTP_file file, int64_t position) {
    LARGE_INTEGER liOffset;
    liOffset.QuadPart = position;
    return SetFilePointerEx(file, liOffset, NULL, FILE_BEGIN);
}

int64_t HTTP_fileSize(HTTP_file file) {
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize))
        return -1;
    return fileSize.QuadPart;
}

bool HTTP_fileIsDirectory(const wchar_t* path) {
    DWORD dwAttrib = GetFileAttributesW(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool HTTP_fileIsFile(const wchar_t* path) {
    DWORD dwAttrib = GetFileAttributesW(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}