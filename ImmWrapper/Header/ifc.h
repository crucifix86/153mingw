// Stub header for Immersion Force Feedback SDK (ifc.h)
// The actual SDK is proprietary - this provides stub declarations
#ifndef _IFC_H_
#define _IFC_H_

#include <windows.h>

class CImmDevice;
class CImmCompoundEffect;

// Stub class declarations for force feedback
class CImmDevice
{
public:
    static CImmDevice* CreateDevice(HINSTANCE hInst, HWND hWnd) { return NULL; }
    void Release() {}
};

class CImmProject
{
public:
    CImmProject() {}
    ~CImmProject() {}
    bool OpenFile(const char* szFile, CImmDevice* pDevice) { return false; }
    void Close() {}
    CImmCompoundEffect* CreateEffect(const char* szName, CImmDevice* pDevice) { return NULL; }
    void DestroyEffect(CImmCompoundEffect* pEffect) {}
    bool Start(const char* szName, CImmDevice* pDevice) { return false; }
    bool Start(const char* szName) { return false; }
};

class CImmCompoundEffect
{
public:
    CImmCompoundEffect() {}
    ~CImmCompoundEffect() {}
    bool Start(int nIterations = 1) { return false; }
    bool Stop() { return false; }
};

#endif // _IFC_H_
