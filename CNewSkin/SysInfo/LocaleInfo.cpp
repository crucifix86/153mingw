//------------------------------------------------------------------------------
// LocaleInfo.cpp
//    
//   This file contains LocaleInfo, which is basically just grabs the
//   locale information from various sources on your system.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#include "LocaleInfo.h"
#include "SysUtils.h"

#include <sstream>
using namespace std;

LocaleInfo::LocaleInfo()
{
   determineLocaleInfo();
}

LocaleInfo::LocaleInfo(const LocaleInfo& source)
{
   assign(source);
}

LocaleInfo& LocaleInfo::operator=(const LocaleInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

LocaleInfo::~LocaleInfo()
{
   // nothing to do yet
}

void LocaleInfo::determineLocaleInfo(void)
{
	// Retrieves the active input locale identifier.
	HKL hKl = ::GetKeyboardLayout (0);

	// LOWORD of the returned value contains the language identifier.
	LANGID langID = LOWORD ((DWORD)hKl);

	// Retirieve the information about the locale.
	LCID lcID = MAKELCID (langID, SORT_DEFAULT);
	TCHAR chPage[7];
	TCHAR szCurrency[7];
	TCHAR szVal[MAX_PATH];
	TCHAR szFormatString[100];

	// Get currently ised code page.
	::GetLocaleInfo (lcID, LOCALE_IDEFAULTANSICODEPAGE, chPage, 7);
	m_iCodePage = atoi (chPage);

	// Get default OEM code page.
	::GetLocaleInfo (lcID, LOCALE_IDEFAULTCODEPAGE, chPage, 7);
	m_iOEMCodePage = atoi (chPage);
 
	// Get country name in english.
	::GetLocaleInfo (lcID, LOCALE_SENGCOUNTRY , szVal, MAX_PATH);
	m_strCountry = szVal;

	// Get country code -- not sure if this works
	::GetLocaleInfo (lcID, LOCALE_ICOUNTRY , chPage, 7);
	m_iCountryCode = atoi (chPage);

	// Get language name.
	::GetLocaleInfo (lcID, LOCALE_SENGLANGUAGE , szVal, MAX_PATH);
	m_strLanguage = szVal;

	// Get TimeFormat String
	::GetLocaleInfo (lcID, LOCALE_STIMEFORMAT , szFormatString, 100);
	m_strTimeFormat = szFormatString;
	
	// Get Date Format String
	::GetLocaleInfo (lcID, LOCALE_SLONGDATE  , szFormatString, 100);
	m_strDateFormat = szFormatString;

	// Get the string used for local currency.
	::GetLocaleInfo (lcID, LOCALE_SCURRENCY, szCurrency, 7);
	m_strCurrency = szCurrency;

	// Get time format specifier i.e. 12 hour (AM/PM) or 24 hour format
	// is used to indicate time.
	::GetLocaleInfo (lcID, LOCALE_ITIME, szVal, 3);
	if (atoi (szVal) == 0)
	{
		m_strTimeFormatSpecifier = "AM / PM 12-hour format";
	}
	else
	{
		m_strTimeFormatSpecifier = "24-hour format";
	}
	
	// Get calendar type
	::GetLocaleInfo (lcID, LOCALE_ICALENDARTYPE, szVal, 3);
	switch (atoi (szVal))
	{
		case 1:
			m_strCalendarType = ("Gregorian - Localized");
			break;
		case 2:
			m_strCalendarType = ("Gregorian - English strings always");
			break;
		case 3:
			m_strCalendarType = ("Year of the Emperor - Japan");
			break;
		case 4:
			m_strCalendarType = ("Year of Taiwan");
			break;
		case 5:
			m_strCalendarType = ("Tangun Era - Korea");
			break;
		case 6:
			m_strCalendarType = ("Hijri - Arabic lunar");
			break;
		case 7:
			m_strCalendarType = ("Thai");
			break;
		case 8:
			m_strCalendarType = ("Hebrew - Lunar");
			break;
		case 9:
			m_strCalendarType = ("Gregorian Middle East French");
			break;
		case 10:
			m_strCalendarType = ("Gregorian Arabic");
			break;
		case 11:
			m_strCalendarType = ("Gregorian Transliterated English");
			break;
		case 12:
			m_strCalendarType = ("Gregorian Transliterated French");
			break;
		default:
			m_strCalendarType = ("Unknown");
	}

	LANGID langId;
	// Get the ID of the language identifier.
	langId = ::GetSystemDefaultLangID ();

	DWORD dwSize, dwError, dwReturnedSize;
	char szLanguage[MAX_PATH];
	// Get the string for the language identifier.
	dwSize = MAX_PATH;
	dwReturnedSize = VerLanguageName (langId, szLanguage, dwSize);
	if (dwReturnedSize <= dwSize)
	{
		m_strLocaleLocalLanguage = szLanguage;
	}

	// Get time zone information.
	TIME_ZONE_INFORMATION info;
    TCHAR szTimezone[31];
	dwError = ::GetTimeZoneInformation (&info);
	if (TIME_ZONE_ID_INVALID != dwError)
	{
        wsprintf(szTimezone, "%S", info.StandardName);
        m_strLocaleTimeZone = szTimezone;

        // TODO: Add Daylight Timezone name
        //wsprintf(szTimezone, "%S", info.DaylightName);
        //m_strLocaleDaylightTimeZone = szTimezone;

        // Bias -- Standard versus Daylight Bias
        //
        //wsprintf(szTimezone, "Bias: %d", info.Bias);
        //::MessageBox(NULL,szTimezone,"Bias",MB_OK);
        //wsprintf(szTimezone, "StandardBias: %d", info.StandardBias);
        //::MessageBox(NULL,szTimezone,"StandardBias",MB_OK);

	}

}

void LocaleInfo::assign(const LocaleInfo& source)
{
    m_iCodePage =       source.m_iCodePage;
    m_strCalendarType = source.m_strCalendarType;
    m_iOEMCodePage =    source.m_iOEMCodePage;
    m_strLanguage =     source.m_strLanguage;
    m_strDateFormat =   source.m_strDateFormat;
    m_strCountry =      source.m_strCountry;
    m_iCountryCode =    source.m_iCountryCode;
    m_strTimeFormat =   source.m_strTimeFormat;
    m_strCurrency =     source.m_strCurrency;
    m_strTimeFormatSpecifier = source.m_strTimeFormatSpecifier;

    m_strLocaleLocalLanguage = source.m_strLocaleLocalLanguage;
    m_strLocaleTimeZone = source.m_strLocaleTimeZone;
}
