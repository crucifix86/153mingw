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
#include "DlgAstrolabeRandomAddon.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAstrolabeRandomAddon, CDlgBase)
AUI_ON_COMMAND("Btn_ClickRandom", OnCommandRandom)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAstrolabeRandomAddon, CDlgBase)
AUI_END_EVENT_MAP()


CDlgAstrolabeRandomAddon::CDlgAstrolabeRandomAddon() :
	m_bRandomRunning(false),
	m_bNextReady(false),
	m_iTimer(0),
	m_iCount(0)
{
}

CDlgAstrolabeRandomAddon::~CDlgAstrolabeRandomAddon()
{

}

bool CDlgAstrolabeRandomAddon::OnInitDialog()
{
	m_pItemIcon = (PAUIIMAGEPICTURE)GetDlgItem("Item_01");
	m_pMaxPileText = (PAUILABEL)GetDlgItem("Txt_MaxRandTimes");
	m_pNumStringsCombo = (PAUICOMBOBOX)GetDlgItem("Combo_MinAddOnCount");
	m_pMaxAttemptsText = (PAUIEDITBOX)GetDlgItem("Edt_MaxTryTimes");
	m_pSwiftCheckbox = (PAUICHECKBOX)GetDlgItem("Chk_FastRandom");
	m_pStartRandomButton = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_ClickRandom");

	ACString tmp;
	for (int i = 0; i < 10; i++) {
		tmp.Format(_AL("%d"), (i + 1));
		m_pNumStringsCombo->AddString(tmp);
	}
	
	return true;
}

void CDlgAstrolabeRandomAddon::OnTick()
{
	CDlgBase::OnTick();

	if (m_bRandomRunning) {
		// Once response is received, count 3 seconds before next roll
		if (m_bNextReady) {
			m_iTimer += GetGame()->GetRealTickTime();
			if (m_iTimer > 3000) {
				m_bNextReady = false;
				m_iTimer = 0;
				m_iCount--;
				if (m_iCount) {
					CECHostPlayer* pPlayer = GetHostPlayer();
					CECInventory* pInv = pPlayer->GetPack();

					int iSlot = FindUsableItemSlot();
					CECIvtrAstrolabePointIncPill* item = (CECIvtrAstrolabePointIncPill*)pInv->GetItem(iSlot);
					if (item) {
						ACString tmp;
						if (m_iCount > 1) {
							tmp.Format(_AL("%d"), (m_iCount-1));
							m_pMaxAttemptsText->SetText(tmp);
						}
						DoRandom();
					}
					else {
						m_bRandomRunning = false;
						m_pStartRandomButton->SetText(GetStringFromTable(11514));
						m_pMaxAttemptsText->SetText(_AL("1"));
					}
				}
				else {
					m_bRandomRunning = false;
					m_pStartRandomButton->SetText(GetStringFromTable(11514));
					m_pMaxAttemptsText->SetText(_AL("1"));
				}
			}
		}
	}
}

void CDlgAstrolabeRandomAddon::OnShowDialog()
{
	CDlgBase::OnShowDialog();

}

void CDlgAstrolabeRandomAddon::ClearItemIcon()
{
	m_pItemIcon->ClearCover();
	m_pItemIcon->SetText(_AL(""));
	m_pItemIcon->SetDataPtr(NULL);
	m_pItemIcon->SetColor(A3DCOLORRGB(255, 255, 255));
}

