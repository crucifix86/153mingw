/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   18:00
	file name:	DlgRevive.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "AUIObject.h"
#include "AUIDialog.h"
#include "DlgRevive.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgRevive, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("back", OnCommandBack)
AUI_ON_COMMAND("accept", OnCommandAccept)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_ON_COMMAND("Btn_RMB", OnCommandCashResurrect)

AUI_END_COMMAND_MAP()

CDlgRevive::CDlgRevive(): m_bHasScroll(false)
{
}

CDlgRevive::~CDlgRevive()
{
}

bool CDlgRevive::OnInitDialog()
{
	m_pTxt_D = (PAUILABEL)GetDlgItem("Txt_D");
	m_pBtn_Back = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Back");
	m_pBtn_Decide = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Decide");
	m_pBtn_Accept = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Accept");
	m_pBtn_RMB = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_RMB");

	m_iTimesDiedBeforeCooltime = 0;

	return true;
}

void CDlgRevive::OnCommandConfirm(const char *szCommand)
{
	GetGameSession()->c2s_CmdReviveVillage();
}

void CDlgRevive::OnCommandBack(const char *szCommand)
{
	//	Revive with soul stone, check cool time at first
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost && !pHost->GetCoolTime(GP_CT_SOUL_STONE))
		GetGameSession()->c2s_CmdReviveItem();
}

void CDlgRevive::OnCommandAccept(const char *szCommand)
{
	GetGameSession()->c2s_CmdRevivalAgree();
}

void CDlgRevive::OnCommandCancel(const char *szCommand)
{
}

void CDlgRevive::OnCommandCashResurrect(const char* szCommand)
{
	// Check if died before resurrect cooltime expired
	CECHostPlayer* pHost = GetHostPlayer();
	int resCashCost = pHost->GetCashResurrectCashNeeded() / 100;
	// Get resurrection cost

	ACString tmpString;
	m_pAUIManager->MessageBox("CashResurrect", tmpString.Format(GetStringFromTable(293), resCashCost), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgRevive::DoCashResurrect() {
	GetGameSession()->c2s_CmdCashResurrect();
}



void CDlgRevive::OnShowDialog()
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost)
	{
		m_bHasScroll = pHost->GetPack()->GetItemTotalNum(3043) > 0 || pHost->GetPack()->GetItemTotalNum(32021) > 0;
		
		m_pBtn_Decide->Enable(true);
		
		if( pHost->GetBasicProps().iLevel < 10 )
			m_pTxt_D->SetText(GetStringFromTable(274));
		else
			m_pTxt_D->SetText(GetStringFromTable(a_Random(283, 292)));

		// Where to show Cash Resurrect option
		m_pBtn_RMB->Show(pHost->CanResurrectWithCash());
		m_pBtn_RMB->SetText(GetStringFromTable(294));
	}
}

void CDlgRevive::OnTick()
{
	CECHostPlayer* pHost = GetHostPlayer();
	if(pHost)
	{
		m_pBtn_Accept->Enable(pHost->GetReviveLostExp() >= 0.0f ? true : false);		
		m_pBtn_Back->Enable(m_bHasScroll && pHost->GetCoolTime(GP_CT_SOUL_STONE) == 0);

		// If player can resurrect using cash, handle button Text based on ability cooldown
		if (pHost->CanResurrectWithCash()) {
			m_pBtn_RMB->Enable(true);
			int cashReviveCooltime = pHost->GetCoolTime(GP_CT_RESURRECT_BY_CASH) / 1000;
			if (cashReviveCooltime > 0) {
				// Get resurrection time
				ACString tmpString;
				// Show text "Auto Revive (XXX seconds remaining)
				m_pBtn_RMB->SetText(tmpString.Format(GetStringFromTable(295), cashReviveCooltime));
			}
			else {
				// Show regular text "Auto revive using cash"
				m_pBtn_RMB->SetText(GetStringFromTable(294));
			}
		}
		else {
			// Hide button if cant resurrect wit cash
			m_pBtn_RMB->Enable(false);
		}
		
	}	
}