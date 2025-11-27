//------------------------------------------------------------------------------
// KeyboardInfo.cpp
//    
//   This file contains KeyboardInfo.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#include "KeyboardInfo.h"
#include <windows.h>

#include <sstream>
using namespace std;

KeyboardInfo::KeyboardInfo()
{
   determineKeyboardInfo();
}

KeyboardInfo::KeyboardInfo(const KeyboardInfo& source)
{
   assign(source);
}

KeyboardInfo& KeyboardInfo::operator=(const KeyboardInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

KeyboardInfo::~KeyboardInfo()
{
   // nothing to do yet
}

void KeyboardInfo::assign(const KeyboardInfo& source)
{

    m_dwKeyboardRepeat = source.m_dwKeyboardRepeat;
    m_dwKeyboardDelay = source.m_dwKeyboardDelay;

}

void KeyboardInfo::determineKeyboardInfo(void)
{
    //Get the keyboard repeat-speed setting.
    SystemParametersInfo(SPI_GETKEYBOARDSPEED,
                         NULL,
                         &m_dwKeyboardRepeat,
                         NULL);

    //Get the keyboard delay setting.
    SystemParametersInfo(SPI_GETKEYBOARDDELAY,
                         NULL,
                         &m_dwKeyboardDelay,
                         NULL);

    return; 
}
