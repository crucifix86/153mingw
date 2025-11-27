// Filename	: DlgArenaMessage.cpp
// Creator	: Adriano Lopes
// Date		: 2020/12/29

#include "DlgArenaMessage.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "DlgArenaQueue.h"

AUI_BEGIN_COMMAND_MAP(CDlgArenaMessage, CDlgBase)

AUI_ON_COMMAND("blacklist", OnCommand_Enter)
AUI_ON_COMMAND("IDCANCEL", OnCommand_Exit)
AUI_ON_COMMAND("Btn_Close", OnCommand_Exit)

AUI_END_COMMAND_MAP()

CDlgArenaMessage::CDlgArenaMessage()
{
	m_pTxt_time = NULL;
	m_nTotalTime = 0;
	m_nEndTime = 0;
}

CDlgArenaMessage::~CDlgArenaMessage()
{
}

bool CDlgArenaMessage::OnInitDialog()
{
	m_pTxt_time = (PAUILABEL)GetDlgItem("Txt_Time");
	return true;
}
void CDlgArenaMessage::OnShowDialog()
{
	m_nEndTime = GetGame()->GetServerGMTTime()+75;
}
void CDlgArenaMessage::OnTick(){
	int nTime;
	ACString strTxt;
	nTime = m_nEndTime - GetGame()->GetServerGMTTime();
	if (nTime <= 0)
		Show(false);
	strTxt.Format(TEXT("%02d"),nTime);
	m_pTxt_time->SetText(strTxt);
}

void CDlgArenaMessage::OnCommand_Enter(const char* szCommand)
{
	g_pGame->GetGameSession()->c2s_SendCmdArenaAccept(true);

	Show(false);
}

void CDlgArenaMessage::OnCommand_Exit(const char* szCommand)
{
	g_pGame->GetGameSession()->c2s_SendCmdArenaAccept(false);

	Show(false);
}

void CDlgArenaMessage::OnHideDialog()
{
}