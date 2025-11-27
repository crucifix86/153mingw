// Filename	: DlgArenaPoint.cpp
// Creator	: Adriano Lopes
// Date		: 2022/06/06
#include "AFI.h"
#include "A2DSprite.h"
#include "DlgArenaPoint.h"
#include "DlgEnemyTeamMate.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_Global.h"
#include "EC_Configs.h"
#include "EC_Utility.h"
#include "EC_ElsePlayer.h"
#include "DlgHost.h"
#include "AUILabel.h"
#include "AUICommon.h"
#include "AUIProgress.h"
#include "AUIImagePicture.h"
#include "DlgArenaCountTime.h"

#define CDLGENEMY_MAXTEAMMATES		6

AUI_BEGIN_COMMAND_MAP(CDlgArenaPoint, CDlgBase)
AUI_ON_COMMAND("minimize", OnCommandMinimize)
AUI_ON_COMMAND("IDCANCEL", OnCommandCANCAL)
AUI_END_COMMAND_MAP()

CDlgArenaPoint::CDlgArenaPoint()
{
	m_iEndBattleTimestamp = 0;
	m_nInitialTime = 0;
	m_pTxt_red = NULL;
	m_pTxt_blue = NULL;
	m_pTxt_time = NULL;
}

CDlgArenaPoint::~CDlgArenaPoint()
{
}

void CDlgArenaPoint::OnCommandCANCAL(const char* szCommand)
{
}

void CDlgArenaPoint::OnTick() {

	bool negative = true;
	int nActualTimer, nElapsedTimer, nMin, nSec;
	ACString strTxt;
	nActualTimer = m_iEndBattleTimestamp - GetGame()->GetServerGMTTime(); // actual timer of event remain
	nElapsedTimer = nActualTimer - 815 ; // actual timer of event

	if (nElapsedTimer > 0)
		negative = false;

	//if (nTime < 0)
	//	nTime = 0;
	nElapsedTimer = abs(nElapsedTimer);
	nMin = nElapsedTimer / 60;
	nSec = nElapsedTimer % 60;
	strTxt.Format(TEXT("%02d:%02d"), nMin, nSec);
	m_pTxt_time->SetText(strTxt);
	if(!negative)
		m_pTxt_time->SetColor(A3DCOLORRGB(0, 255, 0));
	else
		m_pTxt_time->SetColor(A3DCOLORRGB(252, 234, 204));

	CDlgArenaCountTime* pDlg = dynamic_cast<CDlgArenaCountTime*>(m_pAUIManager->GetDialog("Win_ArenaCountTime"));
	if (!negative && nElapsedTimer == 3) {
		pDlg->Show(true);
	}
	else if (negative && nElapsedTimer == 1) {
		pDlg->Show(false);
	}
	

	UpdateEnemyTeamInfo();
}

