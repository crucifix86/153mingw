#include "DlgVipHelp1.h"
#include "AUIStillImageButton.h"
#include "EC_GameUIMan.h"

AUI_BEGIN_COMMAND_MAP(CDlgVipHelp1, CDlgBase)
AUI_ON_COMMAND("Btn_Next", OnCommand_Next)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

CDlgVipHelp1::CDlgVipHelp1()
{
}

CDlgVipHelp1::~CDlgVipHelp1()
{
}

bool CDlgVipHelp1::OnInitDialog()
{
	//
	return true;
}



void CDlgVipHelp1::OnCommand_Next(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_VipHelp2");
	Show(false);
	pDlg->Show(true);
}

void CDlgVipHelp1::OnCommand_CANCEL(const char* szCommand)
{
	Show(false);
}

