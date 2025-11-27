// Filename	: DlgArenaQueue.cpp
// Creator	: Adriano Lopes
// Date		: 2020/12/29

#include "DlgArenaQueue.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"


CDlgArenaQueue::CDlgArenaQueue()
{
	m_nTotalTime = 0;
	m_nEndTime = 0;

	m_pTxt_time = NULL;
	m_pGfx_1 = NULL;
	m_pBtn_car = NULL;
	m_pBtn_detail = NULL;
	m_pBtn_GuildBattle = NULL;
	m_pBtn_Divination = NULL;
}

CDlgArenaQueue::~CDlgArenaQueue()
{
}

bool CDlgArenaQueue::OnInitDialog()
{
	m_pTxt_time = (PAUILABEL)GetDlgItem("Txt_Time");
	m_pBtn_car = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Car");
	m_pBtn_detail = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Detail");
	m_pBtn_GuildBattle= (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GuildBattle");
	m_pBtn_Divination= (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Divination");
	m_pGfx_1 = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_1");
	return true;
}
void CDlgArenaQueue::OnShowDialog()
{
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	PAUIDIALOG pDlg = pGameUIMan->GetDialog("Win_Map");
	POINT pos = pDlg->GetPos();
 	SIZE size = pDlg->GetDefaultSize();
	SetPosEx(pos.x +30 , 95);
	m_pTxt_time->Show(false);
	m_pBtn_detail->Show(false);
	m_pBtn_car->Show(false);
	m_pBtn_GuildBattle->Show(false);
	m_pBtn_Divination->Show(false);
	m_pGfx_1->Show(false);
}

void CDlgArenaQueue::SetupMatchmakingQueueFlag()
{
	m_nEndTime = GetGame()->GetServerGMTTime() + 600;


	m_pTxt_time->Show(true);
	m_pBtn_detail->Show(true);
	m_pGfx_1->Show(true);

}
void CDlgArenaQueue::SetupArenaFlag()
{
	m_pBtn_GuildBattle->Show(true);
}

void CDlgArenaQueue::OnTick()
{
	if(m_pTxt_time->IsShow()){
		int nTime, nMin, nSec;
		ACString strTxt;
		nTime = m_nEndTime - GetGame()->GetServerGMTTime();
		if( nTime < 0 )
			nTime = 0;
		nTime = abs(nTime - 600);

		if (nTime == 600) {
			Show(false);
		}

		nMin = nTime / 60;
		nSec = nTime % 60;
		strTxt.Format(TEXT("%02d:%02d"),nMin,nSec);
		m_pTxt_time->SetText(strTxt);
	}
}

void CDlgArenaQueue::OnHideDialog()
{
}