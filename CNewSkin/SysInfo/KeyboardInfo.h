//------------------------------------------------------------------------------
// KeyboardInfo.h
//    
//   This file contains KeyboardInfo.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#ifndef KeyboardInfo_H_
#define KeyboardInfo_H_

#include "SysInfoClasses.h"
#include <string>
#include <windows.h>

class KeyboardInfo
{
public:        // object creation/destruction
   KeyboardInfo();
   KeyboardInfo(const KeyboardInfo& source);
   KeyboardInfo& operator=(const KeyboardInfo& right);
   virtual ~KeyboardInfo();

public:
    virtual void determineKeyboardInfo();

public:        // attribute modification
    DWORD getKeyboardRepeat() const;
    DWORD getKeyboardDelay() const;

protected:     // protected members
   void setKeyboardRepeat(DWORD keyboardRepeat);
   void setKeyboardDelay(DWORD keyboardDelay);

   virtual void assign(const KeyboardInfo& source);

private:       // attributes
    DWORD m_dwKeyboardRepeat;
    DWORD m_dwKeyboardDelay;
};

inline DWORD KeyboardInfo::getKeyboardRepeat() const { return (m_dwKeyboardRepeat); }
inline DWORD KeyboardInfo::getKeyboardDelay() const { return (m_dwKeyboardDelay); }

// available for clients
inline void KeyboardInfo::setKeyboardRepeat(DWORD keyboardRepeat) { m_dwKeyboardRepeat = keyboardRepeat; }
inline void KeyboardInfo::setKeyboardDelay(DWORD keyboardDelay) { m_dwKeyboardDelay = keyboardDelay; }

#endif