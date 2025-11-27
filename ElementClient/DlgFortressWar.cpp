#include "DlgFortressWar.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "CSplit.h"
#include "ExpTypes.h"
#include "NetWork/factionfortresslist_re.hpp"
#include "Network/factionfortresschallenge_re.hpp"
#include "Network/rpcdata/gfactionfortressbriefinfo"
#include "Network/factionfortressget_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressWar, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Prev",	OnCommand_Prev)
AUI_ON_COMMAND("Btn_Next",	OnCommand_Next)
AUI_ON_COMMAND("Btn_Refresh",	OnCommand_Refresh)
AUI_ON_COMMAND("Btn_Search",	OnCommand_Search)
AUI_ON_COMMAND("Btn_DeclareWar",OnCommand_DeclareWar)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressWar, CDlgBase)
AUI_ON_EVENT("Lst_Fortress", WM_LBUTTONDOWN, OnEventLButtonDown_Lst_Fortress)
AUI_END_EVENT_MAP()

CDlgFortressWar::CDlgFortressWar()
{
	m_pEdit_Name = NULL;
	m_pLst_Fortress = NULL;
	m_pBtn_DeclareWar = NULL;
	m_nBegin = 0;
	m_status = ST_OPEN;
	m_bAllInfoReady = true;
}

bool CDlgFortressWar::OnInitDialog()
{
	DDX_Control("Edit_Name", m_pEdit_Name);
	DDX_Control("Lst_Fortress", m_pLst_Fortress);
	DDX_Control("Btn_DeclareWar", m_pBtn_DeclareWar);
	return true;
}

void CDlgFortressWar::OnShowDialog()
{
	m_nBegin = 0;
	Refresh();
}

void CDlgFortressWar::OnCommand_CANCEL(const char *szCommand)
{
	Show(false);
	
	//	�ر���ضԻ���
	PAUIDIALOG pDlgInfo = GetGameUIMan()->GetDialog("Win_FortressInfo");
	if (pDlgInfo && pDlgInfo->IsShow())
		pDlgInfo->OnCommand("IDCANCEL");

	//	�ر� NPC ����
	if (GetGameUIMan()->m_pCurNPCEssence != NULL)
		GetGameUIMan()->EndNPCService();
}

void CDlgFortressWar::Refresh()
{
	//	���ݵ�ǰָ���ĳ�ʼλ�ã����»����б�

	if (m_nBegin < 0)
		return;

	//	��������Э��
	GetGameSession()->factionFortress_List(m_nBegin);

	//	�������ȴ�����
	m_pLst_Fortress->ResetContent();
	m_bAllInfoReady = true;
	m_status = ST_OPEN;
	OnListSelChange();
}

void CDlgFortressWar::OnCommand_Prev(const char *szCommand)
{
	if (m_nBegin > 0)
	{
		m_nBegin -= FORTRESS_LIST_SIZE;
		a_ClampFloor(m_nBegin, 0);
	}
	//	else �Ѿ��ǵ�һҳ������ʾ�����������ĵ����İ��ɻ��أ������¸���
	Refresh();
}

void CDlgFortressWar::OnCommand_Next(const char *szCommand)
{
	int count = m_pLst_Fortress->GetCount();
	if (count == 0)
	{
		//	�б��е�ǰҳ��δ��ʾ����Ȼ���ܸ�����һҳ
		return;
	}
	if (count == FORTRESS_LIST_SIZE)
	{
		//	�����к���ҳ
		m_nBegin += FORTRESS_LIST_SIZE;
	}
	//	else	�Ѿ������һҳ������ʾ�����������ĵ����İ��ɻ��أ������¸���
	Refresh();
}

