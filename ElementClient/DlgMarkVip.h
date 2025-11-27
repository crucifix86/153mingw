#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUIListBox.h"
#include "AUICheckBox.h"
#include "AUILabel.h"


/*
* This dialgo is in charge fo Quick Teleport system
* It is currently required to be VIP (with a given level) to access this feature
* It gets accessed from a Shortcut in Actions window (R)
* 
* Allows to save map positions (up to 10 depending of the VIP level)
* and using "energy" to teleport to those saved positions later
* 
* Actions:
* 
* Add position
* Rename position
* Delete position
* Teleport to position
*/
class CDlgMarkVip : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
public:
	CDlgMarkVip();
	virtual ~CDlgMarkVip();

	void OnCommand_CANCEL(const char* szCommand);
	void OnCommand_Add(const char* szCommand);
	void OnCommand_Delete(const char* szCommand);
	void OnCommand_Rename(const char* szCommand);
	void OnCommand_Teleport(const char* szCommand);
	void OnCommand_CheckNoConfirmation(const char* szCommand);

	void OnEventLButtonDBClick_ListLocation(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnEventLButtonUp_ListLocation(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	void UpdateLocationsList();
	void OnNameInput(int type, const ACString& strName);
	void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	void DoDelete();
	void DoTeleport();
	void PreTeleport();

protected:
	PAUILISTBOX m_pListBoxLocations; // List of stored positions (visual representation of HostPlayer.m_FixPositionList, but with different index)
	PAUISTILLIMAGEBUTTON m_pBtnRename;
	PAUISTILLIMAGEBUTTON m_pBtnAdd;
	PAUISTILLIMAGEBUTTON m_pBtnDelete;
	PAUISTILLIMAGEBUTTON m_pBtnTeleport;
	PAUILABEL m_pLblEnergy; // Displays amount of Energy that player has.
	PAUILABEL m_pLblNumLocations; // Displays the amount of locations used from the max amount of locations the player can save.
	PAUICHECKBOX m_pCheckTeleportWithNoConfirmation; // Checkbox wheter or not to ask for confirmation when teleporting
	int m_iSelectedIndex; // Stores the actual teleportID given by server, corresponding to an index at HostPlayer.m_FixPositionList

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
