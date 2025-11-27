#pragma once

#include "DlgBase.h"

class CDlgVipHelp1 : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgVipHelp1();
	virtual ~CDlgVipHelp1();

	void CDlgVipHelp1::OnCommand_Next(const char* szCommand);
	void CDlgVipHelp1::OnCommand_CANCEL(const char* szCommand);

protected:
	virtual bool OnInitDialog();
};
