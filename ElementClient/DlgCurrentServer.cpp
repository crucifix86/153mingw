// Filename	: DlgCurrentServer.cpp
// Creator	: XuWenbin
// Date		: 2013/9/7

#include "DlgCurrentServer.h"
#include "EC_ServerList.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"

#include <AUIDialog.h>

CDlgCurrentServer::CDlgCurrentServer(AUIDialog *pDialog)
: m_pParent(pDialog)
{
	m_pTxt_ServerName = m_pParent->GetDlgItem("Lbl_ServerName");
	m_pTxt_Ping = m_pParent->GetDlgItem("Lbl_Ping");
	if (m_pTxt_ServerName){
		m_pTxt_ServerName->SetText(_AL(""));
		m_pTxt_ServerName->SetData(-1);
	}
	if (m_pTxt_Ping){
		m_pTxt_Ping->SetText(_AL(""));
	}
}

void CDlgCurrentServer::OnTick()
{	
	//	���·������� Ping ��Ϣ
	int iServer = CECServerList::Instance().GetSelected();
	if (iServer >= 0){
		if (m_pTxt_ServerName != NULL){
			const CECServerList::ServerInfo &info = CECServerList::Instance().GetServer(iServer);
			if (iServer != (int)m_pTxt_ServerName->GetData()){
				//	���µ�ǰѡ�еķ���������
				m_pTxt_ServerName->SetData((DWORD)iServer);
				int iGroup = CECServerList::Instance().FindGroup(iServer, false);
				const CECServerList::GroupInfo &group = CECServerList::Instance().GetGroup(iGroup);
				ACString strText;
				// NOTE: Must cast ACString to (const ACHAR*) when passing to variadic Format()
			strText.Format(m_pParent->GetStringFromTable(280), (const ACHAR*)group.group_name, (const ACHAR*)info.server_name);
				m_pTxt_ServerName->SetText(strText);
				
				CECServerList::Instance().SetNeedPing(iServer, true);
				m_pTxt_Ping->SetText(_AL(""));
				m_pTxt_Ping->SetData(0);
			}
		}
		if (m_pTxt_Ping && m_pTxt_Ping->GetData() == 0){
			//	��������ʱ��
			DWORD dwStatus, dwCreateTime;
			unsigned char cExpRate;
			DWORD dwTime = g_pGame->GetGameSession()->GetPingTime(iServer, dwStatus, dwCreateTime, cExpRate);
			if (dwTime != 99999999){
				m_pTxt_Ping->SetData(1);
				CECServerList::Instance().SetNeedPing(iServer, false);
				
				ACString strText;
				strText.Format(m_pParent->GetStringFromTable(281), dwTime);
				m_pTxt_Ping->SetText(strText);
				
				A3DCOLOR clr;
				if( dwTime < 300 ) clr = 0xFF00FF7F;
				else if( dwTime < 1000 ) clr = 0xFFFFFF7F;
				else clr = 0xFFFF0000;
				m_pTxt_Ping->SetColor(clr);
			}else{
				CECServerList::Instance().OnTick();
			}
		}
	}
}
