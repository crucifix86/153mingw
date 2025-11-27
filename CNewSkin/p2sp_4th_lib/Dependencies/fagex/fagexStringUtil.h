
#ifndef __fagexStringUtil_h__
#define __fagexStringUtil_h__

#include "fagexStdHeaders.h"
#include "TCHAR.h"

//#define USE_UNICODE
namespace fagex {
	typedef float							Real;

#ifdef USE_UNICODE
	typedef std::wstring					String;
	#define T_STRING(a)						TEXT(a)
	#define T_CHAR							wchar_t
#else
	typedef std::string						String;
	#define T_STRING(a)						(a)
	#define T_CHAR							char
#endif
	typedef std::vector<String>				StringVector;
	typedef std::map<String,String>			StringMap;
	typedef std::vector<StringMap>			StringMapVector;
	typedef std::map<String,StringMap>		StringMapMap;
	typedef std::multimap<String,String>	StringMultimap;
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	class StringUtil
	{
	public:
#ifdef USE_UNICODE
		typedef std::wostringstream StrStreamType;
#else
		typedef std::ostringstream StrStreamType;
#endif

		//fanzhenyu function-------------------------------------------------------------------
		/*  
		*	static bool replace(String& src, const String& old, const String& now)
		*	replace 改良版，替换的时候允许 now 中包含 old 的值
		*	2007-12-26
		*/
		static bool replace(String& src, const String& old, const String& now);
		

		/*
		*	static bool replaceOf(String& src, const String& old, const String& now)
		*   replaceOf 替换src中出现在old的中的任意字符
		*	2007-12-26
		*/
		static bool replaceOf(String& src, const String& old, const String& now);

		/*
		*	static String getMatchString(const String& src, const String& start, const String& end)
		*	getMatchString取得开始、结束中间的子字串(包括开始、结束字符串)
		*	2007-12-26
		*/
		static String getMatchString(const String& src, const String& start, const String& end);
		static std::wstring getMatchStringW(const std::wstring& src, const std::wstring& start, const std::wstring& end);

		/*
		*	static String getValueFormString(const String& src, const String& start, const String& end)
		*	getMatchString取得开始、结束中间的子字串(不包括开始、结束字符串)
		*	2008-1-9
		*/
		static String getValueFormString(const String& src, const String& start, const String& end);
		static std::wstring getValueFormStringW(const std::wstring& src, const std::wstring& start, const std::wstring& end);

		/*
		*	static bool eraseMatchString(String& src, const String& start, const String& end, int count)
		*	eraseMatchString删除匹配字符串（start、end 为始终标志）count 为删除的数目，0表示全部
		*	2007-12-26
		*/
		static bool eraseMatchString(String& src, const String& start, const String& end, unsigned count = 0);
		
		/*
		*	static String EncodeURIComponent(const String& str)
		*	16进制编码
		*	2007-12-27	
		*/
		static std::string EncodeComponent(const std::string& src);
		
		static std::string EncodeURIComponent(const std::string& src);

		// ------------------------------------------------------------------------------------------
		static std::string vs_sprintf(const char* format, ...)
		{
			std::string s;
			va_list ap;
			va_start(ap, format);
			StringUtil::stdstring_svformat(s, format, ap);
			va_end(ap);
			return s;
		}

		// ------------------------------------------------------------------------------------------
		static void stdstring_svformat(std::string& s, const char *format, va_list ap) {
			size_t len = ::_vscprintf(format, ap) + 1;
			char* buffer = new char[len];
			memset(buffer, 0, len);
			::vsprintf_s(buffer, len, format, ap);
			s.assign(buffer, len);
			delete buffer;
		}

		//fanzhenyu function end--------------------------------------------------------------------
		static StringVector split(const String& str, const String& delims = T_STRING("\t\n "), unsigned int maxSplits = 0);
		
		static void trim( String& str, bool left = true, bool right = true );
		static void Trim(String& str, const String& strtrim,bool left = true, bool right = true );
        static void toLowerCase( String& str );
        static void toUpperCase( String& str );
		static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);
        static String standardisePath( const String &init);
        static void splitFilename(const String& qualifiedName,
            String& outBasename, String& outPath);
        static bool match(const String& str, const String& pattern, bool caseSensitive = true);

		/** Converts a Real to a String. */
        static String toString(Real val, unsigned short precision = 6, 
            unsigned short width = 0, T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

        /** Converts an int to a String. */
        static String toString(int val, unsigned short width = 0, 
            T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

		/** Converts a size_t to a String. */
        static String toString(size_t val, 
            unsigned short width = 0, T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

        /** Converts an unsigned long to a String. */
        static String toString(unsigned long val, 
            unsigned short width = 0, T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

        /** Converts a long to a String. */
        static String toString(long val, 
            unsigned short width = 0, T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );

		/** Converts an unsigned long to a String. */
        static String toString(unsigned long long val, 
            unsigned short width = 0, T_CHAR fill = ' ', 
            std::ios::fmtflags flags = std::ios::fmtflags(0) );
        
		static String toString(bool val, bool yesNo = false);
        static String toString(const StringVector& val);
        static Real parseReal(const String& val);
        static int parseInt(const String& val);
        static unsigned int parseUnsignedInt(const String& val);
        static long parseLong(const String& val);
        static unsigned long parseUnsignedLong(const String& val);
		static unsigned long long parseUnsignedLongLong(const String& val);
        static bool parseBool(const String& val);
        static StringVector parseStringVector(const String& val);
        static bool isNumber(const String& val);

		static const String ENTER;
		static const String BLANK;
	};
}

#endif //__fagexStringUtil_h__