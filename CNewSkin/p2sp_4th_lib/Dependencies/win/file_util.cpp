
#include "file_util.h"

#include <io.h>
#include <share.h>
#include <Shlwapi.h>
#include <atlbase.h>

FILE* OpenFile(const wchar_t* filename, const wchar_t* mode)
{
    return _wfsopen(filename, mode, _SH_DENYNO);
}

bool CloseFile(FILE* file)
{
    if(file == NULL)
    {
        return true;
    }
    return fclose(file) == 0;
}

bool TruncateFile(FILE* file)
{
    if(file == NULL)
    {
        return false;
    }
    long current_offset = ftell(file);
    if(current_offset == -1)
    {
        return false;
    }
    int fd = _fileno(file);
    if(_chsize(fd, current_offset) != 0)
    {
        return false;
    }
    return true;
}

// creates folder and all parts of the path if necessary (parent does not need
// to exists) and verifies that the contents of the folder will be visibile.
//
// in:
//      path                full path to create
//      sa                  security attributes
//
// returns:
//      ERROR_SUCCESS (0)   success
//      ERROR_              failure
int EnsureDirectory(const wchar_t* dir, SECURITY_ATTRIBUTES* sa)
{
    int ret = ERROR_SUCCESS;

    if(::PathIsRelativeW(dir))
    {
        // if not a "full" path bail
        // to ensure that we dont create a dir in the current working directory
        ::SetLastError(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME;
    }

    if(!::CreateDirectoryW(dir, sa)) 
    {
        wchar_t *end, *slash, temp[MAX_PATH+1]; // +1 for PathAddBackslash()

        ret = ::GetLastError();

        // There are certain error codes that we should bail out here
        // before going through and walking up the tree...
        switch(ret)
        {
        case ERROR_FILENAME_EXCED_RANGE:
        case ERROR_FILE_EXISTS:
        case ERROR_ALREADY_EXISTS:
            return ret;
        }

        lstrcpynW(temp, dir, ARRAYSIZE(temp)-1);
        end = ::PathAddBackslashW(temp); // for the loop below

        // assume we have 'X:\' to start this should even work
        // on UNC names because will will ignore the first error
        slash = temp + 3;

        // create each part of the dir in order
        while(*slash)
        {
            while(*slash && *slash!=L'\\')
            {
                slash = ::CharNextW(slash);
            }

            if(*slash) 
            {
                ATLASSERT(*slash == L'\\');

                *slash = 0; // terminate path at seperator

                ret = ::CreateDirectoryW(temp, slash+1==end?sa:NULL)
                    ? ERROR_SUCCESS : ::GetLastError();

            }
            *slash++ = L'\\'; // put the seperator back
        }
    }

    return ret;
}

int EnsureDirectoryForFile(const wchar_t* file_path, SECURITY_ATTRIBUTES* sa)
{
    wchar_t dir[MAX_PATH] = { 0 };
    lstrcpyW(dir, file_path);
    if(::PathRemoveFileSpecW(dir))
    {
        return EnsureDirectory(dir, sa);
    }

    ::SetLastError(ERROR_BAD_PATHNAME);
    return ERROR_BAD_PATHNAME;
}