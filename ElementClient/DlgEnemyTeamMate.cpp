// Filename	: DlgEnemyTeamMate.cpp
// Creator	: Adriano Lopes
// Date		: 2022/06/06

#include "DlgEnemyTeamMate.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"
#include "AUIProgress.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEnemyTeamMate, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommandCANCAL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEnemyTeamMate, CDlgBase)

AUI_ON_EVENT(NULL, WM_LBUTTONUP, OnEventLButtonUp)
AUI_ON_EVENT("*", WM_LBUTTONUP, OnEventLButtonUp)
AUI_END_EVENT_MAP()


CDlgEnemyTeamMate::CDlgEnemyTeamMate()
{
	m_pImgProf = NULL;
	m_pTxtCharName = NULL;
	m_pTxtLV = NULL;
	m_pPrgsHP = NULL;
	m_pPrgsMP = NULL;
	m_pImgCombatMask = NULL;
	for (int i = 0; i < CDLGTEAMMATE_ST_MAX; i++)
		m_pImgSt[i] = NULL;
}


CDlgEnemyTeamMate::~CDlgEnemyTeamMate()
{

}

void CDlgEnemyTeamMate::OnCommandCANCAL(const char* szCommand)
{
}

void CDlgEnemyTeamMate::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Image_Prof", m_pImgProf);
	DDX_Control("Txt_CharName", m_pTxtCharName);
	DDX_Control("Txt_LV", m_pTxtLV);
	DDX_Control("Prgs_HP", m_pPrgsHP);
	DDX_Control("Prgs_MP", m_pPrgsMP);
	DDX_Control("CombatMask", m_pImgCombatMask);
	char szName[40];
	for (int i = 0; i < CDLGTEAMMATE_ST_MAX; i++)
	{
		sprintf(szName, "St_%d", i + 1);
		DDX_Control(szName, m_pImgSt[i]);
	}
}

void CDlgEnemyTeamMate::OnTick()
{

}

void CDlgEnemyTeamMate::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int idMate = GetData();
	GetHostPlayer()->SelectTarget(idMate);
}