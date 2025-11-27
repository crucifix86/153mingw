
#ifndef __file_util_h__
#define __file_util_h__

#include <stdio.h>
#include <Windows.h>

// Wrapper for fopen-like calls. Returns non-NULL FILE* on success.
FILE* OpenFile(const wchar_t* filename, const wchar_t* mode);

// Closes file opened by OpenFile. Returns true on success.
bool CloseFile(FILE* file);

// Truncates an open file to end at the location of the current file pointer.
// This is a cross-platform analog to Windows' SetEndOfFile() function.
bool TruncateFile(FILE* file);

// creates folder and all parts of the path if necessary (parent does not need
// to exists) and verifies that the contents of the folder will be visibile.
int EnsureDirectory(const wchar_t* dir, SECURITY_ATTRIBUTES* sa);
int EnsureDirectoryForFile(const wchar_t* file_path, SECURITY_ATTRIBUTES* sa);

#endif //__file_util_h__