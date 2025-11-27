//------------------------------------------------------------------------------
// DriveInfo.h
//    
//   This file contains the class DriveInfo. It's just a class that iterates 
//   through all drives on a system, getting information for each one. 
//   The information can be obtained via the getDriveStats function [returns 
//   a pointer to a vector of DriveStats objects].
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef DRIVEINFO_H_
#define DRIVEINFO_H_

#include "SysInfoClasses.h"
#include "DriveStats.h"
#include <vector>
#include <iterator>
#include <utility>
#include <string>

#if defined(SYSINFO_USEOLDINTERFACE)
#define HARDDRIVE 1
#define NETWORK   2
#define FLOPPY    4
#define CDROM     8
#endif

// this is the function pointer for GetDiskFreeSpaceEx() [doesn't exist on Win95A]
// so we have to see if kernel32.dll exports this function. if it doesn't, we'll
// use GetDiskFreeSpace()
typedef BOOL (WINAPI* PFNGETDISKFREESPACEEX)(LPCTSTR, PULARGE_INTEGER,
												          PULARGE_INTEGER, PULARGE_INTEGER);

class DriveInfo
{
public:
#if !defined(SYSINFO_USEOLDINTERFACE)
   enum DiskTypes { HARDDRIVE = 1, NETWORK = 2, FLOPPY = 4, CDROM = 8 };
#endif

   // object creation/destruction
   DriveInfo(const unsigned int& unDriveTypes = HARDDRIVE | NETWORK);
   DriveInfo(const DriveInfo& source);
   DriveInfo& operator=(const DriveInfo& right);
   virtual ~DriveInfo();

   virtual void determineDriveInfo(const unsigned int& unDriveTypes = HARDDRIVE | NETWORK);

#if defined(SYSINFO_USEOLDINTERFACE)
   std::vector<DriveStats> const * getDriveStats() const;
#else
   const std::vector<DriveStats>& getDriveStats() const;
#endif

protected:     // protected members
   virtual void determineVolumeInfo(DriveStats* pDriveStats) const;
   virtual void assign(const DriveInfo& source);
   void setDriveStats(const std::vector<DriveStats>& driveStats);

private:       // attributes
   std::vector<DriveStats> m_vdriveStats;
   static const int STRINGSIZE;
};

inline const std::vector<DriveStats>& DriveInfo::getDriveStats() const { return m_vdriveStats; }
inline void DriveInfo::setDriveStats(const std::vector<DriveStats>& driveStats) { m_vdriveStats = driveStats; }

#endif