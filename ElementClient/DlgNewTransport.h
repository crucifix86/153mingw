// Filename	: DlgNewTransport.h
// Creator	: Adriano Lopes
// Date		: 2021/10/16

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "AUIListBox.h"

class CDlgNewTransport : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgNewTransport();
	virtual ~CDlgNewTransport();

protected:
	virtual bool OnInitDialog();
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	void OnEventMapButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	AUIListBox* m_pLstWorld;
	int iSlotUsage;


public:

	void SetSlotUsage(int slot) { iSlotUsage = slot; };

	
};