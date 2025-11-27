#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "EC_FixedMsg.h"
#include "EC_Player.h"
#include "EC_ShortcutMgr.h"
#include "DlgEquipAccessorySlot.h"
#include "EC_HostPlayer.h"
#include "AFI.h"
#include "EC_IvtrDecoration.h"

#define new A_DEBUG_NEW

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipSlot
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipAccessorySlot> EquipAccessorySlotClickShortcut;

AUI_BEGIN_COMMAND_MAP(CDlgEquipAccessorySlot, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommandCANCEL)
AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("Btn_MinusItemNum", OnCommandSubtractItem)
AUI_ON_COMMAND("Btn_AddItemNum", OnCommandAddItem)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEquipAccessorySlot, CDlgBase)
AUI_ON_EVENT("Img_Equip", WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_END_EVENT_MAP()


CDlgEquipAccessorySlot::CDlgEquipAccessorySlot() : m_iMaxNumber(0), m_pConfig(NULL)
{
}

CDlgEquipAccessorySlot::~CDlgEquipAccessorySlot()
{

}

bool CDlgEquipAccessorySlot::OnInitDialog()
{
	m_pImg_Equip = (PAUIIMAGEPICTURE)GetDlgItem("Img_Equip");
	m_pTxt_Name = GetDlgItem("Txt_EquipName");
	m_pImg_Material = (PAUIIMAGEPICTURE)GetDlgItem("Img_Item"); // stone icon

	m_pSuccessRatio = GetDlgItem("Txt_SuccessRatio");
	m_pCost = GetDlgItem("Txt_CostMoney");
	m_pMaterialNum = GetDlgItem("Edt_ItemNum");
	m_pNextSlot = GetDlgItem("Txt_NextSlotNum");
	m_pCurrentSlot = GetDlgItem("Txt_SlotNum");
	m_pMaxMaterial = GetDlgItem("Txt_MaxItemNum"); // required stones

	DATA_TYPE DataType;
	m_pConfig = (EQUIP_MAKE_HOLE_CONFIG*)GetGame()->GetElementDataMan()->get_data_ptr(2013, ID_SPACE_CONFIG, DataType);

	if (!m_pConfig)
		return false;

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipAccessorySlotClickShortcut(this));

	return true;
}

void CDlgEquipAccessorySlot::OnTick()
{
	CDlgBase::OnTick();
}

void CDlgEquipAccessorySlot::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	ClearDialog();
}

void CDlgEquipAccessorySlot::UpdateUI()
{
	ACHAR szText[30];
	a_sprintf(szText, _AL("%d%"), m_iMaterialNum);
	m_pMaterialNum->SetText(szText);

	m_pCost->SetColor(GetHostPlayer()->GetMoneyAmount() > (m_iCost * m_iMaterialNum) ? A3DCOLORRGB(0, 255, 0) : A3DCOLORRGB(255, 0, 0));
	m_pCost->SetText(GetGameUIMan()->GetFormatNumber(m_iCost * m_iMaterialNum));

	float ratio = m_iMaterialNum / (float)m_iMaxNumber * 100;
	a_sprintf(szText, _AL("%.2f%%"), ratio);
	m_pSuccessRatio->SetText(szText);

	int total = GetHostPlayer()->GetPack()->GetItemTotalNum(m_iSocketMaterial);
	bool bCostCovered = GetHostPlayer()->GetMoneyAmount() > (m_iCost * m_iMaterialNum) ? true : false;

	if (total >= m_iMaxNumber && bCostCovered)
	{
		GetDlgItem("Btn_Confirm")->Enable(true);
	}
	else
	{
		GetDlgItem("Btn_Confirm")->Enable(false);
	}
}

void CDlgEquipAccessorySlot::OnCommandSubtractItem(const char* szCommand)
{
	if (m_iMaxNumber > 0) {
		m_iMaterialNum--;
		if (m_iMaterialNum == 0)
			m_iMaterialNum = 1;

		UpdateUI();
	}
}

void CDlgEquipAccessorySlot::OnCommandAddItem(const char* szCommand)
{
	if (m_iMaxNumber > 0) {
		m_iMaterialNum++;

		int total = GetHostPlayer()->GetPack()->GetItemTotalNum(m_iSocketMaterial);
		
		if (m_iMaterialNum > total)
			m_iMaterialNum = total;

		if (m_iMaterialNum > m_iMaxNumber )
			m_iMaterialNum = m_iMaxNumber;
		
		UpdateUI();
	}
}

void CDlgEquipAccessorySlot::OnCommandCANCEL(const char* szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	Show(false);
}

