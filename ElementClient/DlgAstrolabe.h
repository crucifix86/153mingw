#pragma once

#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "AUIEditBox.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrEquipMatter.h"
#include "DlgAstrolabeIncPointValue.h"
#include "DlgAstrolabeRandomAddon.h"
#include "AUISubDialog.h"
#include "EC_GPDataType.h"

#define ASTROLABE_APTIT_MAX 500.f
#define ASTROLABE_APTIT_SUM_MAX 2000

struct AstrolabeMsg {
	CECIvtrAstrolabe* pItem;
	int inventorySlot;
};

class CDlgAstrolabe : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:
	void OnConfirmStargaze(int id);
	void NeedsUpdate(bool bUpdate, S2C::cmd_astrolabe_operate_result* pCmd);

protected:
	bool m_bNeedsUpdate;
	S2C::cmd_astrolabe_operate_result* m_pCmd;

	CECIvtrAstrolabe* m_pCurrentItem;
	void UpdateAstrolabeState(CECIvtrAstrolabe* item);
	void ResetAstrolabe();
	void SetPointActive(int index, int progress, int points);
	void SetPointInactive(int index, int progress, int points);
	void EnableAttrTextSlot(int index, bool bBirthStar, int propertyType, int propertyValue);
	ACString MakeLevelHint();
	ACString MakeEnergyHint();
	void MakePointHint(int index, bool bBirthStar, int aptitude);
	void MakeAttrTextHint(int index, int iType);

	int GetExpForLevel(int level);
	int GetAstralEnergyForLevel(int level);

	std::vector<ASTROLABE_INC_INNER_POINT_VALUE_ESSENCE*> DetermineValidItemIds(int apt);

	AUISubDialog* m_pSubDlg;
	
	PAUILABEL m_pTitle;
	PAUIPROGRESS m_pLevelProgress;
	PAUIPROGRESS m_pEnergyProgress;
	PAUILABEL m_pLevelText;
	PAUILABEL m_pEnergyText;
	PAUILABEL m_pTotalAptitude;
	PAUIIMAGEPICTURE m_pAllGfx;
	PAUIIMAGEPICTURE m_pRndAttGfx;
	PAUIIMAGEPICTURE m_pAttrHighlight;

	PAUIIMAGEPICTURE m_pStarFullProgress[ASTROLABE_ADDON_MAX];
	PAUIIMAGEPICTURE m_pGfxPointValues[5];
	PAUIIMAGEPICTURE m_pGfxPointActive[ASTROLABE_ADDON_MAX];
	PAUIIMAGEPICTURE m_pImgPoint[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pTextAttrValue[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pTextAttrName[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pTxtPointValue[ASTROLABE_ADDON_MAX];
	PAUISTILLIMAGEBUTTON m_pButtonPoint[ASTROLABE_ADDON_MAX];
	PAUIIMAGEPICTURE m_pFace[6];
	PAUIIMAGEPICTURE m_pLines[15];
	unsigned short prevApt[5];
	PAUIIMAGEPICTURE m_pSwallowGfx;
	PAUIIMAGEPICTURE m_pShuffleGfx;


	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	
	bool m_bSwallowing;
	bool m_bSwallowChart;
	int m_iSwallowSlot;
	int m_iSwallowId;

public:

	virtual void OnTick();
	void OnCommandClose(const char* szCommand);
	void OnCommandNext(const char* szCommand);
	void OnCommandIncPointValue(const char* szCommand);
	void OnCommandButtonPress(const char* szCommand);
	void OnCommandSwallow(const char* szCommand);
	void OnCommandRandAtt(const char* szCommand);
	void OnCommandShuffleAtt(const char* szCommand);
	virtual void OnCommand_Help(const char* szCommand);


	bool IsSwallowing() { return m_bSwallowing; }
	void SetSwallowing(bool swallow);
	bool SwallowItem(int inventorySlot, CECIvtrItem* item);
	void SendSwallowCmd(int type);
	void ShowSwallowDialog(AstrolabeMsg* pItem);

	CDlgAstrolabe();
	virtual ~CDlgAstrolabe();
};