void CDlgFortressWar::OnCommand_DeclareWar(const char *szCommand)
{
	int idTarget = GetDeclareWarTarget();
	if (idTarget > 0)
	{
		//	����Ǯ��Ŀ
		int nMoneyNeeded = 5000000;
		CECGameUIMan *pGameUIMan = GetGameUIMan();

		if (nMoneyNeeded > GetHostPlayer()->GetMoneyAmount())
		{
			ACString strMsg;
			strMsg.Format(GetStringFromTable(9153), nMoneyNeeded);
			pGameUIMan->ShowErrorMsg(strMsg);
			return;
		}

		ACString strMsg;
		strMsg.Format(GetStringFromTable(9154), nMoneyNeeded);
		PAUIDIALOG pMsgBox(NULL);
		pGameUIMan->MessageBox("Fortress_DeclareWar", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(idTarget);
	}
}

void CDlgFortressWar::OnCommand_Refresh(const char *szCommand)
{
	Refresh();
}

void CDlgFortressWar::OnEventLButtonDown_Lst_Fortress(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnListSelChange();
}

void CDlgFortressWar::OnListSelChange()
{
	int idTarget = GetDeclareWarTarget();
	m_pBtn_DeclareWar->Enable(idTarget > 0);
}

void CDlgFortressWar::OnPrtcFactionFortressList_Re(const GNET::FactionFortressList_Re *p)
{
	m_nBegin = (int)p->begin;
	m_status = (FORTRESS_STATE)p->status;

	m_pLst_Fortress->ResetContent();

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2, strBuilding;

	m_bAllInfoReady = true;
	for (size_t i = 0; i < p->list.size(); ++ i)
	{
		const GNET::GFactionFortressBriefInfo &info = p->list[i];
		bool bName1OK = GetFactionName(info.factionid, strName1, true);
		bool bName2OK = GetFactionName(info.offense_faction, strName2, true);
		strBuilding = GetBuildingString(info.building.begin(), info.building.size());
		
		strText.Format(_AL("%s\t%d\t%d\t%s\t%s")
			, strName1
			, info.level
			, info.health
			, strBuilding
			, strName2);
		m_pLst_Fortress->AddString(strText);

		//	��¼����������ս
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
	}

	if (m_pLst_Fortress->GetCount())
	{
		//	Ĭ��ѡȡ��һ��
		m_pLst_Fortress->SetCurSel(0);
		OnListSelChange();
	}
}

void CDlgFortressWar::OnPrtcFactionFortressGet_Re(const GNET::FactionFortressGet_Re *p)
{
	//	�˴����� m_nBegin �� m_status ����

	if (p->retcode != ERR_SUCCESS)
	{
		GetGameSession()->OutputLinkSevError(p->retcode);
		return;
	}

	m_pLst_Fortress->ResetContent();

	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2, strBuilding;

	m_bAllInfoReady = true;
	while (true)
	{
		const GNET::GFactionFortressBriefInfo &info = p->brief;
		bool bName1OK = GetFactionName(info.factionid, strName1, true);
		bool bName2OK = GetFactionName(info.offense_faction, strName2, true);
		strBuilding = GetBuildingString(info.building.begin(), info.building.size());
		
		strText.Format(_AL("%s\t%d\t%d\t%s\t%s")
			, strName1
			, info.level
			, info.health
			, strBuilding
			, strName2);
		m_pLst_Fortress->AddString(strText);

		//	��¼����������ս
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.factionid, FORTRESS_LIST_OWNER_INDEX);
		m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, info.offense_faction, FORTRESS_LIST_OFFENSE_INDEX);
		if (!bName1OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME1_INDEX);
			m_bAllInfoReady = false;
		}
		if (!bName2OK)
		{
			//	��¼�������� Tick �и��°�������
			m_pLst_Fortress->SetItemData(m_pLst_Fortress->GetCount()-1, 1, FORTRESS_LIST_NAME2_INDEX);
			m_bAllInfoReady = false;
		}
		break;
	}

	if (m_pLst_Fortress->GetCount())
	{
		//	Ĭ��ѡȡ��һ��
		m_pLst_Fortress->SetCurSel(0);
		OnListSelChange();
	}
}

void CDlgFortressWar::OnPrtcFactionFortressChallenge_Re(const GNET::FactionFortressChallenge_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
	{
		GetGameSession()->OutputLinkSevError(p->retcode);
		return;
	}

	//	���½���
	GetGameUIMan()->MessageBox("", GetStringFromTable(9151), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgFortressWar::OnTick()
{
	int count = m_pLst_Fortress->GetCount();
	if (count <= 0)
		return;

	if (m_bAllInfoReady)
	{
		//	������Ϣ��������������Ҫ�ټ��
		return;
	}

	int idFaction = 0;
	const Faction_Info *pFInfo = NULL;	
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	ACString strText, strName1, strName2;

	m_bAllInfoReady = true;
	for (int i = 0; i < count; ++ i)
	{
		bool bName1Empty = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_NAME1_INDEX) != 0;
		bool bName2Empty = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_NAME2_INDEX) != 0;

		if (!bName1Empty && !bName2Empty)
		{
			//	������Ϣ�Ѿ�����
			continue;
		}

		int idFaction1 = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_OWNER_INDEX);
		if (!GetFactionName(idFaction1, strName1, false))
		{
			//	��������δ���ذ�����Ϣ
			m_bAllInfoReady = false;
		}
		else
		{
			//	�Ѿ�������Ϣ
			if (bName1Empty)
				m_pLst_Fortress->SetItemData(i, 0, FORTRESS_LIST_NAME1_INDEX);
		}

		int idFaction2 = m_pLst_Fortress->GetItemData(i, FORTRESS_LIST_OFFENSE_INDEX);
		if (!GetFactionName(idFaction2, strName2, false))
		{
			//	��������δ���ذ�����Ϣ
			m_bAllInfoReady = false;
		}
		else
		{
			//	�Ѿ�������Ϣ
			if (bName2Empty)
				m_pLst_Fortress->SetItemData(i, 0, FORTRESS_LIST_NAME2_INDEX);
		}

		CSplit s(m_pLst_Fortress->GetText(i));
		CSplit::VectorAWString vec = s.Split(_AL("\t"));
		strText.Format(_AL("%s\t%s\t%s\t%s\t%s"), strName1, vec[1], vec[2], vec[3], strName2);
		m_pLst_Fortress->SetText(i, strText);
	}
}

