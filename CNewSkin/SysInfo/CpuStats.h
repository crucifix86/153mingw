//------------------------------------------------------------------------------
// CpuStats.h
//    
//   This file contains the CpuStats class. This class is basically a 
//   simple class to hold CPU information with member value accessors 
//   and mutators. It really is used internally by CpuInfo and most
//   clients probably won't be seeing it.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef CPUSTATS_H_
#define CPUSTATS_H_

#include "CpuFeatures.h"
#include "SysInfoClasses.h"
#include "RegisterValues.h"

//#include <string>
#include <windows.h>

//-------------------------------------------------------------------
// This is the CpuStats class. It holds the different types of 
// information from Intel CPUs that are relevant to the CpuInfo class.
// It is basically just a glorified struct.
//
class CpuStats
{
public:        // object creation/destruction
   CpuStats();
   CpuStats(const CpuStats& source);
   CpuStats& operator=(const CpuStats& right);
   ~CpuStats();

public:
   // this enum returns a simple value to test against for CPU vendor
   // it isn't explicitly set in this class; when setVendorId is called,
   // the class sets this enumeration member variable
   enum e_CPUVENDORS { VENDOR_UNKNOWN = 0, VENDOR_INTEL, VENDOR_AMD, VENDOR_CYRIX, VENDOR_CENTAUR };

   friend class CpuInfo;

public:        
   // cpu identification
   void setKnowsCpuId(const bool& fKnowsCpuId);
   bool getKnowsCpuId() const;

   DWORD getSignature() const;
   int getFamily() const;   // implicitly set by setSignature()
   int getModel() const;    // implicitly set by setSignature()
   int getStepping() const; // implicitly set by setSignature()
   const char* getName() const;

   // vendor identification
   const char* getVendorId() const;
   e_CPUVENDORS getVendor() const; // implicitly set by setVendorId()

   // individual features supported by the processor
   // test for the feature itself here
   bool getFeature(const DWORD& dwFeatureMask) const;
   bool getExtendedFeature(const DWORD& dwMask) const;

   // these provide access to the entire [Ext]Features values
   DWORD getFeatures() const;
   DWORD getExtendedFeatures() const;

   // so far, these are only used with Intel CPUs
   DWORD getFeatureEbx() const;
   DWORD getFeatureEcx() const;

   // cache values. cache2 is currently used only for AMD CPUs
   DWORD getCacheEax() const;
   DWORD getCacheEbx() const;
   DWORD getCacheEcx() const;
   DWORD getCacheEdx() const;
   DWORD getCache2Eax() const;
   DWORD getCache2Ebx() const;
   DWORD getCache2Ecx() const;
   DWORD getCache2Edx() const;

protected:
   // signature dictates family, model, stepping, and processor identification number [for p4]
   // it also can indirectly determine name, for older CPUs
   void setSignature(const DWORD& dwSignature);
   void setName(const char* strName);
   void setVendorId(const char* strVendorId);
   void setFeatures(const DWORD& dwFeatures);
   void setExtendedFeatures(const DWORD& dwExtFeatures);
   void setFeatureEbx(const DWORD& dwFeatureEbx);
   void setFeatureEcx(const DWORD& dwFeatureEcx);
   void setCacheEax(const DWORD& dwCacheEax);
   void setCacheEbx(const DWORD& dwCacheEbx);
   void setCacheEcx(const DWORD& dwCacheEcx);
   void setCacheEdx(const DWORD& dwCacheEdx);
   void setCache2Eax(const DWORD& dwCacheEax);
   void setCache2Ebx(const DWORD& dwCacheEbx);
   void setCache2Ecx(const DWORD& dwCacheEcx);
   void setCache2Edx(const DWORD& dwCacheEdx);

protected:     // protected members
   void assign(const CpuStats& source);

private:       // attributes
   bool  m_fCpuIdFlag;              // whether opcode CPU_ID is supported -- late 486 and after
   DWORD m_dwExtendedFeatures;      // AMD CPUs have extended features to test for things like 3dNow!
   char m_strVendorId[200];     // vendor id in string form
   char m_strName[200];         // name string -- either returned by modern CPU's or determined for them
   RegisterValues m_cpuidStats;     // registers from CPUID-1
   RegisterValues m_cacheStats;
   RegisterValues m_cacheStats2;

   static const char* VENDOR_INTEL_STR;
   static const char* VENDOR_AMD_STR;
   static const char* VENDOR_CYRIX_STR;
   static const char* VENDOR_CENTAUR_STR;
};

