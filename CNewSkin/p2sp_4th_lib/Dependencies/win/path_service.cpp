
#include "path_service.h"

#include "window_util.h"

#include <Shlwapi.h>
#include <ShlObj.h>

const wchar_t* CFilePath::kCurrentDirectory = L".";
const wchar_t* CFilePath::kParentDirectory = L"..";

static const CFilePath kCurrentDirectoryPath(CFilePath::kCurrentDirectory);
static const CFilePath kParentDirectoryPath(CFilePath::kParentDirectory);

CFilePath::CFilePath()
{
    path_[0] = '\0';
}

CFilePath::CFilePath(const CFilePath& that)
{
    lstrcpyW(path_, that.path_);
}

CFilePath::CFilePath(const wchar_t* path)
{
    lstrcpyW(path_, path);
}

CFilePath::~CFilePath() {}

CFilePath& CFilePath::operator=(const CFilePath& that)
{
    lstrcpyW(path_, that.path_);
    return *this;
}

bool CFilePath::operator==(const CFilePath& that) const
{
    return lstrcmpiW(path_, that.path_) == 0;
}

bool CFilePath::operator!=(const CFilePath& that) const
{
    return lstrcmpiW(path_, that.path_) != 0;
}

bool CFilePath::operator<(const CFilePath& that) const
{
    return lstrcmpiW(path_, that.path_) <= 0;
}

bool CFilePath::Empty() const
{
    return lstrlenW(path_) == 0;
}

void CFilePath::Clear()
{
    path_[0] = '\0';
}

void CFilePath::MakePretty()
{
    ::PathMakePrettyW(path_);
}

void CFilePath::AddBackslash()
{
    ::PathAddBackslashW(path_);
}

bool CFilePath::AddExtension(const wchar_t* ext)
{
    return !!::PathAddExtensionW(path_, ext);
}

bool CFilePath::Append(const wchar_t* append)
{
    return !!::PathAppendW(path_, append);
}

void CFilePath::RemoveExtension()
{
    ::PathRemoveExtensionW(path_);
}

bool CFilePath::RenameExtension(const wchar_t* ext)
{
    return !!::PathRenameExtensionW(path_, ext);
}

bool CFilePath::RemoveFileSpec()
{
    return !!::PathRemoveFileSpecW(path_);
}

bool CFilePath::RemoveLastPart()
{
	return ::PathRemoveBackslashW(path_) && ::PathRemoveFileSpecW(path_);
}

void CFilePath::StripPath()
{
    ::PathStripPathW(path_);
}

bool CFilePath::StripToRoot()
{
    return !!::PathStripToRootW(path_);
}

const wchar_t* CFilePath::FindExtension() const
{
    return ::PathFindExtensionW(path_);
}

const wchar_t* CFilePath::FindFileName() const
{
    return ::PathFindFileNameW(path_);
}

bool CFilePath::IsExists() const
{
    return !!::PathFileExistsW(path_);
}

bool CFilePath::IsDirectory() const
{
    return !!::PathIsDirectoryW(path_);
}

bool CFilePath::IsDirectoryEmpty() const
{
    return !!PathIsDirectoryEmptyW(path_);
}

bool CFilePath::IsDotDirectory() const
{
    return *this == kCurrentDirectoryPath;
}

bool CFilePath::IsDotDotDirectory() const
{
    return *this == kParentDirectoryPath;
}

bool CFilePath::GetTempDir(CFilePath& path)
{
    wchar_t temp_path[MAX_PATH] = { 0 };
    DWORD path_len = ::GetTempPathW(MAX_PATH, temp_path);
    if(path_len>=MAX_PATH || path_len<=0)
    {
        return false;
    }
    path = CFilePath(temp_path);
    return true;
}

bool CFilePath::GetCurrentDirectory(CFilePath& dir)
{
    wchar_t system_buffer[MAX_PATH] = { 0 };
    DWORD len = ::GetCurrentDirectoryW(MAX_PATH, system_buffer);
    if(len==0 || len>MAX_PATH)
    {
        return false;
    }

    dir = CFilePath(system_buffer);
    return true;
}


// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;
bool PathProvider(int key, CFilePath& result)
{
    // We need to go compute the value. It would be nice to support paths with
    // names longer than MAX_PATH, but the system functions don't seem to be
    // designed for it either, with the exception of GetTempPath (but other
    // things will surely break if the temp path is too long, so we don't bother
    // handling it.
    wchar_t system_buffer[MAX_PATH] = { 0 };
    system_buffer[0] = 0;

    CFilePath cur;
    switch(key)
    {
    case DIR_CURRENT:
        if(!CFilePath::GetCurrentDirectory(cur))
        {
            return false;
        }
        break;
    case DIR_EXE:
        ::GetModuleFileNameW(NULL, system_buffer, MAX_PATH);
        cur = CFilePath(system_buffer);
        cur.RemoveFileSpec();
        break;
    case DIR_MODULE:
        {
            // the resource containing module is assumed to be the one that
            // this code lives in, whether that's a dll or exe
            HMODULE this_module = reinterpret_cast<HMODULE>(&__ImageBase);
            ::GetModuleFileNameW(this_module, system_buffer, MAX_PATH);
            cur = CFilePath(system_buffer);
            cur.RemoveFileSpec();
            break;
        }
    case DIR_TEMP:
        if(!CFilePath::GetTempDir(cur))
        {
            return false;
        }
        break;
    case FILE_EXE:
        ::GetModuleFileNameW(NULL, system_buffer, MAX_PATH);
        cur = CFilePath(system_buffer);
        break;
    case FILE_MODULE:
        {
            // the resource containing module is assumed to be the one that
            // this code lives in, whether that's a dll or exe
            HMODULE this_module = reinterpret_cast<HMODULE>(&__ImageBase);
            ::GetModuleFileNameW(this_module, system_buffer, MAX_PATH);
            cur = CFilePath(system_buffer);
            break;
        }
    case DIR_WINDOWS:
        ::GetWindowsDirectoryW(system_buffer, MAX_PATH);
        cur = CFilePath(system_buffer);
        break;
    case DIR_SYSTEM:
        ::GetSystemDirectoryW(system_buffer, MAX_PATH);
        cur = CFilePath(system_buffer);
        break;
    case DIR_PROGRAM_FILES:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_IE_INTERNET_CACHE:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_INTERNET_CACHE, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_COMMON_START_MENU:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_START_MENU:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_COMMON_APP_DATA:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_APP_DATA:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_PROFILE:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    case DIR_LOCAL_APP_DATA_LOW:
        if(GetWinVersion() < WINVERSION_VISTA)
        {
            return false;
        }
        // TODO(nsylvain): We should use SHGetKnownFolderPath instead. Bug 1281128
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        cur.RemoveFileSpec();
        cur.Append(L"LocalLow");
        break;
    case DIR_LOCAL_APP_DATA:
        if(FAILED(::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL,
            SHGFP_TYPE_CURRENT, system_buffer)))
        {
            return false;
        }
        cur = CFilePath(system_buffer);
        break;
    default:
        return false;
    }

    result = cur;
    return true;
}