int	CDlgFortressWar::GetDeclareWarTarget()
{
	int idTarget(0);

	while (true)
	{
		int count = m_pLst_Fortress->GetCount();
		if (count <= 0)
			break;
		
		int nSel = m_pLst_Fortress->GetCurSel();
		if (nSel < 0 || nSel >=count)
			break;
		
		int idMyFaction = GetHostPlayer()->GetFactionID();
		if (idMyFaction <= 0)
			break;
		
		int idFaction = m_pLst_Fortress->GetItemData(nSel, FORTRESS_LIST_OWNER_INDEX);
		if (idFaction == idMyFaction)
		{
			//	���ܶ��Լ��Ļ�����ս
			break;
		}

		if (m_status != ST_CHALLENGE)
		{
			//	��ǰ���ǿ���ս״̬
			break;
		}

		idTarget = idFaction;
		break;
	}

	return idTarget;
}

void CDlgFortressWar::OnCommand_Search(const char *szCommand)
{
	//	���ݰ������Ʋ�ѯ id ��䵽�б��в�������ս
	//

	//	��ȡ����İ�������
	ACString strName = m_pEdit_Name->GetText();
	if (strName.IsEmpty())
	{
		//	δ��������
		ChangeFocus(m_pEdit_Name);
		return;
	}

	//	�ڱ��ػ����в�ѯ��Ӧ���� id
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	const Faction_Info *pInfo = pFMan->FindFactionByName(strName);
	if (!pInfo)
	{
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(9115));
		return;
	}
	
	//	����Э������˰�����Ϣ
	GetGameSession()->factionFortress_Get(pInfo->GetID());

	//	�������ȴ�����
	m_pLst_Fortress->ResetContent();
	m_bAllInfoReady = true;
	OnListSelChange();
}

bool CDlgFortressWar::GetFactionName(unsigned int uID, ACString &strName, bool bRequestFromServer)
{
	//	���� id ��ѯָ����������
	//	�޷��ڱ��ز�ѯʱ���� false
	//
	bool bRet(false);

	while (true)
	{
		if (!uID)
		{
			//	����0ʱ����ѯ�á��ޡ�����ͬû��ѯ��ʱ��-������
			strName = GetStringFromTable(9152);
			if (strName.IsEmpty())
				strName = _AL("none");
			bRet = true;
			break;
		}
		CECFactionMan *pFMan = GetGame()->GetFactionMan();
		const Faction_Info *pInfo = pFMan->GetFaction(uID, bRequestFromServer);
		if (pInfo)
		{
			strName = pInfo->GetName();
			bRet = true;
		}
		
		//	���մ���
		if (strName.IsEmpty())
			strName = _AL("-");
		break;
	}

	return bRet;
}

ACString CDlgFortressWar::GetBuildingString(const void *pBuf, size_t sz)
{
	//	�ӻ����л�ȡ�˻��ش�������ʩ����ȼ�
	ACString strText = GetStringFromTable(9152);

	while (true)
	{
		if (!pBuf)
			break;

#pragma pack(1)
		struct _data 
		{
			int id;
			int finish_time;
		};
#pragma pack()

		if (sz % sizeof(_data))
		{
			//	��ʽ����
			break;
		}

		int nBuilding = sz/sizeof(_data);
		const _data *pData = (const _data *)pBuf;
		CECHostPlayer *pHost = GetHostPlayer();

		int maxLevel = -1;
		const FACTION_BUILDING_ESSENCE *pEssence = NULL;
		for (int i = 0; i < nBuilding; ++ i)
		{
			pEssence = pHost->GetBuildingEssence(pData[i].id);
			if (!pEssence)
			{
				//	�������ʩid
				ASSERT(false);
				continue;
			}
			if (maxLevel < pEssence->level)
			{
				//	��¼��ʩ���Ƽ��ȼ�
				maxLevel = pEssence->level;
				strText = pEssence->name;
				if (strText.IsEmpty())
					strText = _AL("error");
			}
		}
		break;
	}

	return strText;
}