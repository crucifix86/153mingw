#pragma once
#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrEquipMatter.h"

class CDlgAstrolabeSwallow : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	void OnCommandChooseNew(const char* szCommand);
	void OnCommandChooseOld(const char* szCommand);

	CECIvtrAstrolabe* m_pCurrent;
	CECIvtrAstrolabe* m_pSwallow;
	int m_iSlot;

	PAUISTILLIMAGEBUTTON m_pOldButtonPoint[ASTROLABE_ADDON_MAX];
	PAUISTILLIMAGEBUTTON m_pNewButtonPoint[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pOldButtonText[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pNewButtonText[ASTROLABE_ADDON_MAX];

	PAUILABEL m_pOldTextAttrValue[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pOldTextAttrName[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pNewTextAttrValue[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pNewTextAttrName[ASTROLABE_ADDON_MAX];

	PAUILABEL m_pAttrDec[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pAttInc[ASTROLABE_ADDON_MAX];
	PAUILABEL m_pOldTotalAptitude;
	PAUILABEL m_pNewTotalAptitude;

	PAUILABEL m_pChooseNew;
	PAUISTILLIMAGEBUTTON m_pChooseNewBtn;

	void ResetDialog();
	void FillDialog();
	void SetPoint(int index, int pointsOld, int pointsNew, bool bActive);
	void EnableAttrTextSlot(int index, int slotIndex, bool bBirthStar, int propertyType, int propertyValue);

public:
	virtual void OnTick();

	CDlgAstrolabeSwallow();
	virtual ~CDlgAstrolabeSwallow();

	void SetContext(CECIvtrAstrolabe* pCurrent, CECIvtrAstrolabe* pSwallow, int iSlot);
	void SendSwallowCmd(CECIvtrItem* pItem);

	enum {
		NO_INHERIT,
		INHERIT
	};

};

