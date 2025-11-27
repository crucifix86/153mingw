/*
 * FILE: EC_BBSDlg.h
 *
 * DESCRIPTION: BBS Dialog - DISABLED STUB
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 * MODIFIED: Stub for Linux cross-compile
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef EC_BBSDLG_H
#define EC_BBSDLG_H

#include "AUI.h"
#include "EC_Global.h"

class CECBBSDlg
{
public:
	CECBBSDlg();
	~CECBBSDlg();
	bool Release();
	bool InitBBSDlg(PAUIMANAGER pAUIManager, AString initurl, int nWidth, int nHeight);
	bool SetSize(int nWidth, int nHeight);
	bool OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj);
	bool OnCommand(const char *pszCommand, PAUIDIALOG pDlg);
	bool Tick();
	bool Render();
	void Show(bool bShow = true);
	PAUIDIALOG GetBBSDlg() { return NULL; }
private:
	// BBS is disabled - minimal members
	PAUIMANAGER m_pAUIManager;
};

#endif
