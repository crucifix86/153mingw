//------------------------------------------------------------------------------
// SysInfo.h
//    
//   This is the SysInfo class. It gets different types of system 
//   information, including hard-drives, CPU information, total system 
//   RAM, etc.... 
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com.com]
// 
#ifndef SYSINFO_H_
#define SYSINFO_H_

#include "SysInfoClasses.h"
#include "DisplayInfo.h"
#include "CpuInfo.h"
#include "OSInfo.h"
#include "MemoryInfo.h"
//#include "SocketInfo.h"
//#include "DriveInfo.h"
//#include "MouseInfo.h"
//#include "KeyboardInfo.h"
//#include "LocaleInfo.h"
//#include "MultimediaInfo.h"
//#include <windows.h>

//#include <string>
//#include <vector>

class DLL_CLASS SysInfo
{
public:        // object creation/destruction
   SysInfo(
//		   const bool& fCpuSpeed = true, const bool& fDebugInfo = false
//           const unsigned int& unDriveTypes = DriveInfo::HARDDRIVE | DriveInfo::NETWORK,
//           const int& nSockMajor = 2, const int& nSockMinor = 0
			);

//   SysInfo(const SysInfo& source);
//   SysInfo& operator=(const SysInfo& right);
   virtual ~SysInfo();

public:        
   // find operations
//   virtual void determineSysInfo(const bool& fCpuSpeed = true,
//                                 const unsigned int& unDriveTypes = DriveInfo::HARDDRIVE | DriveInfo::NETWORK,
//                                 const int& nSockMajor = 2, const int& nSockMinor = 0);

   // Display information
   const char* GetVideoAdapter() const;
   const char* GetVideoDriver() const;
   const char* GetVideoDriverVersion() const;
   const char* GetVideoVertexShaderVersion() const;
   void GetVideoVertexShaderVersion(DWORD* dwVertexShaderVersion) const;
   const char* GetVideoPixelShaderVersion() const;
   void GetVideoPixelShaderVersion(DWORD* dwPixelShaderVersion) const;
   const char* GetVideoHardwareTL() const;
   const char* GetVideoMemory() const;
   void GetVideoMemory(DWORD* dwVideomMemory) const;
   void GetVendorId(DWORD* dwVenderId) const;
   void GetDeviceId(DWORD* dwDeviceId) const;
   
   // CPU information
   const char* GetCpu() const;
//   const char* getCpuVendor() const;
//   const char* getCpuSpeed() const;
   void GetCpuSpeed(DWORD* pdwCpuSpeed) const;
//   const char* getNumProcessors() const;
//   void getNumProcessors(DWORD* pdwNumProcessors) const;
//   int getCpuFamily() const;
//   int getCpuModel() const;
//   int getCpuStepping() const;
//   bool getFeature(const DWORD& dwFeatureMask) const;
//   bool getExtFeature(const DWORD& dwExtFeatureMask) const;

   // OS information
   const char* GetOS() const;
//   OSInfo::e_OS getOSVersion() const;
//   const char* getOSBuildNumber() const;
//   const char* getOSPlatform() const;
//   const char* getOSMinorVersion() const;
//   const char* getOSServicePack() const;

   // memory information
   const char* GetTotalRam() const;
   void GetTotalRam(DWORDLONG *pTotalRam) const;
//   const char* getAvailRam() const;
//   const char* getTotalPageFile() const;
//   const char* getAvailPageFile() const;
//   const char* getTotalVirtual() const;
//   const char* getAvailVirtual() const;

   // socket information
//   const char* getSocketVersion() const;
//   const char* getHighestSocketVersion() const;
//   const char* getSocketDescription() const;
//   const char* getSocketSystemStatus() const;
//   const char* getSocketMax() const;
//   const char* getIPAddress() const;
//   const char* getDomainName() const;
//   const char* getSocketUdpMax() const;
//   const char* getComputerName() const;
//   const char* getUserName() const;

   // drive information
//#if defined(SYSINFO_USEOLDINTERFACE)
//   std::vector<DriveStats> const * getDriveStats() const;
//#else
//   const std::vector<DriveStats>& getDriveStats() const;
//#endif

