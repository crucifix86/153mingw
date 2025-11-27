// Filename	: DlgColorName.cpp
// Creator	: Deadrakys

#include "Color/GameColorName.h"
#include "Color/DlgColorName.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_SendC2SCmds.h"



#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgColorName, CDlgBase)

AUI_ON_COMMAND("color_name_select", OnCommandSelect)
AUI_ON_COMMAND("color_name_add", OnCommandAdd)
AUI_ON_COMMAND("color_name_delete", OnCommandDelete)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgColorName, CDlgBase)

AUI_ON_EVENT("List_ColorName", WM_LBUTTONUP, OnEventColorNameButtonUp)

AUI_END_EVENT_MAP()


CDlgColorName::CDlgColorName()
{
	chFile = _AL("./userdata/ColorName.ini");
	m_pLstColors = NULL;
}

CDlgColorName::~CDlgColorName()
{
}

bool CDlgColorName::OnInitDialog()
{
	LoadName();
	return true;
}

void CDlgColorName::OnCommandSelect(const char* szCommand)
{
	int nCurSel = m_pLstColors->GetCurSel();
	int nCurrentColorSelected = m_pLstColors->GetItemData(nCurSel);

	#pragma pack(push, 1)
	struct ColorNamePacket
	{
		short cmd;
		unsigned int color;
	};
	#pragma pack(pop)
	
	ColorNamePacket pBuf;
	pBuf.cmd = 744;
	pBuf.color = nCurrentColorSelected;


	//CECHostPlayer* pHost = GetHostPlayer();
	//pHost->SetColorName(nCurrentColorSelected);
	g_pGame->GetGameSession()->SendGameData( &pBuf, sizeof(pBuf));
	
}

void CDlgColorName::OnCommandAdd(const char* szCommand)
{
	AddName();
	SaveName();
	LoadName();
}

void CDlgColorName::OnCommandDelete(const char* szCommand)
{
	DelName();
	SaveName();
	LoadName();
}

void CDlgColorName::OnShowDialog()
{
	if( IsShow() )
	{
		m_pLstColors->SetCurSel(-1);
		GetDlgItem("Txt_Color_R")->SetText(_AL("0"));
		GetDlgItem("Txt_Color_G")->SetText(_AL("0"));
		GetDlgItem("Txt_Color_B")->SetText(_AL("0"));
		GetDlgItem("Txt_Color_A")->SetText(_AL("0"));
		GetDlgItem("Txt_Name")->SetText(_AL(""));
	}
}

void CDlgColorName::DoDataExchange(bool bSave) {
	CDlgBase::DoDataExchange(bSave);
}

void CDlgColorName::OnEventColorNameButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	int nCurrentColorSelected = pList->GetItemData(nCurSel);
	int r, g, b, a;

	GET_RGBA(nCurrentColorSelected, r, g, b, a);
	ACHAR szText[40];
	a_sprintf(szText, _AL("%d"), r); GetDlgItem("Txt_Color_R")->SetText(szText);
	a_sprintf(szText, _AL("%d"), g); GetDlgItem("Txt_Color_G")->SetText(szText);
	a_sprintf(szText, _AL("%d"), b); GetDlgItem("Txt_Color_B")->SetText(szText);
	a_sprintf(szText, _AL("%d"), a); GetDlgItem("Txt_Color_A")->SetText(szText);
	GetDlgItem("Txt_Name")->SetText(m_pLstColors->GetText(nCurSel));
	//m_pLstColors->SetCurSel(-1);
}

void CDlgColorName::OnTick()
{
	if (IsShow())
	{
		int r, g, b, a;
		size_t count = m_pLstColors->GetCount();

		for (size_t i = 0; i < count; i++)
		{
			int nCurrentColorSelected = m_pLstColors->GetItemData(i);
			int value = GameColorName::GetInstance()->GetColorName(nCurrentColorSelected);
			if (nCurrentColorSelected != value)
			{
				GET_RGBA(value, r, g, b, a);
				A3DCOLOR cl = A3DCOLORRGB(r, g, b);
				m_pLstColors->SetItemTextColor(i, cl);
			}
		}
	}
	CDlgBase::OnTick();
}

