//------------------------------------------------------------------------------
// OSInfo.h
//    
//   This file contains OSInfo. Basically, this class just determines 
//   which windows operating system is running. It also figures out service 
//   pack or build information. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef OSINFO_H_
#define OSINFO_H_

#include "SysInfoClasses.h"
#include "stdio.h"
#include <string.h>

class OSInfo
{
public:        // object creation/destruction
   OSInfo();
   OSInfo(const OSInfo& source);
   OSInfo& operator=(const OSInfo& right);
   virtual ~OSInfo();

public:
   enum e_OS { WIN95 = 0, WIN98, WINME, WIN32S, WINNT351, WINNT40, WIN2000, WINXP };

public:        // attribute modification
   e_OS getVersion() const;
   const char* getDescription() const;
   const char* getOSBuildNumber() const;
   const char* getOSPlatform() const;
   const char* getOSMinorVersion() const;
   const char* getOSServicePack() const;

protected:     // protected members
   void setVersion(const e_OS& version);
   void setDescription(const char* description);
   void setOSBuildNumber(const char* osBuildNumber);
   void setOSPlatform(const char* osPlatform);
   void setOSMinorVersion(const char* osMinorVersion);
   void setOSServicePack(const char* osServicePack);

   virtual void assign(const OSInfo& source);
   virtual void determineOSInfo();

private:       // attributes
   e_OS m_eVersion;
   char m_strDescription[200];
   char m_strOSBuildNumber[200];
   char m_strOSPlatform[200];
   char m_strOSMinorVersion[200];
   char m_strOSServicePack[200];

};

inline OSInfo::e_OS OSInfo::getVersion() const { return (m_eVersion); }
inline const char* OSInfo::getDescription() const { return (m_strDescription); }
inline const char* OSInfo::getOSBuildNumber() const { return (m_strOSBuildNumber); };
inline const char* OSInfo::getOSPlatform() const { return (m_strOSPlatform); };
inline const char* OSInfo::getOSMinorVersion() const { return (m_strOSMinorVersion); };
inline const char* OSInfo::getOSServicePack() const { return (m_strOSServicePack); };

// for derived classes
inline void OSInfo::setVersion(const e_OS& version) { m_eVersion = version; }
inline void OSInfo::setDescription(const char* description) { strcpy(m_strDescription,description); }
inline void OSInfo::setOSBuildNumber(const char* osBuildNumber) { strcpy(m_strOSBuildNumber,osBuildNumber); }
inline void OSInfo::setOSPlatform(const char* osPlatform) { strcpy(m_strOSPlatform,osPlatform); }
inline void OSInfo::setOSMinorVersion(const char* osMinorVersion) { strcpy(m_strOSMinorVersion,osMinorVersion); }
inline void OSInfo::setOSServicePack(const char* osServicePack) { strcpy(m_strOSServicePack,osServicePack); }

#endif