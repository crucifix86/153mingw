#include "DlgSoloTowerChallengeState.h"

#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIDef.h"
#include "EC_TaskInterface.h"
#include "TaskTemplMan.h"

#include "DlgSoloTowerChallengeSelectLayer.h"
#include "DlgSoloTowerChallengeSkill.h"
#include "DlgSoloTowerChallengeAward.h"
#include "DlgSoloTowerChallengeAttention.h"

AUI_BEGIN_COMMAND_MAP(CDlgSoloTowerChallengeState, CDlgBase)
AUI_ON_COMMAND("Btn_SelectLayer", OnCommand_ViewSelectLayer)
AUI_ON_COMMAND("Btn_ViewAward", OnCommand_ViewAwards)
AUI_ON_COMMAND("Chk_SkillChoose", OnCommand_ViewSkillChoose)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSoloTowerChallengeState, CDlgBase)
AUI_END_EVENT_MAP()



CDlgSoloTowerChallengeState::CDlgSoloTowerChallengeState()
{

}

CDlgSoloTowerChallengeState::~CDlgSoloTowerChallengeState()
{

}

bool CDlgSoloTowerChallengeState::OnInitDialog()
{
	m_pLblLayer = (PAUILABEL)GetDlgItem("Txt_Layer"); // Text where layer (stage) level will be displayed
	m_pLblTime = (PAUILABEL)GetDlgItem("Txt_Time"); // Text where time of current layer is displayed
	m_pBtnSelectLayer = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SelectLayer"); // Button to open the Select Layer interface
	m_pBtnViewAwards = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_ViewAward"); // Button to open the Awards selection interface
	m_pChkSkillChoose = (PAUICHECKBOX)GetDlgItem("Chk_SkillChoose"); // Button to open the Skill selection interface
	m_pTxtHelp = (PAUITEXTAREA)GetDlgItem("Txt_Help"); // Text where help of current layer is displayed (how to complete the stage)
	m_pEdiHelp = (PAUIEDITBOX)GetDlgItem("Edi_Help"); // background box where help of current layer is displayed (how to complete the stage)

	m_nLayerCurrentTime = 0;
	m_nChallengeState = -1;
	m_nTowerTaskId = 0;

	return true;
}

void CDlgSoloTowerChallengeState::OnShowDialog()
{
	m_dwLastTickTime = GetTickCount();
}

void CDlgSoloTowerChallengeState::OnTick() {

	DWORD dwTick = GetTickCount();

	// If challenge is running, keep record of time
	if (m_nChallengeState == SOLO_CHALLENGE_NOTIFY_START_SUCCESS) {
		DWORD timePassed = (dwTick >= m_dwLastTickTime) ? (dwTick - m_dwLastTickTime) : 0;

		m_nLayerCurrentTime += timePassed;

	}
	else {
		// If its any other state, clear information
		m_pLblLayer->SetText(L"");
		m_nLayerCurrentTime = 0;
		m_pTxtHelp->Show(false);
		m_pEdiHelp->Show(false);
	}

	// Show running time
	m_pLblTime->SetText(GetTimeStringFromSeconds(m_nLayerCurrentTime / 1000));

	m_dwLastTickTime = dwTick;
}

/// <summary>
/// Command handler: Button Select Layer
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeState::OnCommand_ViewSelectLayer(const char* szCommand)
{
	CECHostPlayer* pHost = GetHostPlayer();

	GetGameUIMan()->m_pDlgSoloTowerChallengeSelectLayer->SwitchShow();
}

/// <summary>
/// Command handler: Awards button
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeState::OnCommand_ViewAwards(const char* szCommand)
{
	CECHostPlayer* pHost = GetHostPlayer();
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();
	// Check if player has rewards to claim or claimed (and see), in order to open this Dialog
	if (soloTowerInfo.awardInfo.layer == m_nCurrentLayer && soloTowerInfo.awardInfo.totalDrawItemTimes > 0 && !GetGameUIMan()->m_pDlgSoloTowerChallengeAward->IsShow())
		GetGameUIMan()->m_pDlgSoloTowerChallengeAward->Show(true);
}

/// <summary>
/// Command handler: Skills button
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeState::OnCommand_ViewSkillChoose(const char* szCommand)
{
	GetGameUIMan()->m_pDlgSoloTowerChallengeSkill->SwitchShow();
}

/// <summary>
/// It is called from EC_HostMsg.cpp when receiving message from server regarind state
/// OnMsgSoloChallangeStateNotify()
/// </summary>
/// <param name="newState"></param>
/// <param name="currentLayer"></param>
void CDlgSoloTowerChallengeState::ChangeChallengeState(int newState, int currentLayer)
{
	m_nChallengeState = newState;

	switch (m_nChallengeState) {
		case SOLO_CHALLENGE_NOTIFY_START_SUCCESS:
		{
			m_nCurrentLayer = currentLayer;
			ACString strTemp;
			m_pLblLayer->SetText(strTemp.Format(GetStringFromTable(11540), m_nCurrentLayer));
			m_nLayerCurrentTime = 0;
			break;
		}
		case SOLO_CHALLENGE_NOTIFY_COMPLETE_SUCCESS:
		{
			break;
		}
		case SOLO_CHALLENGE_NOTIFY_START_FAILED:
		case SOLO_CHALLENGE_NOTIFY_COMPLETE_FAILED:
		default:
		{
			break;
		}
	}

}

/// <summary>
/// This function is called from EC_GameUIMan.cpp, when client receives a new quest and identifies it as TowerTask task.
/// It means that this task is a challenge task. We will use it to display quest information in Help text
/// </summary>
/// <param name="idTask"></param>
void CDlgSoloTowerChallengeState::SetChallengeTask(unsigned long idTask)
{
	m_nTowerTaskId = idTask;

	ATaskTemplMan* pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl* pTemp = pMan->GetTaskTemplByID(idTask);

	m_pTxtHelp->SetText(pTemp->GetDescription());
	m_pTxtHelp->Show(true);
	m_pEdiHelp->Show(true);
}

/// <summary>
/// Format time to display as current stage time
/// </summary>
/// <param name="seconds"></param>
/// <returns></returns>
ACString CDlgSoloTowerChallengeState::GetTimeStringFromSeconds(int seconds)
{
	int minutes = seconds / 60;
	int remainingSeconds = seconds - minutes * 60;

	ACString tmpString;
	tmpString.Format(L"%02d:%02d", minutes, remainingSeconds);
	return tmpString;
}
