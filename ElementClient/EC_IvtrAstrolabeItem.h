#pragma once

#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"


struct ASTROLABE_INC_EXP_ESSENCE;
struct ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE;
struct ASTROLABE_RANDOM_ADDON_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Class class CECIvtrAstrolabeExpPill
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrAstrolabeExpPill: public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrAstrolabeExpPill(int tid, int expire_date);
	CECIvtrAstrolabeExpPill(const CECIvtrAstrolabeExpPill& s);
	virtual ~CECIvtrAstrolabeExpPill();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char* GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrAstrolabeExpPill(*this); }
	//  Get experience
	const unsigned int GetExp() { return m_iExp; }
	//  Get level
	const int GetLevel() { return m_iLevel; }

	//	Get database data
	const ASTROLABE_INC_EXP_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	//	Whether a rare item
	virtual bool IsRare() const;

protected:	//	Attributes

	// Experiences in pill
	unsigned int m_iExp;
	// Level of exp pill
	int  m_iLevel;

	//	Data in database
	ASTROLABE_INC_EXP_ESSENCE* m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class class CECIvtrAstrolabePointIncPill
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrAstrolabePointIncPill : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrAstrolabePointIncPill(int tid, int expire_date);
	CECIvtrAstrolabePointIncPill(const CECIvtrAstrolabePointIncPill& s);
	virtual ~CECIvtrAstrolabePointIncPill();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char* GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrAstrolabePointIncPill(*this); }
	//  Get experience
	const unsigned int GetExp() { return m_iExp; }
	//  Get level
	const int GetLevel() { return m_iLevel; }

	//	Get database data
	const ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	//	Whether a rare item
	virtual bool IsRare() const;

protected:	//	Attributes

	// Experiences in pill
	unsigned int m_iExp;
	// Level of exp pill
	int  m_iLevel;

	//	Data in database
	ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE* m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class class CECAstrolabeRandomAddonPill
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrAstrolabeRandomAddonPill : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrAstrolabeRandomAddonPill(int tid, int expire_date);
	CECIvtrAstrolabeRandomAddonPill(const CECIvtrAstrolabeRandomAddonPill& s);
	virtual ~CECIvtrAstrolabeRandomAddonPill();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char* GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrAstrolabeRandomAddonPill(*this); }
	//  Get experience
	const unsigned int GetExp() { return m_iExp; }
	//  Get level
	const int GetLevel() { return m_iLevel; }

	//	Get database data
	const ASTROLABE_RANDOM_ADDON_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	//	Whether a rare item
	virtual bool IsRare() const;

protected:	//	Attributes

	// Experiences in pill
	unsigned int m_iExp;
	// Level of exp pill
	int  m_iLevel;

	//	Data in database
	ASTROLABE_RANDOM_ADDON_ESSENCE* m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};