void CDlgEquipAccessorySlot::OnCommandConfirm(const char* szCommand)
{
	CECIvtrItem* pItem = (CECIvtrItem*)m_pImg_Equip->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
	{
		GetGameSession()->c2s_SendCmdNPCSevMakeAccessorySlot(m_pImg_Equip->GetData(), pItem->GetTemplateID(), m_iSocketMaterial, m_iMaterialNum);
		OnEventLButtonDown(0, 0, NULL);
	}
}
void CDlgEquipAccessorySlot::ClearDialog() 
{
	m_pImg_Equip->SetDataPtr(NULL);

	m_pImg_Equip->SetCover(NULL, -1);
	m_pTxt_Name->SetText(_AL(""));
	m_pCurrentSlot->SetText(_AL("0"));

	m_pImg_Material->SetCover(NULL, -1);
	m_pMaterialNum->SetText(_AL(""));
	m_pMaxMaterial->SetText(_AL("0"));

	m_pCost->SetColor(A3DCOLORRGB(0, 255, 0));
	m_pCost->SetText(_AL("0"));
	m_pNextSlot->SetText(_AL("0"));

	ACHAR szText[30];
	a_sprintf(szText, _AL("%.2f%%"), 0.0f);
	m_pSuccessRatio->SetText(szText);

	m_iMaxNumber = 0;
	m_iMaterialNum = 0;
	m_iCost = 0;
}

void CDlgEquipAccessorySlot::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CECIvtrItem* pItem = (CECIvtrItem*)m_pImg_Equip->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
		pItem->Freeze(false);

	ClearDialog();

	GetGameUIMan()->PlayItemSound(pItem, false);
}

void CDlgEquipAccessorySlot::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Equip", only one available drag-drop target
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DECORATION )
	{
		CECIvtrItem* pItem = (CECIvtrItem*)m_pImg_Equip->GetDataPtr("ptr_CECIvtrItem");
		if (pItem)
			pItem->Freeze(false);

		this->SetEquip(iSrc);
	}
}

void CDlgEquipAccessorySlot::SetEquip(int iSrc)
{
	CECIvtrEquip* pItem = (CECIvtrEquip*)GetHostPlayer()->GetPack()->GetItem(iSrc);
	if (pItem->GetClassID() == CECIvtrItem::ICID_DECORATION && pItem->GetHoleNum() >= 4 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(886), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIOBJECT pObj = GetDlgItem("Img_Equip");
	pItem->Freeze(true);
	pObj->SetData(iSrc);
	pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_iLevel = ((CECIvtrDecoration*)pItem)->GetDBEssence()->level - 1;

	m_iSocketMaterial = m_pConfig->level_list[m_iLevel].hole_list[pItem->GetHoleNum()].require_item_id;


	int iMatSlot = GetHostPlayer()->GetPack()->FindItem(m_iSocketMaterial);
	CECIvtrItem* pMaterialItem = GetHostPlayer()->GetPack()->GetItem(iMatSlot);
	if (pMaterialItem) {
		m_pImg_Material->SetData(m_iSocketMaterial);
		m_pImg_Material->SetDataPtr(pMaterialItem, "ptr_CECIvtrItem");
		af_GetFileTitle(pMaterialItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Material->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	}


	pImage->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip*)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	ACHAR szText[30];
	a_sprintf(szText, _AL("%d"), pItem->GetHoleNum());
	m_pCurrentSlot->SetText(szText);
	m_pTxt_Name->SetText(pItem->GetName());
	
	m_iMaxNumber = m_pConfig->level_list[m_iLevel].hole_list[pItem->GetHoleNum()].require_item_count;
	m_iCost = m_pConfig->level_list[m_iLevel].hole_list[pItem->GetHoleNum()].fee / m_iMaxNumber;
	m_iMaterialNum = 1;


	a_sprintf(szText, _AL("%d"), m_iMaxNumber);
	m_pMaxMaterial->SetText(szText);

	a_sprintf(szText, _AL("%d"), (pItem->GetHoleNum()+1));
	m_pNextSlot->SetText(szText);

	UpdateUI();
	//a_sprintf(szText, _AL("%d"), m_iMaterialNum);
	//m_pMaterialNum->SetText(szText);

	//m_pCost->SetColor(GetHostPlayer()->GetMoneyAmount() > (m_iCost * m_iMaterialNum) ? A3DCOLORRGB(0, 255, 0) : A3DCOLORRGB(255, 0, 0));
	//m_pCost->SetText(GetGameUIMan()->GetFormatNumber(m_iCost * m_iMaterialNum));

	//float ratio = m_iMaterialNum / (float)m_iMaxNumber * 100;
	//a_sprintf(szText, _AL("%.2f%%"), ratio);
	//m_pSuccessRatio->SetText(szText);

	//int total = GetHostPlayer()->GetPack()->GetItemTotalNum(m_iSocketMaterial);
	//bool bCostCovered = GetHostPlayer()->GetMoneyAmount() > (m_iCost * m_iMaterialNum) ? true : false;

	//if (total >= m_iMaxNumber && bCostCovered )
	//{
	//	GetDlgItem("Btn_Confirm")->Enable(true);
	//}
	//else
	//{
	//	GetDlgItem("Btn_Confirm")->Enable(false);
	//}
}