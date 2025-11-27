//------------------------------------------------------------------------------
// OSInfo.cpp
//    
//   This file contains OSInfo. Basically, this class just determines 
//   which windows operating system is running. It also figures out service 
//   pack or build information. 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
//   Updated J. Michael McGarrah
//

#include "OSInfo.h"
#include <windows.h>

#include <tchar.h>
#include "dtwinver.h"

//#include <sstream>
//using namespace std;

OSInfo::OSInfo()
{
   determineOSInfo();
}

OSInfo::OSInfo(const OSInfo& source)
{
   assign(source);
}

OSInfo& OSInfo::operator=(const OSInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

OSInfo::~OSInfo()
{
   // nothing to do yet
}

void OSInfo::assign(const OSInfo& source)
{
//    m_eVersion = source.m_eVersion;
//    m_strDescription = source.m_strDescription;
//
//    m_strOSBuildNumber = source.m_strOSBuildNumber;
//	m_strOSPlatform = source.m_strOSPlatform;
//	m_strOSMinorVersion = source.m_strOSMinorVersion;
//	m_strOSServicePack = source.m_strOSServicePack;
//
}

void OSInfo::determineOSInfo()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   //--------------------------------------------------------------------
   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.
   //
   memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi)))
   {
      osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

      if (!GetVersionEx((OSVERSIONINFO*) &osvi)) 
      {
         return;
      }
   }

   switch (osvi.dwPlatformId)
   {
   case VER_PLATFORM_WIN32_NT:

      //------------------------------------------
      // Test for the product.
      //
      if (osvi.dwMajorVersion <= 4)
      {
         strcat(m_strDescription,"Microsoft Windows NT ");
      }

      if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
      {
         strcat(m_strDescription,"Microsoft Windows 2000 ");
         m_eVersion = WIN2000;
      }

      if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
      {
         strcat(m_strDescription,"Microsoft Windows XP ");
         m_eVersion = WINXP;
      }

      //-------------------------------------------------------------
      // get version, service pack (if any), and build number.
      //
      if (osvi.dwMajorVersion <= 4)
      {
         char os[200];
		 sprintf(os,"version %d.%d %s (Build %d)",osvi.dwMajorVersion,osvi.dwMinorVersion
			 ,osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
//         os << "version " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion
//            << " " << osvi.szCSDVersion << " (Build " << (osvi.dwBuildNumber & 0xFFFF) 
//            << ") " << ends;
         strcat(m_strDescription,os);

         if (osvi.dwMajorVersion == 4)
         {
            m_eVersion = WINNT40;
         }
         else if ((osvi.dwMajorVersion == 3) && (osvi.dwMinorVersion == 51))
         {
            m_eVersion = WINNT351;
         }
      }
      else
      { 
         char os[200];
		 sprintf(os,"%s (Build %d)",osvi.szCSDVersion,osvi.dwBuildNumber & 0xFFFF);
//         os << osvi.szCSDVersion << " (Build " 
//            << (osvi.dwBuildNumber & 0xFFFF) << ")" << ends;
         strcat(m_strDescription,os);
      }
      break;

   case VER_PLATFORM_WIN32_WINDOWS:

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
      {
         strcat(m_strDescription,"Microsoft Windows 95 ");
         if (osvi.szCSDVersion[1] == 'C')
         {
          strcat(m_strDescription,"OSR2 ");
         }
         else
         {
			 char o[2];
			 sprintf(o,"%c",osvi.szCSDVersion[1]);
          strcat(m_strDescription,o);
          strcat(m_strDescription," ");
         }

         m_eVersion = WIN95;
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
      {
         strcat(m_strDescription,"Microsoft Windows 98 ");
         if (osvi.szCSDVersion[1] == 'A')
         {
          strcat(m_strDescription,"SE ");
         }

         m_eVersion = WIN98;
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
      {
         strcat(m_strDescription,"Microsoft Windows Me ");
         m_eVersion = WINME;
      } 
      break;

   case VER_PLATFORM_WIN32s:
      strcat(m_strDescription,"Microsoft Win32s ");
      m_eVersion = WIN32S;
      break;
   }

    // ******************************
    // SECOND OS VERSION CODE SECTION
    // ******************************

//	m_strOSBuildNumber;
//	m_strOSPlatform;
//	m_strOSMinorVersion;
//	m_strOSServicePack;


    OS_VERSION_INFO osverinfo;
    memset(&osverinfo, 0, sizeof(OS_VERSION_INFO));

    TCHAR sText[512];
    TCHAR sBuf[100];

    COSVersion os;  

    if (os.GetVersion(&osverinfo))
    {                                           
        switch (osverinfo.dwUnderlyingPlatformId)
        {
            case DT_PLATFORM_DOS:               
                {
                    _stprintf(sBuf, _T("DOS"));  
                    _tcscat(sText, sBuf);                
                    strcpy(m_strOSPlatform,sBuf);
                    break;
                }
            case DT_PLATFORM_WINDOWS3x:         
                {
                    _stprintf(sBuf, _T("Windows"));
                    _tcscat(sText, sBuf);                
                    strcpy(m_strOSPlatform,sBuf);
                    break;
                }
            case DT_PLATFORM_WINDOWSCE:            
                {
                    _stprintf(sBuf, _T("Windows CE"));
                    _tcscat(sText, sBuf);
                    strcpy(m_strOSPlatform,sBuf);
                    break;
                }
            case DT_PLATFORM_WINDOWS9x:           
                {
                    if (os.IsWindows95(&osverinfo))
                        _stprintf(sBuf, _T("Windows 95"));
                    else if (os.IsWindows95SP1(&osverinfo))
                        _stprintf(sBuf, _T("Windows 95 SP1"));
                    else if (os.IsWindows95OSR2(&osverinfo))
                        _stprintf(sBuf, _T("Windows 95 OSR2"));
                    else if (os.IsWindows98(&osverinfo))
                        _stprintf(sBuf, _T("Windows 98"));
                    else if (os.IsWindows98SP1(&osverinfo))
                        _stprintf(sBuf, _T("Windows 98 SP1"));
                    else if (os.IsWindows98SE(&osverinfo))
                        _stprintf(sBuf, _T("Windows 98 Second Edition"));
                    else if (os.IsWindowsME(&osverinfo))
                        _stprintf(sBuf, _T("Windows Millenium Edition"));
                    else
                        _stprintf(sBuf, _T("Windows ??"));
                    _tcscat(sText, sBuf);                  
                    strcpy(m_strOSPlatform,sBuf);
                    break;
                }
            case DT_PLATFORM_NT:    
                {
                    if (os.IsNTPreWin2k(&osverinfo))
                    {
                        _stprintf(sBuf, _T("Windows NT"));
                        _tcscat(sText, sBuf);                  
                
                        if (os.IsNTWorkstation(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Workstation)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsNTStandAloneServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Server)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsNTPDC(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Primary Domain Controller)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsNTBDC(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Backup Domain Controller)"));
                            _tcscat(sText, sBuf);
                        }
                
                        if (os.IsNTDatacenterServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Datacenter)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsNTEnterpriseServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Enterprise)"));
                            _tcscat(sText, sBuf);
                        }
                    }
                    else if (os.IsWindows2000(&osverinfo))
                    {
                        _stprintf(sBuf, _T("Windows 2000"));
                        _tcscat(sText, sBuf);                  
                
                        if (os.IsWin2000Professional(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Professional)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsWin2000Server(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Server)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsWin2000DomainController(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Domain Controller)"));
                            _tcscat(sText, sBuf);
                        }
                
                        if (os.IsWin2000DatacenterServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Datacenter)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsWin2000AdvancedServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Advanced Server)"));
                            _tcscat(sText, sBuf);
                        }
                
                        if (os.IsTerminalServicesInstalled(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Terminal Services)"));
                            _tcscat(sText, sBuf);
                        }
                        if (os.ISSmallBusinessEditionInstalled(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (BackOffice Small Business Edition)"));
                            _tcscat(sText, sBuf);
                        }
                    }
                    else if (os.IsWindowsXP(&osverinfo))
                    {
                        _stprintf(sBuf, _T("Windows XP"));
                        _tcscat(sText, sBuf);
                
                        if (os.IsXPPersonal(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Personal)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsXPProfessional(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Professional)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsXPServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Server)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsXPDomainController(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Domain Controller)"));
                            _tcscat(sText, sBuf);
                        }
                
                        if (os.IsXPDatacenterServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Datacenter)"));
                            _tcscat(sText, sBuf);
                        }
                        else if (os.IsXPAdvancedServer(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Advanced Server)"));
                            _tcscat(sText, sBuf);
                        }
                
                        if (os.IsTerminalServicesInstalled(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (Terminal Services)"));
                            _tcscat(sText, sBuf);
                        }
                        if (os.ISSmallBusinessEditionInstalled(&osverinfo))
                        {
                            _stprintf(sBuf, _T(" (BackOffice Small Business Edition)"));
                            _tcscat(sText, sBuf);
                        }
                    }
                    strcpy(m_strOSPlatform,sBuf);
                    break;
              }
              default:                         
              {
                  _stprintf(sBuf, _T("Unknown OS"));
                  _tcscat(sText, sBuf);                
                  strcpy(m_strOSPlatform,sBuf);
                  break;
              }
        } // end switch

        _stprintf(sBuf, _T("%d."), osverinfo.dwUnderlyingMajorVersion);
        _tcscat(sText, sBuf);
        strcpy(m_strOSMinorVersion,sBuf);

        _stprintf(sBuf, _T("%02d"), osverinfo.dwUnderlyingMinorVersion);
        _tcscat(sText, sBuf);
        strcat(m_strOSMinorVersion,sBuf);
        
        if (osverinfo.dwUnderlyingBuildNumber)
        {
            _stprintf(sBuf, _T("%d"), osverinfo.dwUnderlyingBuildNumber);
            _tcscat(sText, sBuf);
            strcpy(m_strOSBuildNumber,sBuf);
        }
        
        if (osverinfo.wUnderlyingServicePack)       
        {
            _stprintf(sBuf, _T("Service Pack: %d"), osverinfo.wUnderlyingServicePack);
            _tcscat(sText, sBuf);
            strcpy(m_strOSServicePack,sBuf);
        }                    
      } 
      else
      {
          _stprintf(sText, _T("Failed in call to GetOSVersion"));
          strcpy(m_strOSPlatform,sText);
          strcpy(m_strOSBuildNumber,"unknown");
          strcpy(m_strOSMinorVersion,"unknown");
          strcpy(m_strOSServicePack,"unknown");
      }

        //	m_strOSBuildNumber;  *
        //	m_strOSPlatform;     *
        //	m_strOSMinorVersion; *
        //	m_strOSServicePack;  *


    return; 
}
