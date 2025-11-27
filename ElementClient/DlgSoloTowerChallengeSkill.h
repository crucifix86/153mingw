#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

enum
{
	SOLO_CHALLENGE_MAX_SKILLS_COUNT = 16,
};

/// <summary>
/// This dialog class is in charge of handling event points and using those points to get specific buffs
/// </summary>
class CDlgSoloTowerChallengeSkill : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
public:
	CDlgSoloTowerChallengeSkill();
	virtual ~CDlgSoloTowerChallengeSkill();
	void OnCommand_RestoreCredits(const char* szCommand);
	void OnCommand_PurchaseSkill(const char* szCommand);

protected:

	PAUISTILLIMAGEBUTTON m_pBtnSkill[SOLO_CHALLENGE_MAX_SKILLS_COUNT]; // Button for each skill (on top of each skill image)
	PAUIIMAGEPICTURE m_pImgSkill[SOLO_CHALLENGE_MAX_SKILLS_COUNT]; // Actual skill images
	PAUIIMAGEPICTURE m_pImgSkillHightlight; // Dont know, didnt use it.
	PAUILABEL m_pTxtCredits; // Show amount of credits xx/xx
	PAUISTILLIMAGEBUTTON m_pBtnRestoreCredits; // Button to reset buffs and restore credits

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
