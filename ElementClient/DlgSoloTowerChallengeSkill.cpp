#include "DlgSoloTowerChallengeSkill.h"

#include "AUIStillImageButton.h"
#include "AUIDialog.h"

#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"
#include "AUIDef.h"

AUI_BEGIN_COMMAND_MAP(CDlgSoloTowerChallengeSkill, CDlgBase)
AUI_ON_COMMAND("Btn_RenewCredit", OnCommand_RestoreCredits)
AUI_ON_COMMAND("Btn_*", OnCommand_PurchaseSkill)
AUI_END_COMMAND_MAP()




CDlgSoloTowerChallengeSkill::CDlgSoloTowerChallengeSkill()
{

}

CDlgSoloTowerChallengeSkill::~CDlgSoloTowerChallengeSkill()
{

}

bool CDlgSoloTowerChallengeSkill::OnInitDialog()
{
	m_pTxtCredits = (PAUILABEL)GetDlgItem("Txt_Credit");
	for (int i = 0; i < SOLO_CHALLENGE_MAX_SKILLS_COUNT; i++) {
		char szName[20];
		sprintf(szName, "Btn_%d", i + 1);
		m_pBtnSkill[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);

		sprintf(szName, "Img_skill%d", i + 1);
		m_pImgSkill[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	}
	m_pBtnRestoreCredits = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_RenewCredit");
	m_pImgSkillHightlight = (PAUIIMAGEPICTURE)GetDlgItem("Img_SkillHighLight");

	return true;
}

void CDlgSoloTowerChallengeSkill::OnShowDialog()
{

}

void CDlgSoloTowerChallengeSkill::OnTick() 
{
	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	// Keep all skills data updated
	for (int i = 0; i < SOLO_CHALLENGE_MAX_SKILLS_COUNT; i++) {

		// Get cooltime for this skill
		int maxCooltime;
		int curCooltime = pHost->GetCoolTime(GP_CT_SOLO_CHALLENGE_BUFF_0 + i + 1, &maxCooltime);

		// Skill cooltime
		AUIClockIcon* pClock = m_pImgSkill[i]->GetClockIcon();
		if (curCooltime > 0) {
			pClock->SetProgressRange(0, maxCooltime);
			pClock->SetProgressPos(maxCooltime - curCooltime);
			pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
		}
		else {
			pClock = m_pImgSkill[i]->GetClockIcon();
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
		}

		// skills stack info
		if (soloTowerInfo.skillsStackNum[i] > 0) {
			ACString tmpString;
			m_pImgSkill[i]->SetText(tmpString.Format(L"%d", soloTowerInfo.skillsStackNum[i]));
		}
		else {
			m_pImgSkill[i]->SetText(_AL(""));
		}
	}

	// Update current points
	ACString scoreString;
	m_pTxtCredits->SetText(scoreString.Format(L"%d/%d", soloTowerInfo.currentScore, soloTowerInfo.totalScoreEarned));

	// Keep this dialog in sync with SoloTowerChallengeState Dialog (when state moves, this moves).
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_SoloTowerChallengeState");
	SetPosEx(pDlg->GetPos().x + pDlg->GetRect().Width(), 0, 0, alignMin, pDlg);

}

/// <summary>
/// Command handler: Restore credits button
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeSkill::OnCommand_RestoreCredits(const char* szCommand)
{
	GetGameSession()->c2s_CmdSoloChallengeOperateRequest(3, NULL);
}

/// <summary>
/// Command handler: Purchase skill (skill button)
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeSkill::OnCommand_PurchaseSkill(const char* szCommand)
{
	using namespace C2S;

	int slotIndex = atoi(szCommand + strlen("Btn_")) - 1;

	cmdopt_solo_challenge_opt_score_cost scoreCost = { slotIndex };

	GetGameSession()->c2s_CmdSoloChallengeOperateRequest(2, &scoreCost);
}