void CDlgAstrolabeRandomAddon::SetItemIcon()
{
	AString strFile;
	ACHAR szText[40];

	if (m_pCurrentUsingItem) {

		m_pItemIcon->SetDataPtr(m_pCurrentUsingItem, "ptr_CECIvtrItem");
		m_pItemIcon->SetData(CECGameUIMan::ICONS_INVENTORY);
		if (m_pCurrentUsingItem->GetCount() > 1)
		{
			a_sprintf(szText, _AL("%d"), m_pCurrentUsingItem->GetCount());
			m_pItemIcon->SetText(szText);
			m_pMaxPileText->SetText(szText);

		}
		else {
			m_pItemIcon->SetText(_AL(""));
			m_pMaxPileText->SetText(_AL("1"));
		}
		

		af_GetFileTitle(m_pCurrentUsingItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pItemIcon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	}
	else {
		a_sprintf(szText, _AL("%d"), 1);
		m_pMaxPileText->SetText(szText);
	}
}

std::vector<ASTROLABE_RANDOM_ADDON_ESSENCE*> CDlgAstrolabeRandomAddon::DetermineValidItemIds()
{
	DATA_TYPE DataType;
	unsigned int tid = GetGame()->GetElementDataMan()->get_first_data_id(ID_SPACE_ESSENCE, DataType);

	int iType = 0;

	std::vector<ASTROLABE_RANDOM_ADDON_ESSENCE*> items;

	while (tid)
	{
		if (DataType == DT_ASTROLABE_RANDOM_ADDON_ESSENCE)
		{
			ASTROLABE_RANDOM_ADDON_ESSENCE* pItem = (ASTROLABE_RANDOM_ADDON_ESSENCE*)GetGame()->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

			if (pItem)
			{
				items.push_back(pItem);
			}
		}
		tid = GetGame()->GetElementDataMan()->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}

	return items;
}

int CDlgAstrolabeRandomAddon::FindUsableItemSlot()
{
	CECIvtrAstrolabePointIncPill* pItem = NULL;
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();
	int iSlot = -1;
	std::vector<ASTROLABE_RANDOM_ADDON_ESSENCE*> validIdList = DetermineValidItemIds();

	for (auto item : validIdList) {
		iSlot = pInv->FindItem(item->id);
		if (iSlot != -1) {
			break;
		}
	}
	return iSlot;
}

CECIvtrAstrolabeRandomAddonPill* CDlgAstrolabeRandomAddon::FindUsableItem()
{
	CECIvtrAstrolabeRandomAddonPill* pItem = NULL;
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	int iSlot = FindUsableItemSlot();
	if (iSlot >= 0)
		pItem = (CECIvtrAstrolabeRandomAddonPill*)pInv->GetItem(iSlot);

	return pItem;
}

void CDlgAstrolabeRandomAddon::SetContext(CECIvtrAstrolabe* item, CECIvtrAstrolabeRandomAddonPill* pUsing)
{
	ACString tmp;

	m_pCurrentItem = item;
	m_pCurrentUsingItem = pUsing;
	SetItemIcon();

	tmp.Format(_AL("%d"), m_pCurrentUsingItem->GetCount());
	m_pMaxPileText->SetText(tmp);

	m_pNumStringsCombo->SetCurSel(0);
	m_pMaxAttemptsText->SetText(_AL("1"));

}

void CDlgAstrolabeRandomAddon::OnCommandRandom(const char* szCommand)
{
	bool swift = m_pSwiftCheckbox->IsChecked();
	
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	int iSlot = FindUsableItemSlot();
	m_pCurrentUsingItem = (CECIvtrAstrolabeRandomAddonPill*)pInv->GetItem(iSlot);

	if (!m_pCurrentUsingItem) {
		PAUIDIALOG MsgBox;
		GetGameUIMan()->MessageBox("Game_Horoscope_PillMissing", GetStringFromTable(11527), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
		MsgBox->SetLife(3);

		return;
	}

	if (m_bRandomRunning) {
		m_bRandomRunning = false;
		m_pStartRandomButton->SetText(GetStringFromTable(11514));
	}
	else {
		m_iCount = a_atoi(m_pMaxAttemptsText->GetText());

		// Dont allow max value to exceed item count
		if (m_iCount == 0) {
			PAUIDIALOG MsgBox;
			GetGameUIMan()->MessageBox("Game_Horoscope_PillMissing", GetStringFromTable(11527), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
			MsgBox->SetLife(3);
		}
		else if (m_iCount > m_pCurrentUsingItem->GetCount()) {
			m_pMaxAttemptsText->SetFocus(true);
		}
		else {

			if (swift) {
				// swiftly just does 1 call
				DoRandom();
			}
			else {
				if (m_iCount) {
					ACString tmp;
					m_bRandomRunning = true;
					m_bNextReady = false;
					m_iTimer = 0;
					
					m_pStartRandomButton->SetText(GetStringFromTable(11515));
					if (m_iCount != 0) {
						tmp.Format(_AL("%d"), m_iCount);
						m_pMaxAttemptsText->SetText(tmp);
					}
					DoRandom();
				}
			}
		}
	}
}

void CDlgAstrolabeRandomAddon::DoRandom()
{
	bool swift = m_pSwiftCheckbox->IsChecked();

	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	int iSlot = FindUsableItemSlot();
	m_pCurrentUsingItem = (CECIvtrAstrolabeRandomAddonPill*)pInv->GetItem(iSlot);
	int limit = m_pNumStringsCombo->GetCurSel() + 1;
	int count = m_bRandomRunning ? 1 : m_iCount;
	GetGameSession()->c2s_CmdAstrolabeRandomAddon(count, limit, iSlot, m_pCurrentUsingItem->GetDBEssence()->id);
}

void CDlgAstrolabeRandomAddon::UpdateNow(S2C::cmd_astrolabe_operate_result* pCmd) {
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();

	// Update the count or clear if no longer valid
	int iSlot = FindUsableItemSlot();
	CECIvtrAstrolabeRandomAddonPill* item = (CECIvtrAstrolabeRandomAddonPill*)pInv->GetItem(iSlot);
	// out of items or reached max strings
	if (item == NULL || (item != m_pCurrentUsingItem) || item->GetCount() == 0 || pCmd->args[2] == 1) {
		m_pCurrentUsingItem = item;
		ClearItemIcon();
		if (m_bRandomRunning ) {
			m_pStartRandomButton->SetText(GetStringFromTable(11514));
			m_bRandomRunning = false;
			m_pMaxAttemptsText->SetText(_AL("1"));
		}
		SetItemIcon();
	}
	else {
		SetItemIcon();
	}
	m_bNextReady = true;
}