   // mouse information
//   bool getMousePresent() const;
//   bool getMouseSwapped() const;
//   int getMouseThresholdX() const;
//   int getMouseThresholdY() const;
//   int getMouseSpeed() const;

   // keyboard information
//   long getKeyboardRepeat() const;
//   long getKeyboardDelay() const;

   // locale information
//   int         getCodePage() const;
//   const char* getCalendarType() const;
//   int         getOEMCodePage() const;
//   const char* getLanguage() const;
//   const char* getDateFormat() const;
//   const char* getCountry() const;
//   int         getCountryCode() const;
//   const char* getTimeFormat() const;
//   const char* getCurrency() const;
//   const char* getTimeFormatSpecifier() const;
//   const char* getLocaleLocalLanguage() const;
//   const char* getLocaleTimeZone() const;

   // multimedia information
//   bool getMMIsInstalled() const;
//   bool getMMHasVolCtrl() const;
//   bool getMMHasSeparateLRVolCtrl() const;
//   const char* getMMCompanyName() const;
//   const char* getMMProductName() const;

   // debugging information
//   void setDebug(const bool& fDebug);
//   bool getDebug() const;
//   const char* getDebugInfo() const;

protected:     // protected members
//   void setCpuInfo(const CpuInfo& cpuInfo);
//   void setOSInfo(const OSInfo& osInfo);
//   void setMemoryInfo(const MemoryInfo& memoryInfo);
//   void setSocketInfo(const SocketInfo& socketInfo);
//   void setDriveInfo(const DriveInfo& driveInfo);
//   void setMouseInfo(const MouseInfo& mouseInfo);
//   void setKeyboardInfo(const KeyboardInfo& keyboardInfo);
//   void setLocaleInfo(const LocaleInfo& localeInfo);
//   void setMultimediaInfo(const MultimediaInfo& multimediaInfo);

//   virtual void assign(const SysInfo& source);

private:       // attributes
   DisplayInfo m_videoInfo;
   CpuInfo    m_cpuInfo;
   OSInfo     m_osInfo;
   MemoryInfo m_memoryInfo;
//   SocketInfo m_socketInfo;
//   DriveInfo  m_driveInfo;
//   MouseInfo  m_mouseInfo;
//   KeyboardInfo  m_keyboardInfo;
//   LocaleInfo m_localeInfo;
//   MultimediaInfo m_multimediaInfo;
//   bool m_fDebugInfo;
//   const char* m_strDebugInfo;
};

