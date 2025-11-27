
#include "fagexStringUtil.h"

namespace fagex
{
	#pragma warning (disable : 4996)
	#pragma warning (disable : 4244)

	const String StringUtil::BLANK;
	const String StringUtil::ENTER = T_STRING("\r\n");

	//---------------------------------------------------------------------------------------
	bool StringUtil::replace(String& src, const String& old, const String& now)
	{
		size_t num = old.length();
		if(num == 0) return false;

		size_t index = src.find(old, 0);
		while(index != String::npos)
		{
			src.replace(index, num, now);
			index = index + now.length();
			index = src.find(old,index);
		}
		return true;
	}

	//---------------------------------------------------------------------------------------
	bool StringUtil::replaceOf(String& src, const String& old, const String& now)
	{
		if(old.length() == 0) return false;

		size_t index = src.find_first_of(old, 0);
		while(index != String::npos)
		{
			src.replace(index, 1, now);
			index = index + now.length();
			index = src.find_first_of(old,index);
		}
		return true;
	}

	//---------------------------------------------------------------------------------------
	String StringUtil::getMatchString(const String& src, const String& start, const String& end)
	{
		size_t start_pos = src.find(start, 0);
		if(start_pos == String::npos)
			return StringUtil::BLANK;

		size_t end_pos = src.find(end, start_pos + start.length());
		if(end_pos == String::npos)
			return StringUtil::BLANK;

		return src.substr(start_pos, end_pos + end.length() - start_pos);
	}

	//---------------------------------------------------------------------------------------
	std::wstring StringUtil::getMatchStringW(const std::wstring& src, const std::wstring& start, const std::wstring& end)
	{
		size_t start_pos = src.find(start, 0);
		if(start_pos == std::wstring::npos)
			return L"";

		size_t end_pos = src.find(end, start_pos + start.length());
		if(end_pos == std::wstring::npos)
			return L"";

		return src.substr(start_pos, end_pos + end.length() - start_pos);
	}

	//---------------------------------------------------------------------------------------
	String StringUtil::getValueFormString(const String& src, const String& start, const String& end)
	{
		size_t start_pos = src.find(start, 0);
		if(start_pos == String::npos)
			return StringUtil::BLANK;

		size_t end_pos = src.find(end, start_pos + start.length());
		if(end_pos == String::npos)
			return StringUtil::BLANK;

		return src.substr(start_pos + start.length(), end_pos - start_pos - start.length());
	}		

	//---------------------------------------------------------------------------------------
	std::wstring StringUtil::getValueFormStringW(const std::wstring& src, const std::wstring& start, const std::wstring& end)
	{
		size_t start_pos = src.find(start, 0);
		if(start_pos == std::wstring::npos)
			return L"";

		size_t end_pos = src.find(end, start_pos + start.length());
		if(end_pos == std::wstring::npos)
			return L"";

		return src.substr(start_pos + start.length(), end_pos - start_pos - start.length());
	}

	//---------------------------------------------------------------------------------------
	bool StringUtil::eraseMatchString(String& src, const String& start, const String& end, unsigned count)
	{
		if(start.length() == 0 || end.length() == 0) return false;

		size_t istart = 0;
		size_t iend = 0;
		size_t times = 0;
		while(true)
		{
			istart = src.find(start);
			if(istart == String::npos) break;
			iend = src.find(end,istart);
			if(iend == String::npos) break;
			src.erase(istart, iend + end.length() - istart);

			if(count == 0) continue;
			if(++times >= count) break;
		}
		return true;
	}

	//---------------------------------------------------------------------------------------
	std::string StringUtil::EncodeComponent(const std::string& src)
	{
		std::string ret;
		std::string::const_iterator it = src.begin();
		for(; it != src.end(); ++it)
		{
			char tmp = (char)(*it);
			char szString[3] = {0};
			sprintf_s(szString, 3, "%02x", tmp & 0xff);
			ret += szString;
		}
		return ret;
	}

	//---------------------------------------------------------------------------------------
	std::string StringUtil::EncodeURIComponent(const std::string& src)
	{
		char szString[4] = {0};
		char tmp;

		std::string ret;
		std::string::const_iterator it = src.begin();
		for(; it != src.end(); ++it)
		{
			tmp = (char)(*it);
			if(tmp == ' ' || tmp > 127)
			{
				sprintf_s(szString, 4, "%%%02x", tmp);
				ret += szString;
			}
			else
			{
				ret += tmp;
			}
		}
		return ret;
	}

