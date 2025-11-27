#pragma once
#include "DlgBase.h"
#include <vector>
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIProgress.h"
#include "EC_IvtrEquip.h"

struct EQUIP_MAKE_HOLE_CONFIG;

class CDlgEquipAccessorySlot : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	EQUIP_MAKE_HOLE_CONFIG* m_pConfig;

	AUIImagePicture* m_pImg_Equip;
	AUIImagePicture* m_pImg_Material;
	PAUIOBJECT		m_pTxt_Name;

	PAUIOBJECT	m_pSuccessRatio;
	PAUIOBJECT	m_pCost;
	PAUIOBJECT	m_pMaterialNum;
	PAUIOBJECT	m_pNextSlot;
	PAUIOBJECT	m_pCurrentSlot;
	PAUIOBJECT	m_pMaxMaterial; // required stones
	
	int m_iMaterialNum;
	int m_iMaxNumber;
	int m_iCost;
	int m_iSocketMaterial;
	int m_iLevel;

public:
	virtual void OnTick();
	void OnCommandCANCEL(const char* szCommand);
	void OnCommandConfirm(const char* szCommand);
	void OnCommandSubtractItem(const char* szCommand);
	void OnCommandAddItem(const char* szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	void SetEquip(int iSrc);
	void ClearDialog();
	void UpdateUI();
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	CDlgEquipAccessorySlot();
	virtual ~CDlgEquipAccessorySlot();

};

