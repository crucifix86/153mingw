
#include "string_conversion.h"
#include "string_util.h"

#include <assert.h>
#include <Windows.h>

std::wstring UTF8ToWide(const std::string& utf8)
{
    if(utf8.length() == 0)
    {
        return std::wstring();
    }

    // compute the length of the buffer we'll need
    int charcount = ::MultiByteToWideChar(CP_UTF8, 0,
        utf8.c_str(), -1, NULL, 0);

    if(charcount == 0)
    {
        return std::wstring();
    }

    // convert
    wchar_t* buf = new wchar_t[charcount];
    ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
        -1, buf, charcount);
    std::wstring result(buf);
    delete[] buf;
    return result;
}

std::string WideToUTF8(const std::wstring& wide)
{
    if(wide.length() == 0)
    {
        return std::string();
    }

    // compute the length of the buffer we'll need
    int charcount = ::WideCharToMultiByte(CP_UTF8, 0,
        wide.c_str(), -1, NULL, 0, NULL, NULL);
    if(charcount == 0)
    {
        return std::string();
    }

    // convert
    char* buf = new char[charcount];
    ::WideCharToMultiByte(CP_UTF8, 0, wide.c_str(),
        -1, buf, charcount, NULL, NULL);

    std::string result(buf);
    delete[] buf;
    return result;
}

namespace {
	template<class STR>
	static bool DoIsStringASCII(const STR& str)
	{
		for(size_t i=0; i<str.length(); ++i)
		{
			typename ToUnsigned<typename STR::value_type>::Unsigned c = str[i];
			if(c > 0x7F)
			{
				return false;
			}
		}
		return true;
	}

	bool IsStringASCII(const std::wstring& str)
	{
		return DoIsStringASCII(str);
	}

	bool IsStringASCII(const std::string& str)
	{
		return DoIsStringASCII(str);
	}
}

std::wstring ASCIIToWide(const std::string& ascii)
{
	assert(IsStringASCII(ascii));
	return std::wstring(ascii.begin(), ascii.end());
}

std::string WideToASCII(const std::wstring& wide)
{
	assert(IsStringASCII(wide));
	return std::string(wide.begin(), wide.end());
}