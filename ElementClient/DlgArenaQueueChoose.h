// Filename	: DlgArenaQueueChoose.h
// Creator	: Adriano Lopes
// Date		: 2020/12/21

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "AUIRadioButton.h"
#include "AUIImagePicture.h"
class CDlgArenaQueueChoose : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgArenaQueueChoose();
	virtual ~CDlgArenaQueueChoose();

	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnCommandConfirm(const char* szCommand);
};