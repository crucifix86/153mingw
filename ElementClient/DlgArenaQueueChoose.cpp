// Filename	: DlgArenaQueueChoose.cpp
// Creator	: Adriano Lopes
// Date		: 2021/07/22

#include "DlgArenaQueueChoose.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

AUI_BEGIN_COMMAND_MAP(CDlgArenaQueueChoose, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgArenaQueueChoose, CDlgBase)


AUI_END_EVENT_MAP()

CDlgArenaQueueChoose::CDlgArenaQueueChoose()
{
}

CDlgArenaQueueChoose::~CDlgArenaQueueChoose()
{
}

bool CDlgArenaQueueChoose::OnInitDialog()
{
	return true;
}
void CDlgArenaQueueChoose::OnShowDialog()
{
}
void CDlgArenaQueueChoose::OnHideDialog()
{
}

void CDlgArenaQueueChoose::OnCommandConfirm(const char* szCommand)
{
	int nIndex;
	DDX_RadioButton(true, 0, nIndex);


	g_pGame->GetGameSession()->c2s_SendCmdArenaEnter(nIndex - 1);
	Show(false);

}