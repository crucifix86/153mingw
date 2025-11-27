// Filename	: DlgColorName.h
// Creator	: Deadraky

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"

class CDlgColorName : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

	inline void GET_RGBA(int value, int& red, int& grin, int& blue, int& alpha)
	{
		blue = value & 0xff;
		grin = ((value & (0xff << 8)) >> 8);
		red = ((value & (0xff << 16)) >> 16);
		alpha = ((value & (0xff << 24)) >> 24);
	}

	inline unsigned int SET_RGBA(int red, int grin, int blue, int alpha)
	{
		return (_byteswap_ulong(RGB(red, grin, blue)) >> 8) | (alpha << 24);
	}

public:
	CDlgColorName();
	virtual ~CDlgColorName();

	virtual void OnTick();

	void OnCommandSelect(const char* szCommand);
	void OnCommandAdd(const char* szCommand);
	void OnCommandDelete(const char* szCommand);




	void OnEventColorNameButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj);

	void LoadName();
	void SaveName();
	void AddName();
	void DelName();
	void SelectName();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	void DoDataExchange(bool bSave);

	const wchar_t* chFile;
	PAUILISTBOX m_pLstColors;
};
