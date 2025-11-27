
#include "registry.h"

#include <atlbase.h>

CRegistryValueIterator::CRegistryValueIterator(HKEY root_key,
                                               const wchar_t* folder_key)
{
    LONG result = ::RegOpenKeyExW(root_key, folder_key, 0, KEY_READ, &key_);
    if(result != ERROR_SUCCESS)
    {
        key_ = NULL;
    }
    else
    {
        DWORD count = 0;
        result = ::RegQueryInfoKeyW(key_, NULL, 0, NULL, NULL, NULL,
            NULL, &count, NULL, NULL, NULL, NULL);

        if(result != ERROR_SUCCESS)
        {
            ::RegCloseKey(key_);
            key_ = NULL;
        }
        else
        {
            index_ = count - 1;
        }
    }

    Read();
}

CRegistryValueIterator::~CRegistryValueIterator()
{
    if(key_)
    {
        ::RegCloseKey(key_);
        key_ = NULL;
    }
}

bool CRegistryValueIterator::Valid() const
{
    return key_!=NULL && index_>=0;
}

void CRegistryValueIterator::operator++()
{
    --index_;
    Read();
}

bool CRegistryValueIterator::Read()
{
    if(Valid())
    {
        DWORD ncount = sizeof(name_) / sizeof(name_[0]);
        value_size_ = sizeof(value_);
        LRESULT r = ::RegEnumValueW(key_, index_, name_, &ncount,
            NULL, &type_, reinterpret_cast<BYTE*>(value_), &value_size_);
        if(ERROR_SUCCESS == r)
        {
            return true;
        }
    }

    name_[0] = '\0';
    value_[0] = '\0';
    value_size_ = 0;
    return false;
}

DWORD CRegistryValueIterator::ValueCount() const
{
    DWORD count = 0;
    HRESULT result = ::RegQueryInfoKeyW(key_, NULL, 0, NULL, NULL, NULL,
        NULL, &count, NULL, NULL, NULL, NULL);

    if(result != ERROR_SUCCESS)
    {
        return 0;
    }

    return count;
}

