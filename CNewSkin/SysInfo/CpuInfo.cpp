//------------------------------------------------------------------------------
// CpuInfo.cpp
//    
//   This file contains the class CpuInfo. It basically determines all 
//   pertinent information for the CPU. It currently gets in-depth 
//   information for both Intel and AMD CPUs. 
//
//   Parts of this code were submitted by and should be credited to:
//   Alexandre Cesari [acesari@aclogic.com] -- CPU Speed detection
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
//   Updates J. Michael McGarrah [mcgarrah@mcgarware.com]
// 
#include "CpuInfo.h"
#include <tchar.h>  // for _T [registry searches]
#include <sstream>
using namespace std;

//---------------------------------------------------------------------------
// CPU information defines -- VERY x86-dependent stuff going on here
//
#define CPU_ID __asm _emit 0xf __asm _emit 0xa2

const int CpuInfo::BRANDTABLESIZE = 4;

CpuInfo::CpuInfo(const bool fCpuSpeed/* = true*/, const bool fDebug /* = false */)
{
//   m_fDebug = fDebug;
   strcpy(m_strCpuSpeed,"0");
   determineCpuInfo();
   if (fCpuSpeed)
   {
      determineCpuSpeed();
   }
 //  m_osDebugResults << ends;
//   std::string d = m_osDebugResults.str();
}

CpuInfo::CpuInfo(const CpuInfo& source)
{
   assign(source);
}

