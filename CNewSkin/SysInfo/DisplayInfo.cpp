 #include "DisplayInfo.h"
#include <windows.h>
#include <ddraw.h>
#include <d3d9.h>
#include <conio.h>
#include <sstream>
#include "SysUtils.h"

using namespace std;


DisplayInfo::DisplayInfo()
{
	determineDisplayInfo();
}

DisplayInfo::DisplayInfo(const DisplayInfo& source)
{
   assign(source);
}

DisplayInfo& DisplayInfo::operator=(const DisplayInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

DisplayInfo::~DisplayInfo()
{
   m_pD3D->Release();
}

void DisplayInfo::determineDisplayInfo()
{
   m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

   // first get adapter identifier
   m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_WHQL_LEVEL, &m_idAdapter);

   ZeroMemory(&m_d3dcaps, sizeof(m_d3dcaps));	
   m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_d3dcaps);

   HRESULT hval;

   // now query video memory through ddraw interface
   IDirectDraw * pDD;
   IDirectDraw2 * pDD2;
   DirectDrawCreate(NULL, &pDD, NULL);
   hval = pDD->QueryInterface(IID_IDirectDraw2, (LPVOID *) &pDD2);
   if( DD_OK != hval )
   {
	// error handling
   }
   pDD->Release();
   DDSCAPS		ddscaps;
   ddscaps.dwCaps = DDSCAPS_LOCALVIDMEM;
   DWORD dwFree;
   hval = pDD2->GetAvailableVidMem(&ddscaps, &m_iVideoMemory, &dwFree);
   if( DD_OK != hval )
   {
   // error handling
   }
   pDD2->Release();
   strcpy(m_sVideoAdapter,m_idAdapter.Description);
   strcpy(m_sVideoDriver,m_idAdapter.Driver);
   WORD Product = HIWORD(m_idAdapter.DriverVersion.HighPart);
   WORD Version = LOWORD(m_idAdapter.DriverVersion.HighPart);
   WORD SubVersion = HIWORD(m_idAdapter.DriverVersion.LowPart);
   WORD Build = LOWORD(m_idAdapter.DriverVersion.LowPart);
   sprintf(m_sVideoDriverVersion,"%d.%d.%d.%d",Product,Version,SubVersion,Build);
   DWORD dwVertexVersion = m_d3dcaps.VertexShaderVersion;
   if( (dwVertexVersion & 0xffff0000) != 0xFFFE0000 )
      strcpy(m_sVideoVertexShaderVersion,"Unknown");
   else
      sprintf(m_sVideoVertexShaderVersion,"%d.%d",HIBYTE(LOWORD(dwVertexVersion)),LOBYTE(LOWORD(dwVertexVersion)));
   DWORD dwPixelVersion = m_d3dcaps.PixelShaderVersion;
   if( (dwPixelVersion & 0xffff0000) != 0xFFFF0000 )
      strcpy(m_sVideoPixelShaderVersion,"Unknown");
   else
      sprintf(m_sVideoPixelShaderVersion,"%d.%d",HIBYTE(LOWORD(dwPixelVersion)),LOBYTE(LOWORD(dwPixelVersion)));
   if( m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
      strcpy(m_sVideoHardwareTL,"Available");
   else
      strcpy(m_sVideoHardwareTL,"Not available");
	sprintf(m_sVideoMemory,"%d MB",m_iVideoMemory>>20);
}

void DisplayInfo::assign(const DisplayInfo& source)
{
   // assign all of source's members to this*
   m_pD3D=source.m_pD3D;
   m_idAdapter=source.m_idAdapter;
   m_d3dcaps=source.m_d3dcaps;
   m_iVideoMemory=source.m_iVideoMemory;
}
