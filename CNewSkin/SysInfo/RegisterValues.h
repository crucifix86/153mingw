//------------------------------------------------------------------------------
// RegisterValues.h
//    
//   This file holds RegisterValues, a class meant to stores the values 
//   of the four main registers [usually after a CPUID instruction]. 
//   
// 
//   Copyright (c) 2001 Paul Wendt [p-wendt@wideopenwest.com]
// 
#ifndef REGISTERVALUES_H_
#define REGISTERVALUES_H_

#include "SysInfoClasses.h"
#include <windows.h>

class RegisterValues
{
public:        // object creation/destruction
   RegisterValues();
   RegisterValues(DWORD dwEax, DWORD dwEbx, DWORD dwEcx, DWORD dwEdx);
   RegisterValues(const RegisterValues& source);
   RegisterValues& operator=(const RegisterValues& right);
   ~RegisterValues();

public:        // attribute modification
   DWORD getEaxValue() const;
   void setEaxValue(DWORD dwEax);
   DWORD getEbxValue() const;
   void setEbxValue(DWORD dwEbx);
   DWORD getEcxValue() const;
   void setEcxValue(DWORD dwEcx);
   DWORD getEdxValue() const;
   void setEdxValue(DWORD dwEdx);

protected:     // protected members
   void assign(const RegisterValues& source);

private:       // attributes
   DWORD m_dwEaxValue;
   DWORD m_dwEbxValue;
   DWORD m_dwEcxValue;
   DWORD m_dwEdxValue;
};

inline DWORD RegisterValues::getEaxValue() const { return (m_dwEaxValue); }
inline void RegisterValues::setEaxValue(DWORD dwEax) { m_dwEaxValue = dwEax; }
inline DWORD RegisterValues::getEbxValue() const { return (m_dwEbxValue); }
inline void RegisterValues::setEbxValue(DWORD dwEbx) { m_dwEbxValue = dwEbx;}
inline DWORD RegisterValues::getEcxValue() const { return (m_dwEcxValue); }
inline void RegisterValues::setEcxValue(DWORD dwEcx) { m_dwEcxValue = dwEcx;}
inline DWORD RegisterValues::getEdxValue() const { return (m_dwEdxValue); }
inline void RegisterValues::setEdxValue(DWORD dwEdx) { m_dwEdxValue = dwEdx;}

#endif