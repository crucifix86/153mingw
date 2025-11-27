#include "EC_Global.h"
#include "EC_IvtrAstrolabeItem.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "EC_Configs.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrAstrolabeExpPill
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrAstrolabeExpPill::CECIvtrAstrolabeExpPill(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID = ICID_ASTROLABE_EXP_ITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (ASTROLABE_INC_EXP_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit = m_pDBEssence->pile_num_max;
	m_iPrice = m_pDBEssence->price;
	m_iShopPrice = m_pDBEssence->shop_price;
	m_iProcType = m_pDBEssence->proc_type;
	m_i64EquipMask = 0;

	m_bNeedUpdate = false;
}

CECIvtrAstrolabeExpPill::CECIvtrAstrolabeExpPill(const CECIvtrAstrolabeExpPill& s) : CECIvtrItem(s)
{
	m_pDBEssence = s.m_pDBEssence;
}

CECIvtrAstrolabeExpPill::~CECIvtrAstrolabeExpPill()
{
}

bool CECIvtrAstrolabeExpPill::IsRare() const
{
	return false;
}

//	Set item detail information
bool CECIvtrAstrolabeExpPill::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrAstrolabeExpPill::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrAstrolabeExpPill::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrAstrolabeExpPill::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char* CECIvtrAstrolabeExpPill::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrAstrolabePointIncPill
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrAstrolabePointIncPill::CECIvtrAstrolabePointIncPill(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID = ICID_ASTROLABE_POINT_INC_ITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit = m_pDBEssence->pile_num_max;
	m_iPrice = m_pDBEssence->price;
	m_iShopPrice = m_pDBEssence->shop_price;
	m_iProcType = m_pDBEssence->proc_type;
	m_i64EquipMask = 0;

	m_bNeedUpdate = false;
}

CECIvtrAstrolabePointIncPill::CECIvtrAstrolabePointIncPill(const CECIvtrAstrolabePointIncPill& s) : CECIvtrItem(s)
{
	m_pDBEssence = s.m_pDBEssence;
}

CECIvtrAstrolabePointIncPill::~CECIvtrAstrolabePointIncPill()
{
}

bool CECIvtrAstrolabePointIncPill::IsRare() const
{
	return false;
}

//	Set item detail information
bool CECIvtrAstrolabePointIncPill::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrAstrolabePointIncPill::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrAstrolabePointIncPill::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrAstrolabePointIncPill::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char* CECIvtrAstrolabePointIncPill::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrAstrolabeRandomAddonPill
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrAstrolabeRandomAddonPill::CECIvtrAstrolabeRandomAddonPill(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID = ICID_ASTROLABE_RANDOM_ADDON_ITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (ASTROLABE_RANDOM_ADDON_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit = m_pDBEssence->pile_num_max;
	m_iPrice = m_pDBEssence->price;
	m_iShopPrice = m_pDBEssence->shop_price;
	m_iProcType = m_pDBEssence->proc_type;
	m_i64EquipMask = 0;

	m_bNeedUpdate = false;
}

CECIvtrAstrolabeRandomAddonPill::CECIvtrAstrolabeRandomAddonPill(const CECIvtrAstrolabeRandomAddonPill& s) : CECIvtrItem(s)
{
	m_pDBEssence = s.m_pDBEssence;
}

CECIvtrAstrolabeRandomAddonPill::~CECIvtrAstrolabeRandomAddonPill()
{
}

bool CECIvtrAstrolabeRandomAddonPill::IsRare() const
{
	return false;
}

//	Set item detail information
bool CECIvtrAstrolabeRandomAddonPill::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrAstrolabeRandomAddonPill::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrAstrolabeRandomAddonPill::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrAstrolabeRandomAddonPill::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char* CECIvtrAstrolabeRandomAddonPill::GetDropModel()
{
	return m_pDBEssence->file_matter;
}


