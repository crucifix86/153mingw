
#ifndef __path_service_h__
#define __path_service_h__

#include <Windows.h>

class CFilePath
{
public:
    // A special path component meaning "this directory."
    static const wchar_t* kCurrentDirectory;

    // A special path component meaning "the parent directory."
    static const wchar_t* kParentDirectory;

    CFilePath();
    CFilePath(const CFilePath& that);
    explicit CFilePath(const wchar_t* path);
    ~CFilePath();
    CFilePath& operator=(const CFilePath& that);

    bool operator==(const CFilePath& that) const;
    bool operator!=(const CFilePath& that) const;
    bool operator<(const CFilePath& that) const;

    const wchar_t* value() const { return path_; }
    bool Empty() const;

    // 清除路径
    void Clear();
    // 路径小写
    void MakePretty();
    // 添加反斜线
    void AddBackslash();
    // 添加扩展名
    bool AddExtension(const wchar_t* ext);
    // 添加路径
    bool Append(const wchar_t* append);
    // 删除扩展名
    void RemoveExtension();
    // 修改扩展名
    bool RenameExtension(const wchar_t* ext);
    // 去掉文件名
    bool RemoveFileSpec();
    // 去掉路径, 只剩文件名
    void StripPath();
    // 只保留盘符
    bool StripToRoot();

	// 相当于.. 操作
	bool RemoveLastPart();

	const wchar_t* FindExtension() const;
    const wchar_t* FindFileName() const;

    // 文件或者目录是否存在
    bool IsExists() const;
    // 是否是目录
    bool IsDirectory() const;
    // 目录是否为空
    bool IsDirectoryEmpty() const;
    // 是否是.目录
    bool IsDotDirectory() const;
    // 是否是..目录
    bool IsDotDotDirectory() const;

    // 获取系统临时目录
    static bool GetTempDir(CFilePath& path);
    // 获取当前目录
    static bool GetCurrentDirectory(CFilePath& dir);

private:
    wchar_t path_[MAX_PATH];
};


enum
{
    PATH_START = 0,

    DIR_CURRENT,                // current directory
    DIR_EXE,                    // directory containing FILE_EXE
    DIR_MODULE,                 // directory containing FILE_MODULE
    DIR_TEMP,                   // temporary directory
    FILE_EXE,                   // Path and filename of the current executable.
    FILE_MODULE,                // Path and filename of the module containing the code for the
                                // PathService (which could differ from FILE_EXE if the
                                // PathService were compiled into a shared object, for example).

    PATH_END
};

enum
{
    PATH_WIN_START = 100,

    DIR_WINDOWS,                // Windows directory, usually "c:\windows"
    DIR_SYSTEM,                 // Usually c:\windows\system32"
    DIR_PROGRAM_FILES,          // Usually c:\program files

    DIR_IE_INTERNET_CACHE,      // Temporary Internet Files directory.
    DIR_COMMON_START_MENU,      // Usually "C:\Documents and Settings\All Users\
                                // Start Menu\Programs"
    DIR_START_MENU,             // Usually "C:\Documents and Settings\<user>\
                                // Start Menu\Programs"
    DIR_COMMON_APP_DATA,        // All Users\Application Data
    DIR_APP_DATA,               // Application Data directory under the user profile.
    DIR_PROFILE,                // Usually "C:\Documents and settings\<user>.
    DIR_LOCAL_APP_DATA_LOW,     // Local AppData directory for low integrity level.
    DIR_LOCAL_APP_DATA,         // "Local Settings\Application Data" directory under the
                                // user profile.
    PATH_WIN_END
};

bool PathProvider(int key, CFilePath& result);

#endif //__path_service_h__