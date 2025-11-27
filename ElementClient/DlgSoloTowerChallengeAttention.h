#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"

/*
* This Dialog is a popup message that appears when player
* attempts to move to a new stage in Solo Tower Challenge
* but didnt claim prizes for current stage.
* 
* It let player continue to next layer or open Awards dialog to claim prizes.
*/
class CDlgSoloTowerChallengeAttention : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
public:
	CDlgSoloTowerChallengeAttention();
	virtual ~CDlgSoloTowerChallengeAttention();

	void OnCommand_SkipAwards(const char* szCommand);
	void OnCommand_GetAwards(const char* szCommand);

protected:

	PAUISTILLIMAGEBUTTON m_pBtnSkipAwards; // Skip awards and go to the next layer
	PAUISTILLIMAGEBUTTON m_pBtnGetAwards; // Open the Awards dialog and hide this one

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
