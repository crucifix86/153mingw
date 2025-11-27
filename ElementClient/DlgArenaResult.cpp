// Filename	: CDlgArenaResult.cpp
// Creator	: Adriano Lopes
// Date		: 2022/06/13


#include "DlgArenaResult.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_Resource.h"

#include "EC_HostPlayer.h"
#include "EC_Player.h"
#include "EC_Model.h"
#include "DlgArenaPoint.h"
#include "DlgArenaQueue.h"


AUI_BEGIN_COMMAND_MAP(CDlgArenaResult, CDlgBase)
AUI_ON_COMMAND("Btn_Departure", ExitArena)
AUI_ON_COMMAND("IDCANCEL", OnCommandCANCAL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgArenaResult, CDlgBase)
AUI_END_EVENT_MAP()

CDlgArenaResult::CDlgArenaResult()
{
	exitcount = 0;
	m_pBtn_Departure = NULL;
	m_pLst1 = NULL;
	m_pLst2 = NULL;

}

CDlgArenaResult::~CDlgArenaResult()
{
}

void CDlgArenaResult::OnCommandCANCAL(const char* szCommand)
{
}

bool CDlgArenaResult::OnInitDialog()
{
	DDX_Control("Btn_Departure", m_pBtn_Departure);
	DDX_Control("List_1", m_pLst1);
	DDX_Control("List_2", m_pLst2);
	m_pLst1->ResetContent();
	m_pLst2->ResetContent();

	GetDlgItem("Img_B66")->Show(false);
	GetDlgItem("Img_R66")->Show(false);
	GetDlgItem("Img_EnemyB66")->Show(false);
	GetDlgItem("Img_EnemyR66")->Show(false);
	GetDlgItem("Img_B33")->Show(false);
	GetDlgItem("Img_R33")->Show(false);
	GetDlgItem("Img_EnemyR33")->Show(false);
	GetDlgItem("Img_EnemyB33")->Show(false);
	return true;

}

void CDlgArenaResult::OnShowDialog()
{

}
void CDlgArenaResult::OnHideDialog()
{
}


void CDlgArenaResult::OnTick()
{

	int decreasedcount = exitcount - GetGame()->GetServerGMTTime();
	ACString str;
	if (decreasedcount < 0) {
		ClearArenaBattleInfo();
		str.Format(GetStringFromTable(20011));
		m_pBtn_Departure->SetText(str);
		Show(false);
	}
	else if(decreasedcount > 0){
		decreasedcount = abs(decreasedcount);
		str.Format(GetStringFromTable(20012), decreasedcount);
		m_pBtn_Departure->SetText(str);
	}
}

void CDlgArenaResult::ExitArena(const char* szCommand)
{
	ClearArenaBattleInfo();
	g_pGame->GetGameSession()->c2s_SendCmdArenaExit();
	Show(false);
}

void CDlgArenaResult::ClearArenaBattleInfo() {
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	static const char* szHeadHook = "HH_Head";

	CECHostPlayer::BATTLEINFO binfo = GetHostPlayer()->GetBattleInfo();
	binfo.nType = CECHostPlayer::BT_NONE;
	binfo.idBattle = 0;
	binfo.iEndTime = 0;
	GetHostPlayer()->RemoveGfx(res_GFXFile(RES_GFX_BLUEFLAGARENA), szHeadHook, PLAYERMODEL_TYPEALL);
	GetHostPlayer()->RemoveGfx(res_GFXFile(RES_GFX_REDFLAGARENA), szHeadHook, PLAYERMODEL_TYPEALL);
	
	GetHostPlayer()->SetBattleCamp(0);


	CDlgArenaPoint* dlg = dynamic_cast<CDlgArenaPoint*>(pGameUI->GetDialog("Win_ArenaPoint"));
	dlg->Show(false);
	dlg->HideEnemiesDlg();

	CDlgArenaQueue* dlg2 = dynamic_cast<CDlgArenaQueue*>(pGameUI->GetDialog("Win_ArenaQueue"));
	dlg2->Show(false);
}



