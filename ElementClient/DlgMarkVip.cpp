#include "DlgMarkVip.h"

#include "AUIStillImageButton.h"
#include "AUIDialog.h"

#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "AUIDef.h"

#include "DlgInputName.h"

AUI_BEGIN_COMMAND_MAP(CDlgMarkVip, CDlgBase)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_ON_COMMAND("arrow", OnCommand_Add)
AUI_ON_COMMAND("delete", OnCommand_Delete)
AUI_ON_COMMAND("edit", OnCommand_Rename)
AUI_ON_COMMAND("Btn_Transfer", OnCommand_Teleport)
AUI_ON_COMMAND("Chk_Attention", OnCommand_CheckNoConfirmation)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgMarkVip, CDlgBase)
AUI_ON_EVENT("List_Choose", WM_LBUTTONDBLCLK, OnEventLButtonDBClick_ListLocation)
AUI_ON_EVENT("List_Choose", WM_LBUTTONUP, OnEventLButtonUp_ListLocation)
AUI_END_EVENT_MAP()

CDlgMarkVip::CDlgMarkVip()
{

}

CDlgMarkVip::~CDlgMarkVip()
{

}

bool CDlgMarkVip::OnInitDialog()
{
	//
	m_pLblEnergy = (PAUILABEL)GetDlgItem("Txt_Energy"); // Text where Energy is displayed
	m_pLblNumLocations = (PAUILABEL)GetDlgItem("Txt_Volume"); // Text where amount of teleports positions are used
	m_pListBoxLocations = (PAUILISTBOX)GetDlgItem("List_Choose"); // List of teleport positions
	m_pCheckTeleportWithNoConfirmation = (PAUICHECKBOX)GetDlgItem("Chk_Attention"); // checkbox for no confirmation when teleporting
	
	return true;
}

void CDlgMarkVip::OnShowDialog()
{
	UpdateLocationsList();
	m_pCheckTeleportWithNoConfirmation->Check(GetGame()->GetConfigs()->GetGameSettings().bMarkVipNoConfirmationTeleport);
}

void CDlgMarkVip::OnTick() {

	CECHostPlayer *pHost = GetHostPlayer();

	// Display Correct Energy
	int energy = pHost->GetFixPositionEnergy();
	ACString strText;
	m_pLblEnergy->SetText(strText.Format(_AL("%d"), energy));

	// Display amount of locations
	int countLocations = pHost->GetCountFixPositionList();
	int maxLocations = pHost->GetCashVipFixPositionNum();

	m_pLblNumLocations->SetText(strText.Format(_AL("%d/%d"), countLocations, maxLocations));

}

void CDlgMarkVip::OnCommand_CANCEL(const char* szCommand)
{
	Show(false);
}

