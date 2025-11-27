//------------------------------------------------------------------------------
// MouseInfo.cpp
//    
//   This file contains MouseInfo. Basically, this class just determines 
//   if the mouse is present and any other parameters it can find about
//   the pointing/mouse device.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#include "MouseInfo.h"
#include <windows.h>

#include <sstream>
using namespace std;

MouseInfo::MouseInfo()
{
   determineMouseInfo();
}

MouseInfo::MouseInfo(const MouseInfo& source)
{
   assign(source);
}

MouseInfo& MouseInfo::operator=(const MouseInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

MouseInfo::~MouseInfo()
{
   // nothing to do yet
}

void MouseInfo::assign(const MouseInfo& source)
{
    m_bPresent = source.m_bPresent;
    m_bSwapped = source.m_bSwapped;
    m_iThresholdX = source.m_iThresholdX;
    m_iThresholdY = source.m_iThresholdY;
    m_iSpeed = source.m_iSpeed;
}

void MouseInfo::determineMouseInfo(void)
{
    // Check if mouse is present or not.
    m_bPresent = GetSystemMetrics (SM_MOUSEPRESENT);

    if (m_bPresent)
    {
        // Mouse wheel present
        //m_bWheel = GetSystemMetrics(SM_MOUSEWHEELPRESENT);

        // Number of mouse buttons
        //m_iButtons = GetSystemMetrics(SM_CMOUSEBUTTONS);

        // Check if mouse buttons are swapped
        m_bSwapped = GetSystemMetrics (SM_SWAPBUTTON);

        // Get mouse speed, and double click thresholds x/y
        //   mouseInfo [0] & mouseInfo [1], give twp threshold values for mouse
	    //   mpuseInfo [2] gives the mouse speed.
	    int mouseInfo[3];	// We need an array (size 3) of int for mouse information
        SystemParametersInfo (SPI_GETMOUSE, NULL, mouseInfo, NULL);
        m_iThresholdX = mouseInfo[0];
        m_iThresholdY = mouseInfo[1];
        m_iSpeed = mouseInfo[2];

        // Get DblClick time
        //m_iThreshold = GetDoubleClickTime();
    }
    else
    {
        //m_bWheel = FALSE;
        //m_iButtons = 0;
        m_bSwapped = FALSE;
        m_iThresholdX = 0;
        m_iThresholdY = 0;
        m_iSpeed = 0;
        //m_iThreshold = 0;
    }

    return; 
}
