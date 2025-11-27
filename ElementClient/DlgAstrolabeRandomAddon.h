#pragma once
#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIComboBox.h"
#include "AUICheckBox.h"
#include "EC_GPDataType.h"
#include "EC_IvtrAstrolabeItem.h"

class CDlgAstrolabeRandomAddon : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	PAUIIMAGEPICTURE m_pItemIcon;
	PAUILABEL m_pMaxPileText;
	PAUICOMBOBOX m_pNumStringsCombo;
	PAUIEDITBOX m_pMaxAttemptsText;
	PAUICHECKBOX m_pSwiftCheckbox;
	PAUISTILLIMAGEBUTTON m_pStartRandomButton;

	bool m_bRandomRunning;
	bool m_bNextReady;
	int m_iTimer;
	int m_iCount;

	CECIvtrAstrolabe* m_pCurrentItem;
	CECIvtrAstrolabeRandomAddonPill* m_pCurrentUsingItem;

	void ClearItemIcon();
	void SetItemIcon();
	std::vector<ASTROLABE_RANDOM_ADDON_ESSENCE*> DetermineValidItemIds();
	int FindUsableItemSlot();
	void OnCommandRandom(const char* szCommand);
	void DoRandom();

public:
	CECIvtrAstrolabeRandomAddonPill* FindUsableItem();
	void SetContext(CECIvtrAstrolabe* item, CECIvtrAstrolabeRandomAddonPill* pUsing);
	void UpdateNow(S2C::cmd_astrolabe_operate_result* pCmd);

	virtual void OnTick();

	CDlgAstrolabeRandomAddon();
	virtual ~CDlgAstrolabeRandomAddon();

};

