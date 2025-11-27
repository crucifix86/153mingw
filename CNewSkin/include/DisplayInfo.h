//------------------------------------------------------------------------------
// DisplayInfo.h
//    
//   This file contains the class DisplayInfo. It contains information
//   regarding all display adaptors on a system, such as memory and all
//   available display resolutions/refresh rates.
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef DISPLAYINFO_H_
#define DISPLAYINFO_H_

#include "SysInfoClasses.h"
//#include <string>
#include <d3d9.h>

class DisplayInfo
{
public:        // object creation/destruction
   DisplayInfo();
   DisplayInfo(const DisplayInfo& source);
   DisplayInfo& operator=(const DisplayInfo& right);
   virtual ~DisplayInfo();

public:        // attribute modification
   virtual void determineDisplayInfo();

   const char* getVideoAdapter() const;
   const char* getVideoDriver() const;
   const char* getVideoDriverVersion() const;
   const char* getVideoVertexShaderVersion() const;
   void getVideoVertexShaderVersion(DWORD* dwVertexShaderVersion) const;
   const char* getVideoPixelShaderVersion() const;
   void getVideoPixelShaderVersion(DWORD* dwPixelShaderVersion) const;
   const char* getVideoHardwareTL() const;
   const char* getVideoMemory() const;
   void getVideoMemory(DWORD* dwVideoMemory) const;
   void getVendorId(DWORD* dwVenderId) const;
   void getDeviceId(DWORD* dwDeviceId) const;
   
protected:     // protected members
   void assign(const DisplayInfo& source);

private:       // attributes
   IDirect3D9 * m_pD3D;
   D3DADAPTER_IDENTIFIER9 m_idAdapter;
   D3DCAPS9 m_d3dcaps;
   DWORD m_iVideoMemory;
   char m_sVideoAdapter[200];
   char m_sVideoDriver[200];
   char m_sVideoDriverVersion[200];
   char m_sVideoVertexShaderVersion[200];
   char m_sVideoPixelShaderVersion[200];
   char m_sVideoHardwareTL[200];
   char m_sVideoMemory[200];
};

inline const char* DisplayInfo::getVideoAdapter() const { return m_sVideoAdapter; }
inline const char* DisplayInfo::getVideoDriver() const { return m_sVideoDriver; }
inline const char* DisplayInfo::getVideoDriverVersion() const { return m_sVideoDriverVersion; }
inline const char* DisplayInfo::getVideoVertexShaderVersion() const { return m_sVideoVertexShaderVersion; }
inline const char* DisplayInfo::getVideoPixelShaderVersion() const { return m_sVideoPixelShaderVersion; }
inline const char* DisplayInfo::getVideoHardwareTL() const { return m_sVideoHardwareTL; }
inline const char* DisplayInfo::getVideoMemory() const { return m_sVideoMemory; }
inline void DisplayInfo::getVideoVertexShaderVersion(DWORD *dwVertexShaderVersion) const { *dwVertexShaderVersion=m_d3dcaps.VertexShaderVersion; }
inline void DisplayInfo::getVideoPixelShaderVersion(DWORD *dwPixelShaderVersion) const{ *dwPixelShaderVersion=m_d3dcaps.PixelShaderVersion; }
inline void DisplayInfo::getVideoMemory(DWORD *dwVideoMemory) const { *dwVideoMemory=m_iVideoMemory; }
inline void DisplayInfo::getVendorId(DWORD *dwVenderId) const{ *dwVenderId=m_idAdapter.VendorId; }
inline void DisplayInfo::getDeviceId(DWORD *dwDeviceId) const { *dwDeviceId=m_idAdapter.DeviceId; }

#endif