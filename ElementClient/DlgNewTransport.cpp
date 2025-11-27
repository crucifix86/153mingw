// Filename	: DlgNewTransport.cpp
// Creator	: Adriano Lopes
// Date		: 2021/10/16

#include "DlgNewTransport.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "EC_GPDataType.h"
#include "DlgWorldMap.h"

#define new A_DEBUG_NEW

extern CECGame* g_pGame;


AUI_BEGIN_COMMAND_MAP(CDlgNewTransport, CDlgBase)


AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgNewTransport, CDlgBase)
AUI_ON_EVENT("List_World", WM_LBUTTONUP, OnEventMapButtonUp)
AUI_END_EVENT_MAP()


CDlgNewTransport::CDlgNewTransport()
{
	m_pLstWorld = NULL;
	iSlotUsage = -1;
}

CDlgNewTransport::~CDlgNewTransport()
{
}

bool CDlgNewTransport::OnInitDialog() { 

	DDX_Control("List_World", m_pLstWorld);

	m_pLstWorld->ResetContent();

	m_pLstWorld->AddString(_AL("Perfect World"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 1);

	m_pLstWorld->AddString(_AL("Celestial Vale"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 161);

	m_pLstWorld->AddString(_AL("Morai"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 137);

	m_pLstWorld->AddString(_AL("Lothranis"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 121);

	m_pLstWorld->AddString(_AL("Momaganon"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 122);
	
	m_pLstWorld->AddString(_AL("Primal World"));
	m_pLstWorld->SetItemData(m_pLstWorld->GetCount() - 1, 163);

	m_pLstWorld->SetCurSel(-1);

	return true; 
}

void CDlgNewTransport::DoDataExchange(bool bSave) {
	CDlgBase::DoDataExchange(bSave);
}
void CDlgNewTransport::OnShowDialog(){
	m_pLstWorld->SetCurSel(-1);
}

void CDlgNewTransport::OnHideDialog(){}

void CDlgNewTransport::OnEventMapButtonUp(WPARAM wParam, LPARAM lParam, AUIObject* pObj) {
	PAUILISTBOX pList = (PAUILISTBOX)pObj;
	int nCurSel = pList->GetCurSel();
	int nMapSelected = pList->GetItemData(nCurSel);
	unsigned int realmLevel = g_pGame->GetGameRun()->GetHostPlayer()->GetRealmLevel();

	CECGameUIMan* pGameUI = GetGameUIMan();

	m_pLstWorld->SetCurSel(-1);

	const ROLEBASICPROP& rbp = g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps();

	//chek req
	if (nMapSelected == 137 && rbp.iLevel < 95 && rbp.iLevel2 < 8)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(30000), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	else if (nMapSelected == 121 && rbp.iLevel2 < 20)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(30001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	else if (nMapSelected == 122 && rbp.iLevel2 < 20)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(30001), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	else if (nMapSelected == 163 && realmLevel < 1)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(30002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	else if (pGameUI && !GetHostPlayer()->IsFighting() && nMapSelected != -1)
	{
		CDlgWorldMap* pMap = (CDlgWorldMap*)pGameUI->GetDialog("Win_WorldMapTravel");
		pMap->BuildTravelMap(999, (void*)iSlotUsage ,nMapSelected, iSlotUsage);
		pMap->Show(true);
	}
	


}