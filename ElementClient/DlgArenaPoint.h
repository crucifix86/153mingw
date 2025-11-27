// Filename	: DlgArenaPoint.h
// Creator	: Adriano Lopes
// Date		: 2022/06/06

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "hashmap.h"

class CDlgEnemyTeamMate;
class CECElsePlayer;

class CDlgArenaPoint : public CDlgBase
{

	AUI_DECLARE_COMMAND_MAP()


public:
	CDlgArenaPoint();
	virtual ~CDlgArenaPoint();

	virtual bool OnInitDialog();
	virtual void OnTick();

	PAUILABEL m_pTxt_red;
	PAUILABEL m_pTxt_blue;
	PAUILABEL m_pTxt_time;

	int m_iEndBattleTimestamp;
	int	m_nInitialTime;

	bool UpdateEnemyTeamInfo();

	void OnCommandMinimize(const char* szCommand);
	void OnCommandCANCAL(const char* szCommand);
	void HideEnemiesDlg();

	CDlgEnemyTeamMate* GetEnemyTeamMateDlg(int index);

	abase::hash_map<int, CECElsePlayer*> m_pEnemyList;

	abase::vector<CDlgEnemyTeamMate*> m_pEnemyMates;

	enum
	{
		TEAM_STATUS_OPEN = 0,
		TEAM_STATUS_CLOSE
	};

	enum
	{
		ARENA_PREPARE,	   //总战斗开始准备
		ARENA_PREPARE_1,   //总战斗开始准备1
		ARENA_ROUND_FIGHT, //单局战斗中
		ARENA_ROUND_REST,  //局间休息恢复
		ARENA_FINISH,	   //总战斗结束
	};

	inline int GetStageTime(int state)
	{
		switch (state)
		{
		case ARENA_PREPARE:
			return 75;
		case ARENA_PREPARE_1:
			return 10;
		case ARENA_ROUND_FIGHT:
			return 300;
		case ARENA_ROUND_REST:
			return 40;
		case ARENA_FINISH:
			return 45;
		}
		return 0;
	}
};