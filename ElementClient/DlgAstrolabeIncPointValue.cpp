
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "CSplit.h"
#include "EC_TimeSafeChecker.h"
#include "AFI.h"
#include "A3DGFXExMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "EC_FixedMsg.h"
#include "EC_Player.h"
#include "DlgAstrolabe.h"
#include "DlgAstrolabeIncPointValue.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAstrolabeIncPointValue, CDlgBase)
AUI_ON_COMMAND("Btn_IncOnce", OnCommandShift)
AUI_ON_COMMAND("Btn_BatchInc", OnCommandMultiShift)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAstrolabeIncPointValue, CDlgBase)
AUI_END_EVENT_MAP()


CDlgAstrolabeIncPointValue::CDlgAstrolabeIncPointValue() : 
	m_pCurrentItem(NULL), 
	m_iSlot(-1),
	m_bMultiShiftActive(false),
	m_bNextShiftReady(false)
{
}

CDlgAstrolabeIncPointValue::~CDlgAstrolabeIncPointValue()
{

}

bool CDlgAstrolabeIncPointValue::OnInitDialog()
{
	m_pItemIcon = (PAUIIMAGEPICTURE)GetDlgItem("Item_01");
	m_pMultiShiftButton = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_BatchInc");
	return true;
}

void CDlgAstrolabeIncPointValue::OnTick()
{
	if (m_bMultiShiftActive) {
		if (m_bNextShiftReady) {
			m_bNextShiftReady = false;

			CECHostPlayer* pPlayer = GetHostPlayer();
			CECInventory* pInv = pPlayer->GetPack();

			int iSlot = FindUsableItemSlot(m_pCurrentItem->SumAptit());
			CECIvtrAstrolabePointIncPill* item = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);
			if ( item )
				DoShift();
			else
			{
				UpdateNow();
			}
		}
	}
	CDlgBase::OnTick();
}

void CDlgAstrolabeIncPointValue::OnShowDialog()
{
	CDlgBase::OnShowDialog();
}

void CDlgAstrolabeIncPointValue::SetContext(CECIvtrAstrolabe* item, CECIvtrAstrolabePointIncPill* pUsing)
{
	m_pCurrentItem = item;
	m_pCurrentUsingItem = pUsing;
	SetItemIcon();
}

void CDlgAstrolabeIncPointValue::ClearItemIcon() 
{
	m_pItemIcon->ClearCover();
	m_pItemIcon->SetText(_AL(""));
	m_pItemIcon->SetDataPtr(NULL);
	m_pItemIcon->SetColor(A3DCOLORRGB(255, 255, 255));
}

void CDlgAstrolabeIncPointValue::SetItemIcon() 
{
	AString strFile;
	ACHAR szText[40];

	m_pItemIcon->SetDataPtr(m_pCurrentUsingItem, "ptr_CECIvtrItem");
	m_pItemIcon->SetData(CECGameUIMan::ICONS_INVENTORY);
	if (m_pCurrentUsingItem->GetCount() > 1)
	{
		a_sprintf(szText, _AL("%d"), m_pCurrentUsingItem->GetCount());
		m_pItemIcon->SetText(szText);
	}
	else
		m_pItemIcon->SetText(_AL(""));

	af_GetFileTitle(m_pCurrentUsingItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pItemIcon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
}

std::vector<ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*> CDlgAstrolabeIncPointValue::DetermineValidItemIds(int apt)
{
	DATA_TYPE DataType;
	unsigned int tid = GetGame()->GetElementDataMan()->get_first_data_id(ID_SPACE_ESSENCE, DataType);

	int iType = 0;

	std::vector<ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*> items;

	while (tid)
	{
		if (DataType == DT_ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE)
		{
			ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE* pItem = (ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*)GetGame()->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

			if (pItem)
			{
				if (apt >= pItem->require_min_all_inner_point_value && apt <= pItem->require_max_all_inner_point_value) {
					items.push_back(pItem);
				}
			}
		}
		tid = GetGame()->GetElementDataMan()->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}

	// sort them by max requirement so least is first
	std::sort(items.begin(), items.end(), [](ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE* pItem, ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE* pItem2) -> bool {
		return pItem->require_max_all_inner_point_value < pItem2->require_max_all_inner_point_value;
		});

	return items;
}

int CDlgAstrolabeIncPointValue::FindUsableItemSlot(int apt) 
{
	CECIvtrAstrolabePointIncPill* pItem = NULL;
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();
	int iSlot = -1;
	std::vector<ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*> validIdList = DetermineValidItemIds(apt);

	for (auto item : validIdList) {
		iSlot = pInv->FindItem(item->id);
		if (iSlot != -1) {
			break;
		}
	}
	return iSlot;
}

CECIvtrAstrolabePointIncPill* CDlgAstrolabeIncPointValue::FindUsableItem(int apt) 
{
	CECIvtrAstrolabePointIncPill* pItem = NULL;
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	int iSlot = FindUsableItemSlot(apt);
	if ( iSlot >= 0 )
		pItem = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);	

	return pItem;
}

void CDlgAstrolabeIncPointValue::OnCommandShift(const char* szCommand)
{
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	int iSlot = FindUsableItemSlot(m_pCurrentItem->SumAptit());
	m_pCurrentUsingItem = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);

	if(m_pCurrentItem){
	PAUIDIALOG MsgBox;
	ACString tmp;
	tmp.Format(GetStringFromTable(11511), 1, m_pCurrentUsingItem->GetDBEssence()->name, m_pCurrentItem->GetName());
	GetGameUIMan()->MessageBox("Game_Starshift_Use", tmp, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
	}

//	DoShift();
}

void CDlgAstrolabeIncPointValue::OnConfirmShift()
{
	DoShift();
}

void CDlgAstrolabeIncPointValue::OnCommandMultiShift(const char* szCommand)
{
	if (!m_bMultiShiftActive) {
		m_bMultiShiftActive = true;
		m_bNextShiftReady = true;

		m_pMultiShiftButton->SetText(GetStringFromTable(11533));
	}
	else {
		m_bMultiShiftActive = false;
		m_bNextShiftReady = false;
		m_pMultiShiftButton->SetText(GetStringFromTable(11532));
	}
}

bool CDlgAstrolabeIncPointValue::DoShift() 
{
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();
	
	int iSlot = FindUsableItemSlot(m_pCurrentItem->SumAptit());
	m_pCurrentUsingItem = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);

	if (m_pCurrentUsingItem == NULL) {
		PAUIDIALOG MsgBox;
		GetGameUIMan()->MessageBox("Game_Stargaze_PowderMissing", GetStringFromTable(11510), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
		MsgBox->SetLife(3);
		return false;
	}
	else {
		m_bNextShiftReady = false;
		GetGameSession()->c2s_CmdAstrolabeIncPoint(iSlot, m_pCurrentUsingItem->GetDBEssence()->id);
		return true;
	}
}

void CDlgAstrolabeIncPointValue::UpdateNow() {
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	// Update the count or clear if no longer valid
	int iSlot = FindUsableItemSlot(m_pCurrentItem->SumAptit());
	CECIvtrAstrolabePointIncPill* item = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);
	if (item == NULL || (item != m_pCurrentUsingItem) || item->GetCount() == 0) {
		ClearItemIcon();
		if (m_bMultiShiftActive) {
			m_pMultiShiftButton->SetText(GetStringFromTable(11532));
			m_bMultiShiftActive = false;
		}
	}
	else {
		SetItemIcon();
	}
	m_bNextShiftReady = true;
}
