#pragma once

#include "DlgBase.h"

class CDlgVipHelp2 : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgVipHelp2();
	virtual ~CDlgVipHelp2();

	void CDlgVipHelp2::OnCommand_Next(const char* szCommand);
	void CDlgVipHelp2::OnCommand_CANCEL(const char* szCommand);

protected:
	virtual bool OnInitDialog();
};