CpuInfo& CpuInfo::operator=(const CpuInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

CpuInfo::~CpuInfo()
{
   // nothing to do yet
}

void CpuInfo::determineCpuInfo(void)
{
   SYSTEM_INFO stCpuInfo;

   GetSystemInfo(&stCpuInfo);

   sprintf(m_strNumProcessors,"%d",stCpuInfo.dwNumberOfProcessors);

//   if (m_fDebug) m_osDebugResults << "# CPUs detected: " << stCpuInfo.dwNumberOfProcessors << endl;

   determineCpuStats(stCpuInfo.wProcessorArchitecture);
}

// this is pretty straight forward. as long as it's a fairly-recent processor, we'll
// try to get the speed. first, though, we set the priority to high ... so that
// we'll get a greater chance of getting the correct speed.
// the primary algorithm is very accurate, but not supported across all x86 processors.
// the second approach is to get the speed from the registry; this isn't supported
// on all platforms, so ...
// the third approach just does a crude and slow benchmark.
void CpuInfo::determineCpuSpeed(void)
{
   if ( ((m_cpuStats.getVendor() == CpuStats::VENDOR_INTEL) && (m_cpuStats.getFamily() > 4)) ||
        ((m_cpuStats.getVendor() == CpuStats::VENDOR_AMD)   && (m_cpuStats.getFamily() > 5)) )
   {
      DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
      SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

      DWORD dwSpeed = calculateCpuSpeed();
//      if (m_fDebug) m_osDebugResults << "CPU Speed from QueryPerformanceFrequency: " << dwSpeed << endl;
      if (dwSpeed == 0)
      {
         dwSpeed = getCpuSpeedFromRegistry();
//         if (m_fDebug) m_osDebugResults << "CPU Speed from Registry [NT only]" << dwSpeed << endl;
         if (dwSpeed == 0)
         {
            dwSpeed = calculateCpuSpeedMethod2();
 //           if (m_fDebug) m_osDebugResults << "CPU Speed -- old, slow method: " << dwSpeed << endl;
         }
      }

      SetPriorityClass(GetCurrentProcess(), dwStartingPriority);

      // if it's still 0 at this point, god help us all
      if (dwSpeed == 0)
      {
         strcpy(m_strCpuSpeed,"Unknown");
      }
      else
      {
         sprintf(m_strCpuSpeed,"%u MHz",dwSpeed);
      }
   }
      
//   if (m_fDebug) m_osDebugResults << "Final determined speed: " << m_strCpuSpeed << endl;
}

void CpuInfo::assign(const CpuInfo& source)
{
//   m_cpuStats = source.m_cpuStats;
//   m_strCpuSpeed = source.m_strCpuSpeed;
//   m_strNumProcessors = source.m_strNumProcessors;
//   m_fDebug = source.m_fDebug;
//   m_osDebugResults << source.m_osDebugResults.str();
}

void CpuInfo::determineCpuStats(WORD wProcessorArchitecture)
{
   // make sure the cpu knows CPUID
   if (!determineCpuId())
   {
      return;
   }

   m_cpuStats.setKnowsCpuId(true);

   DWORD dwHighestCpuId = determineHighestCpuId();
   switch (dwHighestCpuId)
   {
   default:
   case 2:     // intel cpu's find cache information here
      determineIntelCacheInfo();
      // no break;
   case 1:     // x86 cpu's do processor identification here
      determineCpuIdentification();
      break;

   case 0:     // don't do anything funky; return
      return;
   }

   if (m_cpuStats.getVendor() == CpuStats::VENDOR_INTEL)
   {
      determineOldIntelName();
   }
 
   DWORD dwLargestExtendedFeature = determineLargestExtendedFeature(); 
   if (dwLargestExtendedFeature >= AMD_L2CACHE_FEATURE)
   { 
      determineAmdL2CacheInfo(); 
   } 

   if (dwLargestExtendedFeature >= AMD_L1CACHE_FEATURE)
   { 
      determineAmdL1CacheInfo(); 
   } 

   if (dwLargestExtendedFeature >= NAMESTRING_FEATURE)
   { 
      determineNameString(); 
   } 

   if (dwLargestExtendedFeature >= AMD_EXTENDED_FEATURE)
   { 
      determineExtendedFeature(); 
   } 
}

bool CpuInfo::determineCpuId(void)
{
   int fKnowsCpuId = 0;
   
   __asm
   {
      pushfd                      // save EFLAGS to stack.
      pop     eax                 // store EFLAGS in EAX.
      mov     edx, eax            // save in EBX for testing later.
      xor     eax, 0200000h       // switch bit 21.
      push    eax                 // copy "changed" value to stack.
      popfd                       // save "changed" EAX to EFLAGS.
      pushfd
      pop     eax
      xor     eax, edx            // See if bit changeable.
      jnz     short has_cpuid     // if so, mark 
      mov     fKnowsCpuId, 0      // if not, put 0
      jmp     done
has_cpuid:
      mov     fKnowsCpuId, 1      // put 1
done:
   }

//   if (m_fDebug) m_osDebugResults << "CPU knows CPUID: " << fKnowsCpuId << endl;

   return ((bool)fKnowsCpuId);
}

DWORD CpuInfo::determineHighestCpuId(void)
{
   DWORD dwHighest = 0;
   char szTemp[16] = {0};

   __asm
   {
      mov      eax, 0
      CPUID

      test    eax,eax                   // 0 is highest function, then don't query more info
      jz      no_features
    
      mov     dwHighest, eax            // highest supported instruction
      mov     DWORD PTR [szTemp+0],ebx  // Stash the manufacturer string for later
      mov     DWORD PTR [szTemp+4],edx
      mov     DWORD PTR [szTemp+8],ecx

no_features:
   }

   if (dwHighest != 0)
   {
      m_cpuStats.setVendorId(szTemp);
   }

 //  if (m_fDebug) m_osDebugResults << "Highest CPUID is: " << dwHighest << endl;

   return (dwHighest);
}

void CpuInfo::determineCpuIdentification(void)
{
   DWORD dwSignature = 0;
   DWORD dwFeatureEbx = 0;
   DWORD dwFeatureEcx = 0 ;
   DWORD dwFeatures = 0;

   __asm
   {
      mov      eax, 1
      CPUID

      mov      [dwSignature], eax     // store CPU signature
      mov      [dwFeatureEbx], ebx    
      mov      [dwFeatureEcx], ecx
      mov      [dwFeatures], edx      // features
   }  

   m_cpuStats.setSignature(dwSignature);
   m_cpuStats.setFeatureEbx(dwFeatureEbx);
   m_cpuStats.setFeatureEcx(dwFeatureEcx);
   m_cpuStats.setFeatures(dwFeatures);

//   if (m_fDebug) m_osDebugResults << "CPU Signature:   " << dwSignature << endl;
//   if (m_fDebug) m_osDebugResults << "CPU Feature EBX: " << dwFeatureEbx << endl;
//   if (m_fDebug) m_osDebugResults << "CPU Feature ECX: " << dwFeatureEcx << endl;
//   if (m_fDebug) m_osDebugResults << "CPU Features:    " << dwFeatures << endl;
}

DWORD CpuInfo::determineLargestExtendedFeature(void)
{
   DWORD dwHighest = 0;

   __asm
   {
      mov      eax, 0x80000000
      CPUID

      mov dwHighest, eax
   }

//   if (m_fDebug) m_osDebugResults << "Largest extended CPU feature: " << dwHighest << endl;

   return (dwHighest);
}

void CpuInfo::determineExtendedFeature(void)
{
   DWORD dwExtendedFeatures = 0;

   __asm
   {
      mov      eax, 0x80000001
      CPUID

      mov dwExtendedFeatures, edx
   }

   m_cpuStats.setExtendedFeatures(dwExtendedFeatures);
//   if (m_fDebug) m_osDebugResults << "CPU Extended features: " << dwExtendedFeatures << endl;
}

void CpuInfo::determineNameString(void)
{
   char szName[64] = {0};

   __asm
   {
      mov      eax, 0x80000002
      CPUID
      mov      DWORD PTR [szName+0],eax
      mov      DWORD PTR [szName+4],ebx
      mov      DWORD PTR [szName+8],ecx
      mov      DWORD PTR [szName+12],edx
      mov      eax, 0x80000003
      CPUID
      mov      DWORD PTR [szName+16],eax
      mov      DWORD PTR [szName+20],ebx
      mov      DWORD PTR [szName+24],ecx
      mov      DWORD PTR [szName+28],edx
      mov      eax, 0x80000004
      CPUID
      mov      DWORD PTR [szName+32],eax
      mov      DWORD PTR [szName+36],ebx
      mov      DWORD PTR [szName+40],ecx
      mov      DWORD PTR [szName+44],edx
   }

   m_cpuStats.setName(szName);
//   if (m_fDebug) m_osDebugResults << "CPU name string: " << szName << endl;
}

void CpuInfo::determineAmdL1CacheInfo(void)
{
   DWORD dwEax = 0;
   DWORD dwEbx = 0;
   DWORD dwEcx = 0;
   DWORD dwEdx = 0;

   __asm
   {
      mov      eax, 0x80000005
      CPUID
      mov      dwEax, eax
      mov      dwEbx, ebx
      mov      dwEcx, ecx
      mov      dwEdx, edx
   }

   m_cpuStats.setCacheEax(dwEax);
   m_cpuStats.setCacheEbx(dwEbx);
   m_cpuStats.setCacheEcx(dwEcx);
   m_cpuStats.setCacheEdx(dwEdx);
//   if (m_fDebug) m_osDebugResults << "L1 Cache Info EAX: " << dwEax << endl;
//   if (m_fDebug) m_osDebugResults << "L1 Cache Info EBX: " << dwEbx << endl;
//   if (m_fDebug) m_osDebugResults << "L1 Cache Info ECX: " << dwEcx << endl;
//   if (m_fDebug) m_osDebugResults << "L1 Cache Info EDX: " << dwEdx << endl;
}

void CpuInfo::determineAmdL2CacheInfo(void)
{
   DWORD dwEax = 0;
   DWORD dwEbx = 0;
   DWORD dwEcx = 0;
   DWORD dwEdx = 0;

   __asm
   {
      mov      eax, 0x80000006
      CPUID
      mov      dwEax, eax
      mov      dwEbx, ebx
      mov      dwEcx, ecx
      mov      dwEdx, edx
   }

   m_cpuStats.setCache2Eax(dwEax);
   m_cpuStats.setCache2Ebx(dwEbx);
   m_cpuStats.setCache2Ecx(dwEcx);
   m_cpuStats.setCache2Edx(dwEdx);
//   if (m_fDebug) m_osDebugResults << "L2 Cache Info EAX: " << dwEax << endl;
//   if (m_fDebug) m_osDebugResults << "L2 Cache Info EBX: " << dwEbx << endl;
//   if (m_fDebug) m_osDebugResults << "L2 Cache Info ECX: " << dwEcx << endl;
//   if (m_fDebug) m_osDebugResults << "L2 Cache Info EDX: " << dwEdx << endl;
}

void CpuInfo::determineIntelCacheInfo(void)
{
   DWORD dwEax = 0;
   DWORD dwEbx = 0;
   DWORD dwEcx = 0;
   DWORD dwEdx = 0;

   __asm
   {
      mov      eax, 2
      CPUID
      cmp      al, 1
      jne      end_find_intel_cacheinfo

      // if one iteration isn't enough, this code won't produce anything meaningful!
      // this is from intel; look into changing it
      mov      dwEax, eax
      mov      dwEbx, ebx
      mov      dwEcx, ecx
      mov      dwEdx, edx

end_find_intel_cacheinfo:

   }

   m_cpuStats.setCacheEax(dwEax);
   m_cpuStats.setCacheEbx(dwEbx);
   m_cpuStats.setCacheEcx(dwEcx);
   m_cpuStats.setCacheEdx(dwEdx);
//   if (m_fDebug) m_osDebugResults << "Intel Cache Info EAX: " << dwEax << endl;
//   if (m_fDebug) m_osDebugResults << "Intel Cache Info EBX: " << dwEbx << endl;
//   if (m_fDebug) m_osDebugResults << "Intel Cache Info ECX: " << dwEcx << endl;
//   if (m_fDebug) m_osDebugResults << "Intel Cache Info EDX: " << dwEdx << endl;
}


// this function is mostly ripped from intel's developer.intel.com webpage
// on the CPUID instruction
void CpuInfo::determineOldIntelName()
{
   bool fIsCeleron;
   bool fIsXeon;

   struct brand_entry
   {
      long  lBrandValue;
      char* pszBrand;
   };

   struct brand_entry brand_table[BRANDTABLESIZE] = 
   {
      1, "Genuine Intel Celeron(TM) processor",
      2, "Genuine Intel Pentium(R) III processor",
      3, "Genuine Intel Pentium(R) III Xeon(TM) processor",
      8, "Genuine Intel Pentium(R) 4 processor"
   };

   /* we don't support old intel cpu's that don't know CPUID
   if (m_cpuStats.getKnowsCpuId() == 0)
   {
   // 8086 - 80486
   switch(m_cpuStats.getFamily())
   {
   case 3:
      m_strCpuIdentification = "Intel386(TM) processor";
      break;
   case 4:
      m_strCpuIdentification = "Intel486(TM) processor";
      break;
   default:
      m_strCpuIdentification = "Unknown processor";
      break;
   } */

   if (determineHighestCpuId() < NAMESTRING_FEATURE)
   {
      switch (m_cpuStats.getFamily())
      {
      case 4:  // 486
         switch(m_cpuStats.getModel())
         {
         case 0:
         case 1:
            m_cpuStats.setName("Intel486(TM) DX processor");
            break;
         case 2:
            m_cpuStats.setName("Intel486(TM) SX processor");
            break;
         case 3:
            m_cpuStats.setName("IntelDX2(TM) processor");
            break;
         case 4:
            m_cpuStats.setName("Intel486(TM) processor");
            break;
         case 5:
            m_cpuStats.setName("IntelSX2(TM) processor");
            break;
         case 7:
            m_cpuStats.setName("Writeback Enhanced IntelDX2(TM) processor");
            break;
         case 8:
            m_cpuStats.setName("IntelDX4(TM) processor");
            break;
         default:
            m_cpuStats.setName("Intel 486 processor");
            break;
			}

			break;

      case 5:  // pentium
         m_cpuStats.setName("Intel Pentium(R) processor");
         break;

      case 6:  // pentium II and family
         switch (m_cpuStats.getModel())
         {
         case 1:
            m_cpuStats.setName("Intel Pentium(R) Pro processor");
            break;
         case 3:
            m_cpuStats.setName("Intel Pentium(R) II processor, model 3");
            break;
         case 5:
         case 7:
            fIsCeleron = false;
            fIsXeon = false;

            determineCeleronAndXeon(m_cpuStats.getCacheEax(), &fIsCeleron, &fIsXeon, true);
            determineCeleronAndXeon(m_cpuStats.getCacheEbx(), &fIsCeleron, &fIsXeon);
            determineCeleronAndXeon(m_cpuStats.getCacheEcx(), &fIsCeleron, &fIsXeon);
            determineCeleronAndXeon(m_cpuStats.getCacheEdx(), &fIsCeleron, &fIsXeon);
        
            if (fIsCeleron)
            {
               m_cpuStats.setName("Intel Celeron(TM) processor, model 5");
            }
            else
            {
               if (fIsXeon)
               {
                  if (m_cpuStats.getModel() == 5)
                  {
                     m_cpuStats.setName("Intel Pentium(R) II Xeon(TM) processor");
                  }
                  else
                  {
                     m_cpuStats.setName("Intel Pentium(R) III Xeon(TM) processor");
                  }
               }
               else
               {
                  if (m_cpuStats.getModel() == 5)
                  {
                     m_cpuStats.setName("Intel Pentium(R) II processor, model 5");
                  }
                  else
                  {
                     m_cpuStats.setName("Intel Pentium(R) III processor");
                  }
               }
            }

            break;

         case 6:
            m_cpuStats.setName("Intel Celeron(TM) processor, model 6");
            break;
         case 8:
            m_cpuStats.setName("Intel Pentium(R) III Coppermine processor");
            break;
         default:
            {
               int brand_index = 0;
               while ((brand_index < BRANDTABLESIZE) &&
                      ((m_cpuStats.getFeatureEbx() & 0xff) != brand_table[brand_index].lBrandValue))
               {
                  brand_index++;
               }

               if (brand_index < BRANDTABLESIZE)
               {
                  m_cpuStats.setName(brand_table[brand_index].pszBrand);
               }
               else
               {
                  m_cpuStats.setName("Unknown Genuine Intel processor");
               }

               break;
            }
         } 
      }

      if (m_cpuStats.getFeature(MMX_FLAG))
      {
         char strName[200];
		 sprintf(strName,"%s with MMX",m_cpuStats.getName());
         m_cpuStats.setName(strName);
      }		   
   }

//   if (m_fDebug) m_osDebugResults << "CPU Old Intel name: " << m_cpuStats.getName() << endl;
}

// this is ripped from intel, but put into function form. instead of testing for
// each register, we have a function to test a register's value.
// newer processors won't even use this.
void CpuInfo::determineCeleronAndXeon(DWORD dwRegisterCache, bool* pfIsCeleron, bool* pfIsXeon, bool fIsEax)
{
   DWORD dwCacheTemp;

   dwCacheTemp = dwRegisterCache & 0xFF000000;
   if (dwCacheTemp == 0x40000000)
   {
      *pfIsCeleron = true;
   }
   if ((dwCacheTemp >= 0x44000000) && (dwCacheTemp <= 0x45000000))
   {
      *pfIsXeon = true;
   }

   dwCacheTemp = dwRegisterCache & 0xFF0000;
   if (dwCacheTemp == 0x400000)
   {
      *pfIsCeleron = true;
   }
   if ((dwCacheTemp >= 0x440000) && (dwCacheTemp <= 0x450000))
   {
      *pfIsXeon = true;
   }

   dwCacheTemp = dwRegisterCache & 0xFF00;
   if (dwCacheTemp == 0x4000)
   {
      *pfIsCeleron = true;
   }
   if ((dwCacheTemp >= 0x4400) && (dwCacheTemp <= 0x4500))
   {
      *pfIsXeon = true;
   }

   if (!fIsEax)
   {
      dwCacheTemp = dwRegisterCache & 0xFF;     // possibly not needed for m_dwCacheEax
      if (dwCacheTemp == 0x40000000)            
      {
         *pfIsCeleron = true;
      }
      if ((dwCacheTemp >= 0x44000000) && (dwCacheTemp <= 0x45000000))
      {
         *pfIsXeon = true;
      }
   }
}

//------------------------------------------------------------
// This speed-determination algorithm was taken from 
// Alexandre Cesari -- acesari@aclogic.com
//
DWORD CpuInfo::calculateCpuSpeed(void) const
{
   DWORD dwStartTicks = 0;
   DWORD dwEndTicks = 0;
   DWORD dwTotalTicks = 0;
   DWORD dwCpuSpeed = 0;
   __int64 n64Frequency ;
   __int64 n64Start;
   __int64 n64Stop ;

   if (QueryPerformanceFrequency((LARGE_INTEGER*)&n64Frequency)) 
   {
      QueryPerformanceCounter((LARGE_INTEGER*)&n64Start);
      dwStartTicks = determineTimeStamp();

      Sleep(300);

      dwEndTicks = determineTimeStamp();

      QueryPerformanceCounter((LARGE_INTEGER*)&n64Stop);
      dwTotalTicks = dwEndTicks - dwStartTicks;
      dwCpuSpeed = dwTotalTicks / ((1000000*(n64Stop-n64Start))/n64Frequency);
   } 
   else
   {
      dwCpuSpeed = 0;
   }
   
   return (dwCpuSpeed);
}

// this function is ripped from amd
DWORD CpuInfo::calculateCpuSpeedMethod2(void) const
{
   int   nTimeStart = 0;
   int   nTimeStop = 0;
   DWORD dwStartTicks = 0;
   DWORD dwEndTicks = 0;
   DWORD dwTotalTicks = 0;
   DWORD dwCpuSpeed = 0;

   nTimeStart = timeGetTime();

   for(;;)
   {
      nTimeStop = timeGetTime();

      if ((nTimeStop - nTimeStart) > 1)
      {
         dwStartTicks = determineTimeStamp();
         break;
      }
   }

   nTimeStart = nTimeStop;

   for(;;)
   {
      nTimeStop = timeGetTime();
      if ((nTimeStop - nTimeStart) > 500)    // one-half second
      {
         dwEndTicks = determineTimeStamp();
         break;
      }
   }

   dwTotalTicks = dwEndTicks - dwStartTicks;
   dwCpuSpeed = dwTotalTicks / 500000;

   return (dwCpuSpeed);
}

// Not always accurate but order of magnitude faster
//
//   Copyright (c) 2001 J. Michael McGarrah (mcgarrah@mcgarware.com)
//
DWORD CpuInfo::getCpuSpeedFromRegistry(void) const
{
   HKEY hKey;
   LONG result;
   DWORD dwSpeed = 0;
   DWORD dwType = 0;
   DWORD dwSpeedSize;

   // Get the processor speed from registry
   result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                           "Hardware\\Description\\System\\CentralProcessor\\0",
                           0,
                           KEY_QUERY_VALUE,
                           &hKey);

   // Check if the function has succeeded.
   if (result == ERROR_SUCCESS)
   {
      result = ::RegQueryValueEx (hKey, _T("~MHz"), NULL, NULL, (LPBYTE)&dwSpeed, &dwSpeedSize);

      if (result == ERROR_SUCCESS) {
         //----------------------------------------------------------
         // this function was modified so that it doesn't actually
         // modify the object in any way. it's more of a utility
         // function like calculateCpuSpeed() and 
         // calculateCpuSpeedMethod2()
         //
      }
      else
      {
         // explicity make the speed 0 just in case RegQueryValueEx puts a value in dwSpeed
         dwSpeed = 0;
      }
   }
   else
   {
   }

   // Make sure to close the reg key
   RegCloseKey (hKey);

   return (dwSpeed);
}

// stolen from amd
DWORD CpuInfo::determineTimeStamp(void) const
{
   DWORD dwTickVal;

   __asm
   {
      _emit 0Fh
      _emit 31h
      mov   dwTickVal, eax
   }

   return (dwTickVal);
}