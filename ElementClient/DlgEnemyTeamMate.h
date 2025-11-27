// Filename	: DlgEnemyTeamMate.h
// Creator	: Adriano Lopes
// Date		: 2022/06/06

#pragma once

#include "DlgBase.h"


class AUIImagePicture;
class AUILabel;
class AUIProgress;

#define CDLGTEAMMATE_ST_MAX			14

class CDlgEnemyTeamMate : public CDlgBase
{
	friend class CDlgArenaPoint;

	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()


public:
	CDlgEnemyTeamMate();
	virtual ~CDlgEnemyTeamMate();

	virtual void DoDataExchange(bool bSave);
	virtual void OnTick();

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void OnCommandCANCAL(const char* szCommand);

	AUIImagePicture* m_pImgProf;
	AUIImagePicture* m_pImgCombatMask;
	AUILabel* m_pTxtCharName;
	AUILabel* m_pTxtLV;
	AUIProgress* m_pPrgsHP;
	AUIProgress* m_pPrgsMP;
	AUIImagePicture* m_pImgSt[CDLGTEAMMATE_ST_MAX];
};