	//-----------------------------------------------------------------------
	StringVector StringUtil::split(const String& str, const String& delims, unsigned int maxSplits)
	{
		StringVector ret;
		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

		unsigned int numSplits = 0;

		// Use STL methods 
		size_t start, pos;
		start = 0;
		do 
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back( str.substr(start) );
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back( str.substr(start, pos - start) );
				start = pos + 1;
			}
			// parse up to next real data
			start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != String::npos);

		return ret;
	}

	//-----------------------------------------------------------------------
	void StringUtil::trim(String& str, bool left, bool right)
	{
		static const String delims = T_STRING(" \t\r");
		if(right)
			str.erase(str.find_last_not_of(delims)+1); // trim right
		if(left)
			str.erase(0, str.find_first_not_of(delims)); // trim left
	}

	void StringUtil::Trim(String& str, const String& strtrim,bool left, bool right)
	{
		const String sdelims = strtrim;
		if(right)
			str.erase(str.find_last_not_of(sdelims)+1); // trim right
		if(left)
			str.erase(0, str.find_first_not_of(sdelims)); // trim left
	}

	//-----------------------------------------------------------------------
	void StringUtil::toLowerCase(String& str)
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			tolower);
	}

	//-----------------------------------------------------------------------
	void StringUtil::toUpperCase(String& str) 
	{
		std::transform(
			str.begin(),
			str.end(),
			str.begin(),
			toupper);
	}
	//-----------------------------------------------------------------------
	bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String startOfThis = str.substr(0, patternLen);
		if (lowerCase)
			StringUtil::toLowerCase(startOfThis);

		return (startOfThis == pattern);
	}

	//-----------------------------------------------------------------------
	bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String endOfThis = str.substr(thisLen - patternLen, patternLen);
		if (lowerCase)
			StringUtil::toLowerCase(endOfThis);

		return (endOfThis == pattern);
	}
	//-----------------------------------------------------------------------
	String StringUtil::standardisePath(const String& init)
	{
		String path = init;

		std::replace( path.begin(), path.end(), '\\', '/' );
		if( path[path.length() - 1] != '/' )
			path += '/';

		return path;
	}
	//-----------------------------------------------------------------------
	void StringUtil::splitFilename(const String& qualifiedName, 
		String& outBasename, String& outPath)
	{
		String path = qualifiedName;
		// Replace \ with / first
		std::replace( path.begin(), path.end(), '\\', '/' );
		// split based on final /
		size_t i = path.find_last_of('/');

		if (i == String::npos)
		{
			outPath.clear();
			outBasename = qualifiedName;
		}
		else
		{
			outBasename = path.substr(i+1, path.size() - i - 1);
			outPath = path.substr(0, i+1);
		}

	}
	//-----------------------------------------------------------------------
	bool StringUtil::match(const String& str, const String& pattern, bool caseSensitive)
	{
		String tmpStr = str;
		String tmpPattern = pattern;
		if (!caseSensitive)
		{
			StringUtil::toLowerCase(tmpStr);
			StringUtil::toLowerCase(tmpPattern);
		}

		String::const_iterator strIt = tmpStr.begin();
		String::const_iterator patIt = tmpPattern.begin();
		String::const_iterator lastWildCardIt = tmpPattern.end();
		while (strIt != tmpStr.end() && patIt != tmpPattern.end())
		{
			if (*patIt == '*')
			{
				lastWildCardIt = patIt;
				// Skip over looking for next character
				++patIt;
				if (patIt == tmpPattern.end())
				{
					// Skip right to the end since * matches the entire rest of the string
					strIt = tmpStr.end();
				}
				else
				{
					// scan until we find next pattern character
					while(strIt != tmpStr.end() && *strIt != *patIt)
						++strIt;
				}
			}
			else
			{
				if (*patIt != *strIt)
				{
					if (lastWildCardIt != tmpPattern.end())
					{
						// The last wildcard can match this incorrect sequence
						// rewind pattern to wildcard and keep searching
						patIt = lastWildCardIt;
						lastWildCardIt = tmpPattern.end();
					}
					else
					{
						// no wildwards left
						return false;
					}
				}
				else
				{
					++patIt;
					++strIt;
				}
			}

		}
		// If we reached the end of both the pattern and the string, we succeeded
		if (patIt == tmpPattern.end() && strIt == tmpStr.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//-----------------------------------------------------------------------
	//std::ios::fixed
	String StringUtil::toString(Real val, unsigned short precision, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags)
	{
		StringUtil::StrStreamType stream;
		stream.precision(precision);
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringUtil::toString(int val, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags)
	{
		StringUtil::StrStreamType stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String StringUtil::toString(size_t val, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags)
	{
		StringUtil::StrStreamType stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String StringUtil::toString(unsigned long val, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags)
	{
		StringUtil::StrStreamType stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String StringUtil::toString(long val, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags)
	{
		StringUtil::StrStreamType stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String StringUtil::toString(unsigned long long val, 
		unsigned short width, T_CHAR fill, std::ios::fmtflags flags )
	{
		StringUtil::StrStreamType stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	//-----------------------------------------------------------------------
	String StringUtil::toString(const StringVector& val)
	{
		StringUtil::StrStreamType stream;
		StringVector::const_iterator i, iend, ibegin;
		ibegin = val.begin();
		iend = val.end();
		for (i = ibegin; i != iend; ++i)
		{
			if (i != ibegin)
				stream << T_STRING(" ");

			stream << *i; 
		}
		return stream.str();
	}

	//-----------------------------------------------------------------------
	String StringUtil::toString(bool val, bool yesNo)
	{
		if(yesNo)
		{
			if(val)
				return T_STRING("yes");
			else
				return T_STRING("no");
		}
		else
		{
			if(val)
				return T_STRING("true");
			else
				return T_STRING("false");
		}
	}

	//-----------------------------------------------------------------------
	Real StringUtil::parseReal(const String& val)
	{
#ifdef USE_UNICODE
		//return _tstof(val.c_str());
		return _wtof(val.c_str());
#else
		return atof(val.c_str());
#endif
	}

	//-----------------------------------------------------------------------
	int StringUtil::parseInt(const String& val)
	{
#ifdef USE_UNICODE
		//return _tstoi(val.c_str());
		return _wtoi(val.c_str());
#else
		return atoi(val.c_str());
#endif
	}
	//-----------------------------------------------------------------------
	unsigned int StringUtil::parseUnsignedInt(const String& val)
	{
#ifdef USE_UNICODE
		return static_cast<unsigned int>(wcstoul(val.c_str(), 0, 10));
#else
		return static_cast<unsigned int>(strtoul(val.c_str(), 0, 10));
#endif
		//return static_cast<unsigned int>(_tcstoul(val.c_str(), 0, 10));
	}

	//-----------------------------------------------------------------------
	long StringUtil::parseLong(const String& val)
	{
#ifdef USE_UNICODE
		//return _tstol(val.c_str());
		return _wtol(val.c_str());
#else
		return atol(val.c_str());
#endif
	}

	//-----------------------------------------------------------------------
	unsigned long StringUtil::parseUnsignedLong(const String& val)
	{
#ifdef USE_UNICODE
		return wcstoul(val.c_str(), 0, 10);
#else
		return strtoul(val.c_str(), 0, 10);
#endif		
		//return _tcstoul(val.c_str(), 0, 10);
	}

	//-----------------------------------------------------------------------
	unsigned long long StringUtil::parseUnsignedLongLong(const String& val)
	{
#ifdef USE_UNICODE
		return _wcstoui64(val.c_str(),0,10);
#else
		return _strtoui64(val.c_str(),0,10);
#endif 
		//return _tcstoui64(val.c_str(),0,10);
	}

	//-----------------------------------------------------------------------
	bool StringUtil::parseBool(const String& val)
	{
		return (StringUtil::startsWith(val, T_STRING("true")) || StringUtil::startsWith(val, T_STRING("yes")));
	}

	//-----------------------------------------------------------------------
	StringVector StringUtil::parseStringVector(const String& val)
	{
		return StringUtil::split(val);
	}

	//-----------------------------------------------------------------------
	bool StringUtil::isNumber(const String& val)
	{
#ifdef USE_UNICODE
		std::wistringstream str(val);
#else
		std::istringstream str(val);
#endif
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
	}
}