//------------------------------------------------------------------------------
// SocketInfo.h
//    
//   This file contains SocketInfo. All it does is enable the client 
//   to obtain information about the network ... like the IP address, 
//   the computer name, as well as socket information, if requested. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef SOCKETINFO_H_
#define SOCKETINFO_H_

#include "SysInfoClasses.h"
#include <windows.h>
#include <string>

#pragma comment(lib, "wsock32.lib")

class SocketInfo
{
public:        // object creation/destruction
   SocketInfo(const int& nMajorVersion = 2, const int& nMinorVersion = 0);
   SocketInfo(const SocketInfo& source);
   SocketInfo& operator=(const SocketInfo& right);
   virtual ~SocketInfo();

public:
   virtual void determineSocketInfo(const int& nMajorVersion, const int& nMinorVersion);

   std::string getVersion(void) const;
   std::string getHighestVersion(void) const;
   std::string getDescription(void) const;
   std::string getSystemStatus(void) const;
   std::string getMax(void) const;
   std::string getIPAddress(void) const;
   std::string getDomainName(void) const;
   std::string getUdpMax(void) const;
   std::string getUserName(void) const;
   std::string getComputerName(void) const;

protected:     // protected members
   virtual void determineHostAndIPAddress(void);
   virtual void determineNames(void);
   virtual void assign(const SocketInfo& source);

   void setVersion(const std::string& version);
   void setHighestVersion(const std::string& highestVersion);
   void setDescription(const std::string& description);
   void setSystemStatus(const std::string& systemStatus);
   void setMax(const std::string& max);
   void setIPAddress(const std::string& ipAddress);
   void setDomainName(const std::string& domainName);
   void setUdpMax(const std::string& udpMax);
   void setUserName(const std::string& userName);
   void setComputerName(const std::string& computerName);

private:       // attributes
   std::string m_strVersion;
   std::string m_strHighestVersion;
   std::string m_strDescription;
   std::string m_strSystemStatus;
   std::string m_strMax;
   std::string m_strIPAddress;
   std::string m_strDomainName;
   std::string m_strUdpMax;
   std::string m_strUserName;
   std::string m_strComputerName;
   static const int STRINGSIZE;
};

// for public world
inline std::string SocketInfo::getVersion(void) const { return (m_strVersion); }
inline std::string SocketInfo::getHighestVersion(void) const { return (m_strHighestVersion); }
inline std::string SocketInfo::getDescription(void) const { return (m_strDescription); }
inline std::string SocketInfo::getSystemStatus(void) const { return (m_strSystemStatus); }
inline std::string SocketInfo::getMax(void) const { return (m_strMax); }
inline std::string SocketInfo::getIPAddress(void) const { return (m_strIPAddress); }
inline std::string SocketInfo::getDomainName(void) const { return (m_strDomainName); }
inline std::string SocketInfo::getUdpMax(void) const { return (m_strUdpMax); }
inline std::string SocketInfo::getUserName(void) const { return (m_strUserName); }
inline std::string SocketInfo::getComputerName(void) const { return (m_strComputerName); }

// for derived classes
inline void SocketInfo::setVersion(const std::string& version) { m_strVersion = version; }
inline void SocketInfo::setHighestVersion(const std::string& highestVersion) { m_strHighestVersion = highestVersion; }
inline void SocketInfo::setDescription(const std::string& description) { m_strDescription = description; }
inline void SocketInfo::setSystemStatus(const std::string& systemStatus) { m_strSystemStatus = systemStatus; }
inline void SocketInfo::setMax(const std::string& max) { m_strMax = max; }
inline void SocketInfo::setIPAddress(const std::string& ipAddress) { m_strIPAddress = ipAddress; }
inline void SocketInfo::setDomainName(const std::string& domainName) { m_strDomainName = domainName; }
inline void SocketInfo::setUdpMax(const std::string& udpMax) { m_strUdpMax = udpMax; }
inline void SocketInfo::setUserName(const std::string& userName) { m_strUserName = userName; }
inline void SocketInfo::setComputerName(const std::string& computerName) { m_strComputerName = computerName; }

#endif