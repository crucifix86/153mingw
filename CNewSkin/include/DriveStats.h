//------------------------------------------------------------------------------
// DriveStats.h
//    
//   This class contains the DriveStats class, which is basically just 
//   a glorified structure with member accessors/mutators. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef DRIVESTATS_H_
#define DRIVESTATS_H_

#include "SysInfoClasses.h"
#include <windows.h>
#include <string>

// forward declaration so that the class can be declared
// as a friend later on
class DriveInfo;

class DriveStats
{
public:        // object creation/destruction
   DriveStats();
   DriveStats(const DriveStats& source);
   DriveStats& operator=(const DriveStats& right);
   virtual ~DriveStats();

public:      
   void getType(unsigned int* punType) const;
   std::string getType() const;
   std::string getName() const;
   std::string getTotalSpace() const;
   std::string getFreeSpace() const;
   std::string getVolumeName() const;
   std::string getSerialNumber() const;
   std::string getFileSystemType() const;

protected:
   // we ONLY want DriveInfo to be able to modify our values.
   // all other clients should have read-access only.
   friend DriveInfo;
#if defined(SYSINFO_USEOLDINTERFACE)
   void setType(const unsigned int& unType);
#endif
   void setType(const std::string& strType);
   void setName(const std::string& strName);
   void setTotalSpace(const std::string& strTotalSpace);
   void setFreeSpace(const std::string& strFreeSpace);
   void setVolumeName(const std::string& strVolumeName);
   void setSerialNumber(const std::string& strSerialNumber);
   void setFileSystemType(const std::string& strFileSystemType);

protected:     // protected members
   virtual void assign(const DriveStats& source);

private:       // attributes
#if defined(SYSINFO_USEOLDINTERFACE)
   unsigned int m_unType;           // type of drive
#endif
   std::string  m_strType;          // type of drive
   std::string  m_strName;          // drive name
   std::string  m_strTotalSpace;    // total drive space
   std::string  m_strFreeSpace;     // total free space
   std::string  m_strVolumeName;    // name of the volume as set by the user
   std::string  m_strSerialNumber;  // not supported for network drives on Win9x/Me
   std::string  m_strFileSystemType;// NTFS, FAT32 [NOT an enum on purpose]
};

// clients of SysInfo get these
#if defined(SYSINFO_USEOLDINTERFACE)
inline void DriveStats::getType(unsigned int* punType) { *punType = m_unType; }
#endif
inline std::string DriveStats::getType() const { return (m_strType); }
inline std::string DriveStats::getName() const { return (m_strName); }
inline std::string DriveStats::getTotalSpace() const { return (m_strTotalSpace); }
inline std::string DriveStats::getFreeSpace() const {  return (m_strFreeSpace); }
inline std::string DriveStats::getVolumeName() const { return (m_strVolumeName); }
inline std::string DriveStats::getSerialNumber() const { return (m_strSerialNumber); }
inline std::string DriveStats::getFileSystemType() const { return (m_strFileSystemType); }

// ONLY DriveInfo and subclasses get these
#if defined(SYSINFO_USEOLDINTERFACE)
inline void DriveStats::setType(const unsigned int& unType) { m_unType = unType; }
#endif
inline void DriveStats::setType(const std::string& strType) { m_strType = strType; }
inline void DriveStats::setName(const std::string& strName) { m_strName = strName; }
inline void DriveStats::setTotalSpace(const std::string& strTotalSpace) { m_strTotalSpace = strTotalSpace; }
inline void DriveStats::setFreeSpace(const std::string& strFreeSpace) { m_strFreeSpace = strFreeSpace; }
inline void DriveStats::setVolumeName(const std::string& strVolumeName) { m_strVolumeName = strVolumeName; }
inline void DriveStats::setSerialNumber(const std::string& strSerialNumber) { m_strSerialNumber = strSerialNumber; }
inline void DriveStats::setFileSystemType(const std::string& strFileSystemType) { m_strFileSystemType = strFileSystemType; }


#endif