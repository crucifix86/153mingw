#include "DlgVipHelp2.h"
#include "AUIStillImageButton.h"
#include "EC_GameUIMan.h"

AUI_BEGIN_COMMAND_MAP(CDlgVipHelp2, CDlgBase)
AUI_ON_COMMAND("Btn_Next", OnCommand_Next)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

CDlgVipHelp2::CDlgVipHelp2()
{
}

CDlgVipHelp2::~CDlgVipHelp2()
{
}

bool CDlgVipHelp2::OnInitDialog()
{
	//
	return true;
}


void CDlgVipHelp2::OnCommand_Next(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_VipHelp1");
	Show(false);
	pDlg->Show(true);
}

void CDlgVipHelp2::OnCommand_CANCEL(const char* szCommand)
{
	Show(false);
}

