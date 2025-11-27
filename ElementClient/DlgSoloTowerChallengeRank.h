#pragma once

#include "DlgBase.h"

class CDlgSoloTowerChallengeRank : public CDlgBase
{
public:
	CDlgSoloTowerChallengeRank();
	virtual ~CDlgSoloTowerChallengeRank();

protected:

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
