/*
 * FILE: EC_BBSDlg.cpp
 *
 * DESCRIPTION: BBS Dialog - DISABLED STUB
 * The BBS feature is disabled in this build.
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 * MODIFIED: Stub implementation for Linux cross-compile
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_BBSDlg.h"

// BBS is disabled
const bool bUseBBS = false;

CECBBSDlg::CECBBSDlg()
{
}

CECBBSDlg::~CECBBSDlg()
{
}

bool CECBBSDlg::Release()
{
	return true;
}

bool CECBBSDlg::InitBBSDlg(PAUIMANAGER pAUIManager, AString initurl, int nWidth, int nHeight)
{
	return false;
}

bool CECBBSDlg::SetSize(int nWidth, int nHeight)
{
	return false;
}

bool CECBBSDlg::OnEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIDIALOG pDlg, PAUIOBJECT pObj)
{
	return false;
}

bool CECBBSDlg::OnCommand(const char *pszCommand, PAUIDIALOG pDlg)
{
	return false;
}

bool CECBBSDlg::Tick()
{
	return true;
}

bool CECBBSDlg::Render()
{
	return true;
}

void CECBBSDlg::Show(bool bShow)
{
}
