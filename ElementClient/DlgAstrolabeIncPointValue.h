#pragma once

#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrAstrolabeItem.h"

class CDlgAstrolabeIncPointValue : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	PAUIIMAGEPICTURE m_pItemIcon;
	PAUISTILLIMAGEBUTTON m_pMultiShiftButton;

	CECIvtrAstrolabePointIncPill* m_pCurrentUsingItem;
	CECIvtrAstrolabe* m_pCurrentItem;
	int m_iSlot;

	void ClearItemIcon();
	void SetItemIcon();

	void OnCommandShift(const char* szCommand);
	void OnCommandMultiShift(const char* szCommand);
	

	bool m_bMultiShiftActive;
	bool m_bNextShiftReady;

	std::vector<ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*> DetermineValidItemIds(int apt);
	int FindUsableItemSlot(int apt);
	bool DoShift();

public:
	virtual void OnTick();
	void SetContext(CECIvtrAstrolabe* item, CECIvtrAstrolabePointIncPill* pUsing);

	CDlgAstrolabeIncPointValue();
	virtual ~CDlgAstrolabeIncPointValue();

	CECIvtrAstrolabePointIncPill* CDlgAstrolabeIncPointValue::FindUsableItem(int apt);
	void UpdateNow();
	void OnConfirmShift();
};

