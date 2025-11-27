// Filename	: DlgArenaResult.h
// Creator	: Adriano Lopes
// Date		: 2022/06/13

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"

class CDlgArenaResult : public CDlgBase
{
	    AUI_DECLARE_COMMAND_MAP()
		AUI_DECLARE_EVENT_MAP()

public:
	CDlgArenaResult();
	virtual ~CDlgArenaResult();

	int exitcount;

	void SetResultInfo(void *pData);
	void SetCountdownExitArena(int time) { exitcount = time; };
	void ClearArenaBattleInfo();

	void ExitArena(const char* szCommand);
	void OnCommandCANCAL(const char* szCommand);

	PAUISTILLIMAGEBUTTON m_pBtn_Departure;
	AUIListBox* m_pLst1;
	AUIListBox* m_pLst2;


protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnHideDialog();
	virtual void OnTick();





};