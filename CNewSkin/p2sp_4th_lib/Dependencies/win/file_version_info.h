
#ifndef __file_version_info_h__
#define __file_version_info_h__

#include <string>
#include <Windows.h>

class CFilePath;

// Provides a way to access the version information for a file.
// This is the information you access when you select a file in the Windows
// explorer, right-click select Properties, then click the Version tab.
class CFileVersionInfo
{
public:
    ~CFileVersionInfo();

    // Creates a FileVersionInfo for the specified path. Returns NULL if something
    // goes wrong (typically the file does not exit or cannot be opened). The
    // returned object should be deleted when you are done with it.
    static CFileVersionInfo* CreateFileVersionInfo(const CFilePath& file_path);

    // Creates a FileVersionInfo for the current module. Returns NULL in case
    // of error. The returned object should be deleted when you are done with it.
    static CFileVersionInfo* CreateFileVersionInfoForCurrentModule();

    // Accessors to the different version properties.
    // Returns an empty string if the property is not found.
    virtual std::wstring company_name();
    virtual std::wstring company_short_name();
    virtual std::wstring product_name();
    virtual std::wstring product_short_name();
    virtual std::wstring internal_name();
    virtual std::wstring product_version();
    virtual std::wstring private_build();
    virtual std::wstring special_build();
    virtual std::wstring comments();
    virtual std::wstring original_filename();
    virtual std::wstring file_description();
    virtual std::wstring file_version();
    virtual std::wstring legal_copyright();
    virtual std::wstring legal_trademarks();
    virtual std::wstring last_change();
    virtual bool is_official_build();

    // Lets you access other properties not covered above.
    bool GetValue(const wchar_t* name, std::wstring& value);

    // Similar to GetValue but returns a wstring (empty string if the property
    // does not exist).
    std::wstring GetStringValue(const wchar_t* name);

    // Get the fixed file info if it exists. Otherwise NULL
    VS_FIXEDFILEINFO* fixed_file_info() { return fixed_file_info_; }

private:
    CFileVersionInfo(void* data, int language, int code_page);

    CFileVersionInfo(const CFileVersionInfo&);
    void operator=(const CFileVersionInfo&);

    void* data_;
    int language_;
    int code_page_;
    // This is a pointer into the data_ if it exists. Otherwise NULL.
    VS_FIXEDFILEINFO* fixed_file_info_;
};

#endif //__file_version_info_h__