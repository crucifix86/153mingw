//------------------------------------------------------------------------------
// MouseInfo.h
//    
//   This file contains MouseInfo. Basically, this class just determines 
//   if the mouse is present and any other parameters it can find about
//   the mouse device.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#ifndef MOUSEINFO_H_
#define MOUSEINFO_H_

#include "SysInfoClasses.h"
#include <string>

class MouseInfo
{
public:        // object creation/destruction
   MouseInfo();
   MouseInfo(const MouseInfo& source);
   MouseInfo& operator=(const MouseInfo& right);
   virtual ~MouseInfo();

public:
   virtual void determineMouseInfo();

   bool getMousePresent() const;
   bool getMouseSwapped() const;
   int getMouseThresholdX() const;
   int getMouseThresholdY() const;
   int getMouseSpeed() const;

protected:     // protected members
   void setMousePresent(bool mousePresent);
   void setMouseSwapped(bool mouseSwapped);
   void setMouseThresholdX(int mouseThresholdX);
   void setMouseThresholdY(int mousethresholdY);
   void setMouseSpeed(int mouseSpeed);

   virtual void assign(const MouseInfo& source);

private:       // attributes
    bool m_bPresent;        // mouse present
    bool m_bSwapped;        // mouse buttons swapped
    int  m_iThresholdX;
    int  m_iThresholdY;
    int  m_iSpeed;
};

inline bool MouseInfo::getMousePresent() const { return (m_bPresent); }
inline bool MouseInfo::getMouseSwapped() const { return (m_bSwapped); }
inline int MouseInfo::getMouseThresholdX() const { return (m_iThresholdX); }
inline int MouseInfo::getMouseThresholdY() const { return (m_iThresholdY); }
inline int MouseInfo::getMouseSpeed() const { return (m_iSpeed); }

// for derived classes
inline void MouseInfo::setMousePresent(bool mousePresent) { m_bPresent = mousePresent; }
inline void MouseInfo::setMouseSwapped(bool mouseSwapped) { m_bSwapped = mouseSwapped; }
inline void MouseInfo::setMouseThresholdX(int mouseThresholdX) { m_iThresholdX = mouseThresholdX; }
inline void MouseInfo::setMouseThresholdY(int mouseThresholdY) { m_iThresholdY = mouseThresholdY; }
inline void MouseInfo::setMouseSpeed(int mouseSpeed) { m_iSpeed = mouseSpeed; }

#endif