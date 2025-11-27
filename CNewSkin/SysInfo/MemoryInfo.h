//------------------------------------------------------------------------------
// MemoryInfo.h
//    
//   This file contains MemoryInfo, which is basically just determines 
//   the system memory information and then stortes the results. 
//   This class COULD provide member functions that would return the
//   number of bytes instead of strings. These would probably be overloaded
//   functions that take pointer arguments.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef MEMORYINFO_H_
#define MEMORYINFO_H_

//#include "SysInfoClasses.h"
#include <windows.h>
//#include <string>

class MemoryInfo
{
public:        // object creation/destruction
   MemoryInfo();
   MemoryInfo(const MemoryInfo& source);
   MemoryInfo& operator=(const MemoryInfo& right);
   virtual ~MemoryInfo();

public:        
   // operations
   virtual void determineMemoryInfo();

   // attribute modification
   const char* getTotalRam() const;
   void getTotalRam(DWORDLONG* pTotalRam) const;
//   const char* getAvailRam() const;
//   void getAvailRam(DWORD* pAvailRam) const;
//   const char* getTotalPageFile() const;
//   void getTotalPageFile(DWORD* pTotalPageFile) const;
//   const char* getAvailPageFile() const;
//   void getAvailPageFile(DWORD* pAvailPageFile) const;
//   const char* getTotalVirtual() const;
//   void getTotalVirtual(DWORD* pTotalVirtual) const;
//   const char* getAvailVirtual() const;
//   void getAvailVirtual(DWORD* pAvailVirtual) const;

protected:     // protected members
   void setMemoryStatusEx(const MEMORYSTATUSEX& memoryStatus);

   virtual void assign(const MemoryInfo& source);

private:       // attributes
   MEMORYSTATUSEX m_stMemStatusEx;
   static const int MEMORY_OFFSET;
   char m_sTotalRam[200];
};

inline void MemoryInfo::getTotalRam(DWORDLONG* pTotalRam) const { *pTotalRam = m_stMemStatusEx.ullTotalPhys; }
inline const char* MemoryInfo::getTotalRam() const { return m_sTotalRam; }
//inline void MemoryInfo::getAvailRam(DWORD* pAvailRam) const { *pAvailRam = m_stMemStatus.dwAvailPhys; }
//inline void MemoryInfo::getTotalPageFile(DWORD* pTotalPageFile) const { *pTotalPageFile = m_stMemStatus.dwTotalPageFile; }
//inline void MemoryInfo::getAvailPageFile(DWORD* pAvailPageFile) const { *pAvailPageFile = m_stMemStatus.dwAvailPageFile; }
//inline void MemoryInfo::getTotalVirtual(DWORD* pTotalVirtual) const { *pTotalVirtual = m_stMemStatus.dwTotalVirtual; }
//inline void MemoryInfo::getAvailVirtual(DWORD* pAvailVirtual) const { *pAvailVirtual = m_stMemStatus.dwAvailVirtual; }

// for derived classes
inline void MemoryInfo::setMemoryStatusEx(const MEMORYSTATUSEX& memoryStatusEx) { m_stMemStatusEx = memoryStatusEx; }

#endif