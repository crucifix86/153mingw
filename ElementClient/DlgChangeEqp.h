// Filename	: DlgChangeEqp.h
// Creator	: AdrianoLopes
// Date		: 2021/06/29

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIScroll.h"
#include "AUIImagePicture.h"

class CECIvtrEquip;
class CDlgChangeEqp : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgChangeEqp();
	virtual ~CDlgChangeEqp();

	void OnCommand_CANCEL(const char * szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual bool Tick(void);

	void Store(PAUIOBJECT pItem, CECIvtrEquip *pFashion);
	void Replace(PAUIOBJECT pSrcItem, PAUIOBJECT pDstItem);
	void Discard(PAUIOBJECT pItem);

	AString GetIconFile(int iSuite);

protected:
	virtual bool OnInitDialog();

private:
	bool IsEquipShortCutChangeable();
	int  GetEquipShortCutOffset(PAUIOBJECT pItem);
	int  GetEquipShortCutOffsetType(int offset);
	void SetEquipShortCut(PAUIOBJECT pItem, CECIvtrEquip *pEquip);
	bool IsEquipShortCutExchangeable(PAUIOBJECT pItem1, PAUIOBJECT pItem2);
	CECIvtrEquip * GetEquip(PAUIOBJECT pItem);
	bool IsEquipMatch(PAUIOBJECT pItem, CECIvtrEquip * pFashion);


private:
	PAUIOBJECT m_pEdit_Hour;
	PAUIOBJECT m_pEdit_Minute;
	PAUISTILLIMAGEBUTTON m_pButton_BeginChange;
	PAUISTILLIMAGEBUTTON m_pButton_StopChange;
	PAUISCROLL m_pScroll_Suite;

	enum {SuiteSizeWidth = 14};
	enum {SuiteSizeHeight = 3};
	enum {SuiteSizeShown = SuiteSizeWidth * SuiteSizeHeight};
	PAUIIMAGEPICTURE m_pImage_Suite[SuiteSizeHeight];
};