bool CDlgArenaPoint::UpdateEnemyTeamInfo()
{
	CECWorld* pWorld = GetWorld();
	CECHostPlayer* pHost = GetHostPlayer();


	CECTeam* pTeam = pHost->GetTeam();

	int nNumMembers = m_pEnemyList.size();

	PAUIDIALOG pDlgLast = this;
	//if (nNumMembers > 0)
	//{
	//	if (!IsShow())
	//	{
	//		Show(true, false, false);
	//	}
	//	pDlgLast = this;
	//}
	//else if (IsShow())
	//	Show(false);

	ACHAR szText[256];
	AString strFile;
	CDlgEnemyTeamMate* pDlgMate;
	PAUIPROGRESS pProgress;
	CECElsePlayer* pMember;
	
	PAUIIMAGEPICTURE pImage;
	int i, idMateDlg = 1;
	int nStatus = (int)GetData();
	CECPlayerMan* pPlayerMan = pWorld->GetPlayerMan();

	abase::hash_map<int, CECElsePlayer*>::iterator it = m_pEnemyList.begin();

	while (it != m_pEnemyList.end()) {
		pMember = GetWorld()->GetPlayerMan()->GetElsePlayer(it->first);

		pDlgMate = GetEnemyTeamMateDlg(idMateDlg - 1);
		if (nStatus == TEAM_STATUS_CLOSE)
		{
			if (pDlgMate->IsShow())
				pDlgMate->Show(false);
		}
		else
		{
			pImage = pDlgMate->m_pImgProf;
			if (pMember) {
				ACString strPro = GetGameRun()->GetProfName(pMember->GetProfession());
				pImage->SetHint(strPro);
				pImage->FixFrame(pMember->GetProfession() + pMember->GetGender() * NUM_PROFESSION);
			}
			

			ACString strName = GetGameRun()->GetPlayerName(it->first, true);
			//AUI_ConvertChatString(pMember->GetName(), szText);
			pDlgMate->m_pTxtCharName->SetText(strName);
			if (pMember) {
				a_sprintf(szText, _AL("%3d"), pMember->GetBasicProps().iLevel);
				pDlgMate->m_pTxtLV->SetText(szText);

				a_sprintf(szText, _AL("HP: %d/%d\rMP: %d/%d"),
					pMember->GetBasicProps().iCurHP, max(pMember->GetExtendProps().bs.max_hp, 1),
					pMember->GetBasicProps().iCurMP, max(pMember->GetExtendProps().bs.max_mp, 1));

				pProgress = pDlgMate->m_pPrgsHP;
				pProgress->SetProgress(pMember->GetBasicProps().iCurHP
					* AUIPROGRESS_MAX / max(pMember->GetExtendProps().bs.max_hp, 1));
				pProgress->SetHint(szText);
				/*m_pTxt_hpteam = (PAUILABEL)pDlgMate->GetDlgItem("TXT_HPTEAM");
				ACString str;
				str.Format(L"%d/%d", pMember->GetCurHP(), pMember->GetMaxHP());
				m_pTxt_hpteam->SetText(str);*/

				pProgress = pDlgMate->m_pPrgsMP;
				pProgress->SetProgress(pMember->GetBasicProps().iCurMP
					* AUIPROGRESS_MAX / max(pMember->GetExtendProps().bs.max_mp, 1));
				pProgress->SetHint(szText);
			}
			if (!pDlgMate->IsShow())
				pDlgMate->Show(true, false, false);

			pDlgMate->AlignTo(pDlgLast,
				AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_RIGHT,
				AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM);
			pDlgMate->SetCanMove(false);
			pDlgMate->SetData(it->first);

			pImage = pDlgMate->m_pImgCombatMask;
			if(pMember){
				if (pMember->IsFighting())
				{
					pImage->Show(true);
					pImage->FadeInOut(1500);
				}
				else
				{
					pImage->Show(false);
					pImage->FadeInOut(0);
				}
			}

			
			if (pMember) {
				CECIconStateMgr* pMgr = GetGameUIMan()->GetIconStateMgr();
				IconLayoutVertical layout(2);
				pMgr->RefreshStateIcon(pDlgMate->m_pImgSt, CDLGTEAMMATE_ST_MAX,
					true ? &pMember->GetIconStates() : NULL,
					layout, true);
			}

			pDlgLast = pDlgMate;
		}

		++it;
		idMateDlg++;
	}

	for (i = idMateDlg; i < CDLGENEMY_MAXTEAMMATES; i++)
	{
		pDlgMate = GetEnemyTeamMateDlg(idMateDlg - 1);
		if (pDlgMate->IsShow())
			pDlgMate->Show(false);

		idMateDlg++;
	}

	return true;
}

bool CDlgArenaPoint::OnInitDialog()
{

	if (!CDlgBase::OnInitDialog())
		return false;

	m_pTxt_red = (PAUILABEL)GetDlgItem("Txt_Red");
	m_pTxt_blue = (PAUILABEL)GetDlgItem("Txt_Blue");
	m_pTxt_time = (PAUILABEL)GetDlgItem("Txt_Time");


	m_pEnemyMates.clear();
	m_pEnemyList.clear();
	while (true)
	{
		AString strDlgName;
		strDlgName.Format("Win_EnemyTeamMate%d", m_pEnemyMates.size() + 1);
		CDlgEnemyTeamMate * pDlg = dynamic_cast<CDlgEnemyTeamMate*>(m_pAUIManager->GetDialog(strDlgName));
		if (!pDlg) break;

		m_pEnemyMates.push_back(pDlg);
	}

	return true;
}

void CDlgArenaPoint::OnCommandMinimize(const char* szCommand)
{
	int nStatus = (int)GetData();
	if (nStatus == TEAM_STATUS_OPEN)
		SetData(TEAM_STATUS_CLOSE);
	else
		SetData(TEAM_STATUS_OPEN);
}

CDlgEnemyTeamMate* CDlgArenaPoint::GetEnemyTeamMateDlg(int index)
{
	// starts from 0
	if (index >= 0 && index < (int)m_pEnemyMates.size())
	{
		return m_pEnemyMates[index];
	}

	return NULL;
}

void CDlgArenaPoint::HideEnemiesDlg() {
	for (int i = 0; i < CDLGENEMY_MAXTEAMMATES; i++) {
		CDlgEnemyTeamMate* pDlg = GetEnemyTeamMateDlg(i);
		pDlg->Show(false);

	}
}