inline void CpuStats::setKnowsCpuId(const bool& fKnowsCpuId) { m_fCpuIdFlag = fKnowsCpuId;}
inline bool CpuStats::getKnowsCpuId() const { return (m_fCpuIdFlag);}
inline void CpuStats::setSignature(const DWORD& dwSignature) { m_cpuidStats.setEaxValue(dwSignature);}
inline DWORD CpuStats::getSignature() const { return (m_cpuidStats.getEaxValue());}
inline int CpuStats::getFamily() const { return ((getSignature() >> 8) & 0x0f);}
inline int CpuStats::getModel() const { return ((getSignature() >> 4) & 0x0f); }
inline int CpuStats::getStepping() const { return (getSignature() & 0x0f); }
inline void CpuStats::setName(const char* strName) { strcpy(m_strName,strName); }
inline const char* CpuStats::getName() const { return (m_strName); }
inline const char* CpuStats::getVendorId() const { return (m_strVendorId); }
inline bool CpuStats::getFeature(const DWORD& dwFeatureMask) const { return ((m_cpuidStats.getEdxValue() & dwFeatureMask) == dwFeatureMask); }
inline bool CpuStats::getExtendedFeature(const DWORD& dwMask) const { return ((m_dwExtendedFeatures & dwMask) == dwMask); }
inline void CpuStats::setFeatures(const DWORD& dwFeatures) { m_cpuidStats.setEdxValue(dwFeatures);}
inline DWORD CpuStats::getFeatures() const { return (m_cpuidStats.getEdxValue());}
inline void CpuStats::setExtendedFeatures(const DWORD& dwExtFeatures) { m_dwExtendedFeatures = dwExtFeatures;}
inline DWORD CpuStats::getExtendedFeatures() const { return (m_dwExtendedFeatures);}
inline void CpuStats::setFeatureEbx(const DWORD& dwFeatureEbx) { m_cpuidStats.setEbxValue(dwFeatureEbx); }
inline DWORD CpuStats::getFeatureEbx() const { return (m_cpuidStats.getEbxValue()); }
inline void CpuStats::setFeatureEcx(const DWORD& dwFeatureEcx) { m_cpuidStats.setEcxValue(dwFeatureEcx); }
inline DWORD CpuStats::getFeatureEcx() const { return (m_cpuidStats.getEcxValue()); }
inline void CpuStats::setCacheEax(const DWORD& dwCacheEax) { m_cacheStats.setEaxValue(dwCacheEax); }
inline DWORD CpuStats::getCacheEax() const { return (m_cacheStats.getEaxValue());}
inline void CpuStats::setCacheEbx(const DWORD& dwCacheEbx) { m_cacheStats.setEbxValue(dwCacheEbx);}
inline DWORD CpuStats::getCacheEbx() const { return (m_cacheStats.getEbxValue());}
inline void CpuStats::setCacheEcx(const DWORD& dwCacheEcx) { m_cacheStats.setEcxValue(dwCacheEcx);}
inline DWORD CpuStats::getCacheEcx() const { return (m_cacheStats.getEcxValue());}
inline void CpuStats::setCacheEdx(const DWORD& dwCacheEdx) { m_cacheStats.setEdxValue(dwCacheEdx);}
inline DWORD CpuStats::getCacheEdx() const { return (m_cacheStats.getEdxValue());}
inline void CpuStats::setCache2Eax(const DWORD& dwCacheEax) { m_cacheStats2.setEaxValue(dwCacheEax);}
inline DWORD CpuStats::getCache2Eax() const { return (m_cacheStats2.getEaxValue());}
inline void CpuStats::setCache2Ebx(const DWORD& dwCacheEbx) { m_cacheStats2.setEbxValue(dwCacheEbx);}
inline DWORD CpuStats::getCache2Ebx() const { return (m_cacheStats2.getEbxValue());}
inline void CpuStats::setCache2Ecx(const DWORD& dwCacheEcx) { m_cacheStats2.setEcxValue(dwCacheEcx);}
inline DWORD CpuStats::getCache2Ecx() const { return (m_cacheStats2.getEcxValue());}
inline void CpuStats::setCache2Edx(const DWORD& dwCacheEdx) { m_cacheStats2.setEdxValue(dwCacheEdx);}
inline DWORD CpuStats::getCache2Edx() const { return (m_cacheStats2.getEdxValue());}


#endif