void CDlgMarkVip::OnCommand_Add(const char* szCommand) 
{
	CECHostPlayer* pHost = GetHostPlayer();

	// Check if this VIP level allows player to add a new position
	int countLocations = pHost->GetCountFixPositionList();
	int maxLocations = pHost->GetCashVipFixPositionNum();
	if (pHost->GetCountFixPositionList() >= pHost->GetCashVipFixPositionNum()) {
		GetGameUIMan()->MessageBox("", GetStringFromTable(11716), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	///TODO There should be a way to check if position can be saved in this kind of map. 
	// Ofcourse map cant be used in instances, for example. But it seems like there are other specific open maps that cant be used.

	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_ADD_MARKIP);
}
void CDlgMarkVip::OnCommand_Delete(const char* szCommand) 
{
	// Show confirmation box for deleting a position in the list
	if (m_pListBoxLocations->GetCurSel() > -1) {
		m_pAUIManager->MessageBox("FixPositionDeleteConfirm", GetStringFromTable(11734), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgMarkVip::OnCommand_Rename(const char* szCommand)
{
	// Show a inputName dialog to get the string for the new position name
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_RENAME_MARKIP);
}
void CDlgMarkVip::OnCommand_Teleport(const char* szCommand)
{
	// Call teleport initial process
	PreTeleport();
}

void CDlgMarkVip::OnCommand_CheckNoConfirmation(const char* szCommand)
{
	// Store check value into Game settings
	EC_GAME_SETTING setting = GetGame()->GetConfigs()->GetGameSettings();
	setting.bMarkVipNoConfirmationTeleport = m_pCheckTeleportWithNoConfirmation->IsChecked();
	GetGame()->GetConfigs()->SetGameSettings(setting);
}

void CDlgMarkVip::OnEventLButtonDBClick_ListLocation(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	// Update the index that have just selected
	int listBoxCurSelection = m_pListBoxLocations->GetCurSel();
	if (listBoxCurSelection >= 0)
		m_iSelectedIndex = m_pListBoxLocations->GetItemData(listBoxCurSelection);

	// Call Teleport initial process
	PreTeleport();
}

void CDlgMarkVip::PreTeleport() {

	// Check if player has enough energy
	if (m_pLblEnergy == NULL) {
		GetGameUIMan()->MessageBox("", GetStringFromTable(11718), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	// If checked "no confirmation" perform teleport right away
	if (m_pCheckTeleportWithNoConfirmation->IsChecked()) {
		DoTeleport();
		return;
	}

	// Ask for confirmation to teleport
	m_pAUIManager->MessageBox("FixPositionTeleportConfirm", GetStringFromTable(11735), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgMarkVip::OnEventLButtonUp_ListLocation(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	// Update index selected (I think this might be unnecessary)

	int listBoxCurSelection = m_pListBoxLocations->GetCurSel();
	
	if (listBoxCurSelection >= 0)
		m_iSelectedIndex = m_pListBoxLocations->GetItemData(listBoxCurSelection);

}

/*
* This function is called everytime a packet is received from server regarding:
* ** Adding a new position
* ** Deleting a position
* ** Renaming a position
* 
* It refreshes the whole list of positions and updates current selected index
*/
void CDlgMarkVip::UpdateLocationsList() {

	CECHostPlayer* pHost = GetHostPlayer();

	// Clean listbox
	m_pListBoxLocations->ResetContent();

	// Populate listbox
	for (int i = 0; i < 10; i++) {
		const CECHostPlayer::FIX_POSITION_INFO * pFixPositionInfo = pHost->GetFixPositionInfoByIndex(i);

		if (pFixPositionInfo->world_tag == -1) continue;

		m_pListBoxLocations->AddString(pFixPositionInfo->positionName);
		m_pListBoxLocations->SetItemData(m_pListBoxLocations->GetCount() - 1, i);
	}

	// reselect
	int listBoxCurSelection = m_pListBoxLocations->GetCurSel();
	if (listBoxCurSelection >= 0)
		m_iSelectedIndex = m_pListBoxLocations->GetItemData(listBoxCurSelection);

}

/*
* This function is a callback function from DlgNameInput, when user is requested to 
* input a name for a position (when adding or renaming a position)
* 
* This function performs the actual call to server to perform these operations (add and rename)
* 
* @param type Is defined in DlgNameInput, an enum of the type of NameInput 
* @param strName The string entered in the box
* 
*/
void CDlgMarkVip::OnNameInput(int type, const ACString& strName)
{
	CECHostPlayer* pHost = GetHostPlayer();

	if (type == CDlgInputName::INPUTNAME_ADD_MARKIP)
	{
		A3DVECTOR3 pos = pHost->GetPos();
		GetGameSession()->c2s_CmdFixPositionTransmitOperateRequestAdd(pos, strName);	
	}
	else
	{
		GetGameSession()->c2s_CmdFixPositionTransmitOperateRequestRename(m_iSelectedIndex, strName);
	}
}

/*
* Function that performs the actual Delete operation, sending the packet to the server
*/
void CDlgMarkVip::DoDelete()
{
	GetGameSession()->c2s_CmdFixPositionTransmitOperateRequestDelete(m_iSelectedIndex);
}
/*
* Function that performs the actual Delete operation, sending the packet to the server
*/
void CDlgMarkVip::DoTeleport()
{
	GetGameSession()->c2s_CmdFixPositionTransmitOperateRequestTeleport(m_iSelectedIndex);
	Show(false);
}
