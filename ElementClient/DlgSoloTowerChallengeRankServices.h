#pragma once

#include "DlgBase.h"

class CDlgSoloTowerChallengeRankServices : public CDlgBase
{
public:
	CDlgSoloTowerChallengeRankServices();
	virtual ~CDlgSoloTowerChallengeRankServices();

protected:

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