void CDlgArenaResult::SetResultInfo(void* pData) {

	using namespace S2C;

	cmd_arena_end_result* pCmd = (cmd_arena_end_result*)pData;

	m_pLst1->ResetContent();
	m_pLst2->ResetContent();

	if (pCmd->BattleMode == 0) {
		if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
			GetDlgItem("Img_B33")->Show(true);
			GetDlgItem("Img_EnemyR33")->Show(true);
		}
		else {
			GetDlgItem("Img_R33")->Show(true);
			GetDlgItem("Img_EnemyB33")->Show(true);
		}
	}
	else if(pCmd->BattleMode == 1) {
		if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
			GetDlgItem("Img_B66")->Show(true);
			GetDlgItem("Img_EnemyR66")->Show(true);
		}
		else {
			GetDlgItem("Img_R66")->Show(true);
			GetDlgItem("Img_EnemyB66")->Show(true);
		}
	}

	ACString str;

	if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
		str.Format(GetStringFromTable(20014));
		GetDlgItem("Txt_TeamName1")->SetText(str);
		str.Format(GetStringFromTable(20013));
		GetDlgItem("Txt_TeamName2")->SetText(str);
	}
	else {
		str.Format(GetStringFromTable(20013));
		GetDlgItem("Txt_TeamName1")->SetText(str);
		str.Format(GetStringFromTable(20014));
		GetDlgItem("Txt_TeamName2")->SetText(str);
	}

	int allptsOffender = 0;
	int allptsDefender = 0;

	int pluscntOffender = 0;
	int pluscntDefender = 0;

	int idx = 0;

	for (int i = 0; i < pCmd->CountInvader; i++) {

		ACString strPlayerName((const ACHAR*)pCmd->members[idx].rolename, pCmd->members[idx].name_len / sizeof(ACHAR));
		str.Format(_AL("%s \t %s \t %d \t %d \t %s"),
			strPlayerName,
			GetGameRun()->GetProfName(pCmd->members[idx].occupation),
			pCmd->members[idx].victories,
			pCmd->members[idx].rank,
			L""
		);

		allptsOffender += pCmd->members[idx].rank;
		pluscntOffender += 5;

		if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
			m_pLst2->AddString(str);
		}
		else {
			m_pLst1->AddString(str);
		}
		idx++;
	}

	for (int i = 0; i < pCmd->CountDefender; i++) {
		ACString strPlayerName((const ACHAR*)pCmd->members[idx].rolename, pCmd->members[idx].name_len / sizeof(ACHAR));

		str.Format(_AL("%s \t %s \t %d \t %d \t %s"),
			strPlayerName,
			GetGameRun()->GetProfName(pCmd->members[idx].occupation),
			pCmd->members[idx].victories,
			pCmd->members[idx].rank,
			L""
		);
		allptsDefender += pCmd->members[idx].rank;
		pluscntDefender += 5;

		if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
			m_pLst1->AddString(str);
		}
		else {
			m_pLst2->AddString(str);
		}
		idx++;
	}

	if (GetHostPlayer()->GetBattleCamp() == GP_BATTLE_CAMP_DEFENDER) {
		str.Format(L"%d", pCmd->DefenderSurvivors);
		GetDlgItem("Txt_Alive1")->SetText(str);

		str.Format(L"%d", pCmd->InvaderSurvivors);
		GetDlgItem("Txt_Alive2")->SetText(str);

		str.Format(L"%d", pCmd->DefenderDmg);
		GetDlgItem("Txt_Attack1")->SetText(str);

		str.Format(L"%d", pCmd->InvaderDmg);
		GetDlgItem("Txt_Attack2")->SetText(str);

		if (pCmd->Winner == 1) {

			str.Format(GetStringFromTable(20016), allptsDefender, pluscntDefender);
			GetDlgItem("Txt_Score1")->SetText(str);

			str.Format(GetStringFromTable(20015), allptsOffender, pluscntOffender);
			GetDlgItem("Txt_Score2")->SetText(str);


		}
		else if (pCmd->Winner == 2) {
			str.Format(GetStringFromTable(20015), allptsDefender, pluscntDefender);
			GetDlgItem("Txt_Score1")->SetText(str);

			str.Format(GetStringFromTable(20016), allptsOffender, pluscntOffender);
			GetDlgItem("Txt_Score2")->SetText(str);
		}
		else {
			str.Format(L"%d", allptsDefender);
			GetDlgItem("Txt_Score1")->SetText(str);

			str.Format(L"%d", allptsOffender);
			GetDlgItem("Txt_Score2")->SetText(str);
		}
	}
	else {
		str.Format(L"%d", pCmd->DefenderSurvivors);
		GetDlgItem("Txt_Alive2")->SetText(str);

		str.Format(L"%d", pCmd->InvaderSurvivors);
		GetDlgItem("Txt_Alive1")->SetText(str);

		str.Format(L"%d", pCmd->DefenderDmg);
		GetDlgItem("Txt_Attack2")->SetText(str);

		str.Format(L"%d", pCmd->InvaderDmg);
		GetDlgItem("Txt_Attack1")->SetText(str);

		if (pCmd->Winner == 1) {
			str.Format(GetStringFromTable(20015), allptsOffender, pluscntOffender);
			GetDlgItem("Txt_Score1")->SetText(str);

			str.Format(GetStringFromTable(20016), allptsDefender, pluscntDefender);
			GetDlgItem("Txt_Score2")->SetText(str);
		}
		else if (pCmd->Winner == 2) {
			str.Format(GetStringFromTable(20016), allptsOffender, pluscntOffender);
			GetDlgItem("Txt_Score1")->SetText(str);

			str.Format(GetStringFromTable(20015), allptsDefender, pluscntDefender);
			GetDlgItem("Txt_Score2")->SetText(str);
		}
		else {
			str.Format(L"%d", allptsDefender);
			GetDlgItem("Txt_Score2")->SetText(str);

			str.Format(L"%d", allptsOffender);
			GetDlgItem("Txt_Score1")->SetText(str);
		}
	}

}