void CDlgColorName::LoadName()
{
	ACHAR chBuff[256];
	ACHAR szText[40];
	int r, g, b, a;

	memset(chBuff, 0x00, sizeof(chBuff));
	memset(szText, 0x00, sizeof(szText));
	DDX_Control("List_ColorName", m_pLstColors);
	m_pLstColors->ResetContent();

	size_t count = GetPrivateProfileInt(_AL("configs"), _AL("count"), 0, chFile);

	for (size_t i = 0; i < count; i++)
	{
		a_sprintf(szText, _AL("name_%d"), i);
		GetPrivateProfileString(szText, _AL("name"), _AL(""), chBuff, sizeof(chBuff), chFile);

		m_pLstColors->AddString(chBuff);

		r = GetPrivateProfileInt(szText, _AL("color_r"), 0, chFile);
		g = GetPrivateProfileInt(szText, _AL("color_g"), 0, chFile);
		b = GetPrivateProfileInt(szText, _AL("color_b"), 0, chFile);
		a = GetPrivateProfileInt(szText, _AL("color_a"), 0, chFile);
		if (r || g || b)
		{
			A3DCOLOR cl = A3DCOLORRGB(r, g, b);
			m_pLstColors->SetItemTextColor(m_pLstColors->GetCount() - 1, cl);
		}
		m_pLstColors->SetItemData(m_pLstColors->GetCount() - 1, SET_RGBA(r, g, b, a));
	}

	m_pLstColors->SetCurSel(-1);
}

void CDlgColorName::SaveName()
{
	ACHAR chBuff[256];
	ACHAR szText[40];
	size_t count = m_pLstColors->GetCount();
	int r, g, b, a;

	a_sprintf(chBuff, _AL("%d"), count);
	WritePrivateProfileString(_AL("configs"), _AL("count"), chBuff, chFile);

	for (size_t i = 0; i < count; i++)
	{
		a_sprintf(chBuff, _AL("name_%d"), i);
		WritePrivateProfileString(chBuff, _AL("name"), m_pLstColors->GetText(i), chFile);
		int nCurrentColorSelected = m_pLstColors->GetItemData(i);
		GET_RGBA(nCurrentColorSelected, r, g, b, a);

		a_sprintf(szText, _AL("%d"), r); WritePrivateProfileString(chBuff, _AL("color_r"), szText, chFile);
		a_sprintf(szText, _AL("%d"), g); WritePrivateProfileString(chBuff, _AL("color_g"), szText, chFile);
		a_sprintf(szText, _AL("%d"), b); WritePrivateProfileString(chBuff, _AL("color_b"), szText, chFile);
		a_sprintf(szText, _AL("%d"), a); WritePrivateProfileString(chBuff, _AL("color_a"), szText, chFile);
	}
}

void CDlgColorName::AddName()
{
	m_pLstColors->AddString(GetDlgItem("Txt_Name")->GetText());
	int r, g, b, a;

	r = _wtoi(GetDlgItem("Txt_Color_R")->GetText());
	g = _wtoi(GetDlgItem("Txt_Color_G")->GetText());
	b = _wtoi(GetDlgItem("Txt_Color_B")->GetText());
	a = _wtoi(GetDlgItem("Txt_Color_A")->GetText());
	m_pLstColors->SetItemData(m_pLstColors->GetCount() - 1, SET_RGBA(r, g, b, a));
}

void CDlgColorName::DelName()
{
	int nCurSel = m_pLstColors->GetCurSel();
	if (nCurSel != -1)
		m_pLstColors->DeleteString(nCurSel);
	m_pLstColors->SetCurSel(-1);
}

void CDlgColorName::SelectName()
{
	int nCurSel = m_pLstColors->GetCurSel();
	int nCurrentColorSelected = m_pLstColors->GetItemData(nCurSel);
	int r, g, b, a;

	GET_RGBA(nCurrentColorSelected, r, g, b, a);
	ACHAR szText[40];
	a_sprintf(szText, _AL("%d"), r); GetDlgItem("Txt_Color_R")->SetText(szText);
	a_sprintf(szText, _AL("%d"), g); GetDlgItem("Txt_Color_G")->SetText(szText);
	a_sprintf(szText, _AL("%d"), b); GetDlgItem("Txt_Color_B")->SetText(szText);
	a_sprintf(szText, _AL("%d"), a); GetDlgItem("Txt_Color_A")->SetText(szText);
	GetDlgItem("Txt_Name")->SetText(m_pLstColors->GetText(nCurSel));
}
