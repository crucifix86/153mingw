#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include "AUILabel.h"
#include "AUITextArea.h"
#include "AUIEditBox.h"

enum
{
	SOLO_CHALLENGE_NOTIFY_START_SUCCESS = 0,
	SOLO_CHALLENGE_NOTIFY_START_FAILED,
	SOLO_CHALLENGE_NOTIFY_COMPLETE_SUCCESS,
	SOLO_CHALLENGE_NOTIFY_COMPLETE_FAILED,
};

/// <summary>
/// This dialog class is in charge of displaying main information for Solo Tower Challenge event
/// This Dialog appears only when entering a given map (or set of maps actually)
/// </summary>
class CDlgSoloTowerChallengeState : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
public:
	CDlgSoloTowerChallengeState();
	virtual ~CDlgSoloTowerChallengeState();

	void OnCommand_ViewAwards(const char* szCommand);
	void OnCommand_ViewSelectLayer(const char* szCommand);
	void OnCommand_ViewSkillChoose(const char* szCommand);
	ACString GetTimeStringFromSeconds(int seconds);
	void ChangeChallengeState(int newState, int currentLayer);
	void SetChallengeTask(unsigned long idTask);

protected:

	PAUILABEL m_pLblLayer; // Displays the layer (only while player is actually running that stage)
	PAUILABEL m_pLblTime; // Current time (while runing the stage)
	PAUISTILLIMAGEBUTTON m_pBtnSelectLayer; // Open the SelectLayer Dialog
	PAUISTILLIMAGEBUTTON m_pBtnViewAwards; // Opens the Awards Dialog (if there are rewards to claim)
	PAUICHECKBOX m_pChkSkillChoose; // Opens the skills dialog
	PAUITEXTAREA m_pTxtHelp; // It shows quest text when running a stage
	PAUIEDITBOX m_pEdiHelp; // background image for help text

	int m_nChallengeState; // It reflects current challenge state.
	int m_nLayerCurrentTime; // Current time for running stage
	int m_nCurrentLayer; // Current stage (layer)
	DWORD m_dwLastTickTime; // keeps tick info
	int m_nTowerTaskId; // This is the current TaskId related to the active stage.

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
