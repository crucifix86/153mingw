//------------------------------------------------------------------------------
// MultimediaInfo.h
//    
//   This file contains MultimediaInfo.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#ifndef MultimediaInfo_H_
#define MultimediaInfo_H_

#include "SysInfoClasses.h"
#include <string>

class MultimediaInfo
{
public:        // object creation/destruction
   MultimediaInfo();
   MultimediaInfo(const MultimediaInfo& source);
   MultimediaInfo& operator=(const MultimediaInfo& right);
   virtual ~MultimediaInfo();

public:
    virtual void determineMultimediaInfo();

public:        // attribute modification    
   bool getMMIsInstalled() const;
	bool getMMHasVolCtrl() const;
	bool getMMHasSeparateLRVolCtrl() const;
	std::string getMMCompanyName() const;
   std::string getMMProductName() const;

protected:     // protected members
   void setMMIsInstalled(bool mmIsInstalled);
	void setMMHasVolCtrl(bool mmHasVolCtrl);
	void setMMHasSeparateLRVolCtrl(bool mmHasSeparateLRVolCtrl);
   void setMMCompanyName(const std::string& mmCompanyName);
   void setMMProductName(const std::string& mmProductName);

   virtual void assign(const MultimediaInfo& source);

private:
    std::string GetAudioDevCompanyName (int wCompany);

private:       // attributes
   bool m_bIsInstalled;
	bool m_bHasVolCtrl;
	bool m_bHasSeparateLRVolCtrl;
	std::string m_strCompanyName;
   std::string m_strProductName;
};

inline bool MultimediaInfo::getMMIsInstalled() const { return (m_bIsInstalled); }
inline bool MultimediaInfo::getMMHasVolCtrl() const { return (m_bHasVolCtrl); }
inline bool MultimediaInfo::getMMHasSeparateLRVolCtrl() const { return (m_bHasSeparateLRVolCtrl); }
inline std::string MultimediaInfo::getMMCompanyName() const { return (m_strCompanyName); }
inline std::string MultimediaInfo::getMMProductName() const { return (m_strProductName); }

// for clients
inline void MultimediaInfo::setMMIsInstalled(bool mmIsInstalled) { m_bIsInstalled = mmIsInstalled; }
inline void MultimediaInfo::setMMHasVolCtrl(bool mmHasVolCtrl) { m_bHasVolCtrl = mmHasVolCtrl; }
inline void MultimediaInfo::setMMHasSeparateLRVolCtrl(bool mmHasSeparateLRVolCtrl) { m_bHasSeparateLRVolCtrl = mmHasSeparateLRVolCtrl; }
inline void MultimediaInfo::setMMCompanyName(const std::string& mmCompanyName) { m_strCompanyName = mmCompanyName; }
inline void MultimediaInfo::setMMProductName(const std::string& mmProductName) { m_strProductName = mmProductName; }

#endif