inline const char* SysInfo::GetVideoAdapter() const { return m_videoInfo.getVideoAdapter(); };
inline const char* SysInfo::GetVideoDriver() const { return m_videoInfo.getVideoDriver(); };
inline const char* SysInfo::GetVideoDriverVersion() const { return m_videoInfo.getVideoDriverVersion(); };
inline const char* SysInfo::GetVideoVertexShaderVersion() const { return m_videoInfo.getVideoVertexShaderVersion(); };
inline void SysInfo::GetVideoVertexShaderVersion(DWORD *dwVertexShaderVersion) const { m_videoInfo.getVideoVertexShaderVersion(dwVertexShaderVersion); };
inline const char* SysInfo::GetVideoPixelShaderVersion() const { return m_videoInfo.getVideoPixelShaderVersion(); };
inline void SysInfo::GetVideoPixelShaderVersion(DWORD *dwPixelShaderVersion) const { m_videoInfo.getVideoPixelShaderVersion(dwPixelShaderVersion); };
inline const char* SysInfo::GetVideoHardwareTL() const { return m_videoInfo.getVideoHardwareTL(); };
inline const char* SysInfo::GetVideoMemory() const { return m_videoInfo.getVideoMemory(); };
inline void SysInfo::GetVideoMemory(DWORD *dwVideoMemory) const { m_videoInfo.getVideoMemory(dwVideoMemory); };
inline void SysInfo::GetVendorId(DWORD *dwVenderId) const{ m_videoInfo.getVendorId(dwVenderId); }
inline void SysInfo::GetDeviceId(DWORD *dwDeviceId) const { m_videoInfo.getDeviceId(dwDeviceId); }
inline const char* SysInfo::GetCpu() const { return (m_cpuInfo.getCpuIdentification()); }
//inline const char* SysInfo::getCpuVendor() const { return (m_cpuInfo.getStats().getVendorId()); }
//inline const char* SysInfo::getCpuSpeed() const { return (m_cpuInfo.getCpuSpeed()); }
inline void SysInfo::GetCpuSpeed(DWORD* pdwCpuSpeed) const { m_cpuInfo.getCpuSpeed(pdwCpuSpeed); }
//inline const char* SysInfo::getNumProcessors() const { return (m_cpuInfo.getNumProcessors()); }
//inline void SysInfo::getNumProcessors(DWORD* pdwNumProcessors) const { m_cpuInfo.getNumProcessors(pdwNumProcessors); }
//inline int SysInfo::getCpuFamily() const { return (m_cpuInfo.getFamily()); }
//inline int SysInfo::getCpuModel() const { return (m_cpuInfo.getModel()); }
//inline int SysInfo::getCpuStepping() const { return (m_cpuInfo.getStepping()); }
//inline bool SysInfo::getFeature(const DWORD& dwFeatureMask) const { return (m_cpuInfo.getStats().getFeature(dwFeatureMask)); }
//inline bool SysInfo::getExtFeature(const DWORD& dwExtFeatureMask) const { return (m_cpuInfo.getStats().getExtendedFeature(dwExtFeatureMask)); }
inline const char* SysInfo::GetOS() const { return (m_osInfo.getDescription()); }
//inline OSInfo::e_OS SysInfo::getOSVersion() const { return (m_osInfo.getVersion()); }
//inline const char* SysInfo::getOSBuildNumber() const { return (m_osInfo.getOSBuildNumber()); }
//inline const char* SysInfo::getOSPlatform() const { return (m_osInfo.getOSPlatform()); }
//inline const char* SysInfo::getOSMinorVersion() const { return (m_osInfo.getOSMinorVersion()); }
//inline const char* SysInfo::getOSServicePack() const { return (m_osInfo.getOSServicePack()); }
inline const char* SysInfo::GetTotalRam() const { return (m_memoryInfo.getTotalRam()); }
inline void SysInfo::GetTotalRam(DWORDLONG *pTotalRam) const { m_memoryInfo.getTotalRam(pTotalRam); }
//inline const char* SysInfo::getAvailRam() const { return (m_memoryInfo.getAvailRam()); }
//inline const char* SysInfo::getTotalPageFile() const { return (m_memoryInfo.getTotalPageFile()); }
//inline const char* SysInfo::getAvailPageFile() const { return (m_memoryInfo.getAvailPageFile()); }
//inline const char* SysInfo::getTotalVirtual() const { return (m_memoryInfo.getTotalVirtual()); }
//inline const char* SysInfo::getAvailVirtual() const { return (m_memoryInfo.getAvailVirtual()); }
//inline const char* SysInfo::getSocketVersion() const { return (m_socketInfo.getVersion()); }
//inline const char* SysInfo::getHighestSocketVersion() const { return (m_socketInfo.getHighestVersion()); }
//inline const char* SysInfo::getSocketDescription() const { return (m_socketInfo.getDescription()); }
//inline const char* SysInfo::getSocketSystemStatus() const { return (m_socketInfo.getSystemStatus()); }
//inline const char* SysInfo::getSocketMax() const { return (m_socketInfo.getMax()); }
//inline const char* SysInfo::getIPAddress() const { return (m_socketInfo.getIPAddress()); }
//inline const char* SysInfo::getDomainName() const { return (m_socketInfo.getDomainName()); }
//inline const char* SysInfo::getSocketUdpMax() const { return (m_socketInfo.getUdpMax()); }
//inline const char* SysInfo::getComputerName() const { return (m_socketInfo.getComputerName()); }
//inline const char* SysInfo::getUserName() const { return (m_socketInfo.getUserName()); }
//#if defined(SYSINFO_USEOLDINTERFACE)
//inline std::vector<DriveStats> const * SysInfo::getDriveStats() const { return (m_driveInfo.getDriveStats()); }
//#else
//inline const std::vector<DriveStats>& SysInfo::getDriveStats() const { return (m_driveInfo.getDriveStats()); }
//#endif
//inline bool SysInfo::getMousePresent() const { return (m_mouseInfo.getMousePresent()); }
//inline bool SysInfo::getMouseSwapped() const { return(m_mouseInfo.getMouseSwapped()); }
//inline int SysInfo::getMouseThresholdX() const { return(m_mouseInfo.getMouseThresholdX()); }
//inline int SysInfo::getMouseThresholdY() const { return(m_mouseInfo.getMouseThresholdY()); }
//inline int SysInfo::getMouseSpeed() const { return(m_mouseInfo.getMouseSpeed()); }
//inline long SysInfo::getKeyboardRepeat() const { return(m_keyboardInfo.getKeyboardRepeat()); }
//inline long SysInfo::getKeyboardDelay() const { return(m_keyboardInfo.getKeyboardDelay()); }
//inline int SysInfo::getCodePage() const { return(m_localeInfo.getCodePage()); }
//inline const char* SysInfo::getCalendarType() const { return(m_localeInfo.getCalendarType()); }
//inline int SysInfo::getOEMCodePage() const { return(m_localeInfo.getOEMCodePage()); }
//inline const char* SysInfo::getLanguage() const { return(m_localeInfo.getLanguage()); }
//inline const char* SysInfo::getDateFormat() const { return(m_localeInfo.getDateFormat()); }
//inline const char* SysInfo::getCountry() const { return(m_localeInfo.getCountry()); }
//inline int SysInfo::getCountryCode() const { return(m_localeInfo.getCountryCode()); }
//inline const char* SysInfo::getTimeFormat() const { return(m_localeInfo.getTimeFormat()); }
//inline const char* SysInfo::getCurrency() const { return(m_localeInfo.getCurrency()); }
//inline const char* SysInfo::getTimeFormatSpecifier() const { return(m_localeInfo.getTimeFormatSpecifier()); }
//inline const char* SysInfo::getLocaleLocalLanguage() const { return(m_localeInfo.getLocaleLocalLanguage()); }
//inline const char* SysInfo::getLocaleTimeZone() const { return(m_localeInfo.getLocaleTimeZone()); }
//inline bool SysInfo::getMMIsInstalled() const { return(m_multimediaInfo.getMMIsInstalled()); }
//inline bool SysInfo::getMMHasVolCtrl() const { return(m_multimediaInfo.getMMHasVolCtrl()); }
//inline bool SysInfo::getMMHasSeparateLRVolCtrl() const { return(m_multimediaInfo.getMMHasSeparateLRVolCtrl()); }
//inline const char* SysInfo::getMMCompanyName() const { return(m_multimediaInfo.getMMCompanyName()); }
//inline const char* SysInfo::getMMProductName() const { return(m_multimediaInfo.getMMProductName()); }
//inline void SysInfo::setDebug(const bool& fDebug) { m_fDebugInfo = fDebug; }
//inline bool SysInfo::getDebug() const { return (m_fDebugInfo); }
//inline const char* SysInfo::getDebugInfo() const { return (m_strDebugInfo); }
//
//// for derived clients
//inline void SysInfo::setCpuInfo(const CpuInfo& cpuInfo) { m_cpuInfo = cpuInfo; }
//inline void SysInfo::setOSInfo(const OSInfo& osInfo) { m_osInfo = osInfo; }
//inline void SysInfo::setMemoryInfo(const MemoryInfo& memoryInfo) { m_memoryInfo = memoryInfo; }
//inline void SysInfo::setSocketInfo(const SocketInfo& socketInfo) { m_socketInfo = socketInfo; }
//inline void SysInfo::setDriveInfo(const DriveInfo& driveInfo) { m_driveInfo = driveInfo; }
//inline void SysInfo::setMouseInfo(const MouseInfo& mouseInfo) { m_mouseInfo = mouseInfo; }
//inline void SysInfo::setKeyboardInfo(const KeyboardInfo& keyboardInfo) { m_keyboardInfo = keyboardInfo; }
//inline void SysInfo::setLocaleInfo(const LocaleInfo& localeInfo) { m_localeInfo = localeInfo; }
//inline void SysInfo::setMultimediaInfo(const MultimediaInfo& multimediaInfo) { m_multimediaInfo = multimediaInfo; }

#endif