CRegistryKeyIterator::CRegistryKeyIterator(HKEY root_key,
                                           const wchar_t* folder_key)
{
    LONG result = ::RegOpenKeyExW(root_key, folder_key, 0, KEY_READ, &key_);
    if(result != ERROR_SUCCESS)
    {
        key_ = NULL;
    }
    else
    {
        DWORD count = 0;
        HRESULT result = ::RegQueryInfoKeyW(key_, NULL, 0, NULL, &count,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        if(result != ERROR_SUCCESS)
        {
            ::RegCloseKey(key_);
            key_ = NULL;
        }
        else
        {
            index_ = count - 1;
        }
    }

    Read();
}

CRegistryKeyIterator::~CRegistryKeyIterator()
{
    if(key_)
    {
        ::RegCloseKey(key_);
    }
}

bool CRegistryKeyIterator::Valid() const
{
    return key_!=NULL && index_>=0;
}

void CRegistryKeyIterator::operator++()
{
    --index_;
    Read();
}

bool CRegistryKeyIterator::Read()
{
    if(Valid())
    {
        DWORD ncount = sizeof(name_) / sizeof(name_[0]);
        FILETIME written;
        LRESULT r = ::RegEnumKeyExW(key_, index_, name_, &ncount,
            NULL, NULL, NULL, &written);
        if(ERROR_SUCCESS == r)
        {
            return true;
        }
    }

    name_[0] = '\0';
    return false;
}

DWORD CRegistryKeyIterator::SubkeyCount() const
{
    DWORD count = 0;
    HRESULT result = ::RegQueryInfoKeyW(key_, NULL, 0, NULL, &count,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    if(result != ERROR_SUCCESS)
    {
        return 0;
    }

    return count;
}

CRegistryKey::CRegistryKey() : key_(NULL), watch_event_(0) {}

CRegistryKey::CRegistryKey(HKEY rootkey, const wchar_t* subkey, REGSAM access)
: key_(NULL), watch_event_(0)
{
    if(rootkey)
    {
        if(access & (KEY_SET_VALUE|KEY_CREATE_SUB_KEY|KEY_CREATE_LINK))
        {
            Create(rootkey, subkey, access);
        }
        else
        {
            Open(rootkey, subkey, access);
        }
    }
    else
    {
        ATLASSERT(!subkey);
    }
}

CRegistryKey::~CRegistryKey()
{
    Close();
}

void CRegistryKey::Close()
{
    StopWatching();
    if(key_)
    {
        ::RegCloseKey(key_);
        key_ = NULL;
    }
}

bool CRegistryKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    DWORD disposition_value;
    return CreateWithDisposition(rootkey, subkey, disposition_value, access);
}

bool CRegistryKey::CreateWithDisposition(HKEY rootkey, const wchar_t* subkey,
                                    DWORD& disposition, REGSAM access)
{
    ATLASSERT(rootkey && subkey && access);
    Close();

    LONG result = ::RegCreateKeyExW(rootkey, subkey, 0, NULL,
        REG_OPTION_NON_VOLATILE, access, NULL, &key_, &disposition);
    if(result != ERROR_SUCCESS)
    {
        key_ = NULL;
        return false;
    }

    return true;
}

bool CRegistryKey::Open(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    ATLASSERT(rootkey && subkey && access);
    Close();

    LONG result = ::RegOpenKeyExW(rootkey, subkey, 0, access, &key_);
    if(result != ERROR_SUCCESS)
    {
        key_ = NULL;
        return false;
    }
    return true;
}

bool CRegistryKey::CreateKey(const wchar_t* name, REGSAM access)
{
    ATLASSERT(name && access);

    HKEY subkey = NULL;
    LONG result = ::RegCreateKeyExW(key_, name, 0, NULL,
        REG_OPTION_NON_VOLATILE, access, NULL, &subkey, NULL);
    Close();

    key_ = subkey;
    return (result == ERROR_SUCCESS);
}

bool CRegistryKey::OpenKey(const wchar_t* name, REGSAM access)
{
    ATLASSERT(name && access);

    HKEY subkey = NULL;
    LONG result = ::RegOpenKeyExW(key_, name, 0, access, &subkey);

    Close();

    key_ = subkey;
    return (result == ERROR_SUCCESS);
}

DWORD CRegistryKey::ValueCount()
{
    DWORD count = 0;
    HRESULT result = ::RegQueryInfoKeyW(key_, NULL, 0, NULL, NULL,
        NULL, NULL, &count, NULL, NULL, NULL, NULL);
    return (result!=ERROR_SUCCESS) ? 0 : count;
}

bool CRegistryKey::ReadName(int index, std::wstring& name)
{
    wchar_t buf[256];
    DWORD bufsize = sizeof(buf) / sizeof(buf[0]);
    LRESULT r = ::RegEnumValueW(key_, index, buf, &bufsize, NULL,
        NULL, NULL, NULL);
    if(r != ERROR_SUCCESS)
    {
        return false;
    }
    name = buf;
    return true;
}

bool CRegistryKey::ValueExists(const wchar_t* name)
{
    if(!key_)
    {
        return false;
    }
    HRESULT result = ::RegQueryValueExW(key_, name, 0, NULL, NULL, NULL);
    return (result == ERROR_SUCCESS);
}

bool CRegistryKey::ReadValue(const wchar_t* name, void* data,
                        DWORD* dsize, DWORD* dtype)
{
    if(!key_)
    {
        return false;
    }
    HRESULT result = ::RegQueryValueExW(key_, name, 0, dtype,
        reinterpret_cast<LPBYTE>(data), dsize);
    return (result == ERROR_SUCCESS);
}

bool CRegistryKey::ReadValue(const wchar_t* name, std::wstring& value)
{
    const size_t kMaxStringLength = 1024; // This is after expansion.
    // Use the one of the other forms of ReadValue if 1024 is too small for you.
    wchar_t raw_value[kMaxStringLength];
    DWORD type = REG_SZ, size = sizeof(raw_value);
    if(ReadValue(name, raw_value, &size, &type))
    {
        if(type == REG_SZ)
        {
            value = raw_value;
        }
        else if(type == REG_EXPAND_SZ)
        {
            wchar_t expanded[kMaxStringLength];
            size = ::ExpandEnvironmentStringsW(raw_value, expanded, kMaxStringLength);
            // Success: returns the number of wchar_t's copied
            // Fail: buffer too small, returns the size required
            // Fail: other, returns 0
            if(size==0 || size>kMaxStringLength)
            {
                return false;
            }
            value = expanded;
        }
        else
        {
            // Not a string. Oops.
            return false;
        }
        return true;
    }

    return false;
}

bool CRegistryKey::ReadValueDW(const wchar_t* name, DWORD& value)
{
    DWORD type = REG_DWORD;
    DWORD size = sizeof(DWORD);
    DWORD result = 0;
    if(ReadValue(name, &result, &size, &type) &&
        (type==REG_DWORD || type==REG_BINARY) && size == sizeof(DWORD))
    {
        value = result;
        return true;
    }

    return false;
}

bool CRegistryKey::WriteValue(const wchar_t* name, const void * data,
                         DWORD dsize, DWORD dtype)
{
    ATLASSERT(data);

    if(!key_)
    {
        return false;
    }

    HRESULT result = ::RegSetValueExW(key_, name, 0, dtype,
        reinterpret_cast<LPBYTE>(const_cast<void*>(data)), dsize);
    return (result == ERROR_SUCCESS);
}

bool CRegistryKey::WriteValue(const wchar_t* name, const wchar_t* value)
{
    return WriteValue(name, value,
        static_cast<DWORD>(sizeof(*value)*(wcslen(value)+1)), REG_SZ);
}

bool CRegistryKey::WriteValue(const wchar_t* name, DWORD value)
{
    return WriteValue(name, &value, static_cast<DWORD>(sizeof(value)),
        REG_DWORD);
}

bool CRegistryKey::DeleteKey(const wchar_t* name)
{
    return (!key_) ? false : (ERROR_SUCCESS==::SHDeleteKeyW(key_, name));
}

bool CRegistryKey::DeleteValue(const wchar_t* value_name)
{
    ATLASSERT(value_name);
    HRESULT result = ::RegDeleteValueW(key_, value_name);
    return (result == ERROR_SUCCESS);
}

bool CRegistryKey::StartWatching()
{
    if(!watch_event_)
    {
        watch_event_ = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }

    DWORD filter = REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
        REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY;

    // Watch the registry key for a change of value.
    HRESULT result = ::RegNotifyChangeKeyValue(key_, TRUE, filter,
        watch_event_, TRUE);
    if(SUCCEEDED(result))
    {
        return true;
    }
    else
    {
        ::CloseHandle(watch_event_);
        watch_event_ = 0;
        return false;
    }
}

bool CRegistryKey::StopWatching()
{
    if(watch_event_)
    {
        ::CloseHandle(watch_event_);
        watch_event_ = 0;
        return true;
    }
    return false;
}

bool CRegistryKey::HasChanged()
{
    if(watch_event_)
    {
        if(::WaitForSingleObject(watch_event_, 0) == WAIT_OBJECT_0)
        {
            StartWatching();
            return true;
        }
    }
    return false;
}