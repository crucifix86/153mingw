//------------------------------------------------------------------------------
// LocaleInfo.h
//    
//   This file contains LocaleInfo, which is basically just grabs the
//   locale information from various sources on your system.
//
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#ifndef LocaleInfo_H_
#define LocaleInfo_H_

#include "SysInfoClasses.h"
#include <windows.h>
#include <string>

class LocaleInfo
{
public:        // object creation/destruction
   LocaleInfo();
   LocaleInfo(const LocaleInfo& source);
   LocaleInfo& operator=(const LocaleInfo& right);
   virtual ~LocaleInfo();

public:        
   // operations
   virtual void determineLocaleInfo();

   // attribute modification
   int getCodePage() const;
   std::string getCalendarType() const;
   int getOEMCodePage() const;
   std::string getLanguage() const;
   std::string getDateFormat() const;
   std::string getCountry() const;
   int getCountryCode() const;
   std::string getTimeFormat() const;
   std::string getCurrency() const;
   std::string getTimeFormatSpecifier() const;
   std::string getLocaleLocalLanguage() const;
   std::string getLocaleTimeZone() const;

protected:     // protected members
   void setCodePage(int codePage);
   void getCalendarType(const std::string& calendarType);
   void getOEMCodePage(int oemCodePage);
   void getLanguage(const std::string& language);
   void getDateFormat(const std::string& dateFormat);
   void getCountry(const std::string& country);
   void getCountryCode(int countryCode);
   void getTimeFormat(const std::string& timeFormat);
   void getCurrency(const std::string& currency);
   void getTimeFormatSpecifier(const std::string& timeFormatSpecifier);
   void getLocaleLocalLanguage(const std::string& localeLocalLanguage);
   void getLocaleTimeZone(const std::string& localeTimeZone);

   virtual void assign(const LocaleInfo& source);

private:       // attributes
    int         m_iCodePage;
    std::string m_strCalendarType;
    int         m_iOEMCodePage;
    std::string m_strLanguage;
    std::string m_strDateFormat;
    std::string m_strCountry;
    int         m_iCountryCode;
    std::string m_strTimeFormat;
    std::string m_strCurrency;
    std::string m_strTimeFormatSpecifier;

    std::string m_strLocaleLocalLanguage;
    std::string m_strLocaleTimeZone;
};

inline int LocaleInfo::getCodePage() const { return (m_iCodePage); }
inline std::string LocaleInfo::getCalendarType() const { return (m_strCalendarType); }
inline int LocaleInfo::getOEMCodePage() const { return (m_iOEMCodePage); }
inline std::string LocaleInfo::getLanguage() const { return (m_strLanguage); }
inline std::string LocaleInfo::getDateFormat() const { return (m_strDateFormat); }
inline std::string LocaleInfo::getCountry() const { return (m_strCountry); }
inline int LocaleInfo::getCountryCode() const { return (m_iCountryCode); }
inline std::string LocaleInfo::getTimeFormat() const { return (m_strTimeFormat); }
inline std::string LocaleInfo::getCurrency() const { return (m_strCurrency); }
inline std::string LocaleInfo::getTimeFormatSpecifier() const { return (m_strTimeFormatSpecifier); }
inline std::string LocaleInfo::getLocaleLocalLanguage() const { return (m_strLocaleLocalLanguage); }
inline std::string LocaleInfo::getLocaleTimeZone() const { return (m_strLocaleTimeZone); }

// for derived classes
inline void LocaleInfo::setCodePage(int codePage) { m_iCodePage = codePage; }
inline void LocaleInfo::getCalendarType(const std::string& calendarType) { m_strCalendarType = calendarType; }
inline void LocaleInfo::getOEMCodePage(int oemCodePage) { m_iOEMCodePage = oemCodePage; }
inline void LocaleInfo::getLanguage(const std::string& language) { m_strLanguage = language; }
inline void LocaleInfo::getDateFormat(const std::string& dateFormat) { m_strDateFormat = dateFormat; }
inline void LocaleInfo::getCountry(const std::string& country) { m_strCountry = country; }
inline void LocaleInfo::getCountryCode(int countryCode) { m_iCountryCode = countryCode; }
inline void LocaleInfo::getTimeFormat(const std::string& timeFormat) { m_strTimeFormat = timeFormat; }
inline void LocaleInfo::getCurrency(const std::string& currency) { m_strCurrency = currency; }
inline void LocaleInfo::getTimeFormatSpecifier(const std::string& timeFormatSpecifier) { m_strTimeFormatSpecifier = timeFormatSpecifier; }
inline void LocaleInfo::getLocaleLocalLanguage(const std::string& localeLocalLanguage) { m_strLocaleLocalLanguage = localeLocalLanguage; }
inline void LocaleInfo::getLocaleTimeZone(const std::string& localeTimeZone) { m_strLocaleTimeZone = localeTimeZone; }

#endif