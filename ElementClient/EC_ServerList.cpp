#include "EC_ServerList.h"
#include "EC_CrossServerList.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "BolaDebug.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GetHostByName.h"
#include "EC_CrossServer.h"
#include "EC_Reconnect.h"
#include "EC_Split.h"
#include "EC_TimeSafeChecker.h"

#include <AUICommon.h>
#include <achar.h>
#include <AIniFile.h>

#include <ctime>
#include <io.h>

CECServerList & CECServerList::Instance()
{
	static CECServerList dummy;
	return dummy;
}

CECServerList::CECServerList()
{
	m_pingInfo = NULL;
	m_iSelected = -1;
}

void CECServerList::Clear()
{
	delete [] m_pingInfo;
	m_pingInfo = NULL;
	
	m_serverVec.clear();
	m_iSelected = -1;
	m_groupVec.clear();
	
	m_strServerList.Empty();
}

void CECServerList::Init(const ACHAR *szHotGroupName)
{	
	//	���ԭ������
	Clear();
	CECCrossServerList::GetSingleton().Clear();

	// ��˳����ҵ�һ�����ڵ� serverlist.txt ·��
	AString strServerLists[] = 
	{
			"userdata\\server\\serverlist.txt"
	};
	for (int j = 0; j < ARRAY_SIZE(strServerLists); ++ j)
	{
		if (access(strServerLists[j], 0) == 0)
		{
			m_strServerList = strServerLists[j];
			break;
		}
	}
	
	FILE *fStream;
	int i = 0;
	int group_id = -1;
	
	srand(time(NULL));
	
	BOLA_INFO("ServerList::Init - path=%s", (const char*)m_strServerList);
	if(!m_strServerList.IsEmpty() && (fStream = fopen(m_strServerList, "rb")) != NULL )
	{
		BOLA_INFO("ServerList::Init - file opened successfully");
		const ACHAR * szHotFlag = _AL("&1");           // ���·����
		const ACHAR * szCrossServerFlag = _AL("&2");   // �����������
		ACHAR szLine[256];
		wint_t bom = a_fgetc(fStream);
		BOLA_INFO("ServerList::Init - BOM char: 0x%04X", (unsigned int)bom);
		GroupInfo hotGroup;
		hotGroup.bHot = true;
		hotGroup.group_name = szHotGroupName;
		do{
			if (!a_fgets(szLine, ARRAY_SIZE(szLine), fStream))
				break;
			// Log what we read (convert to narrow for logging)
			char narrowLine[256];
			for(int k=0; k<255 && szLine[k]; k++) {
				narrowLine[k] = (szLine[k] < 128) ? (char)szLine[k] : '?';
				narrowLine[k+1] = 0;
			}
			BOLA_INFO("ServerList::Init - line read: [%s] len=%d", narrowLine, (int)a_strlen(szLine));
			if( a_strlen(szLine) > 0){
				int n = a_strlen(szLine) - 1;
				while( n >= 0 && (szLine[n] == '\r' || szLine[n] == '\n') )
					n--;
				szLine[n + 1] = '\0';
				bool bCrossServer = false;
				CECSplitHelperW s(szLine, _AL("\t"));
				if (s.Count() >= 2){
					CECSplitHelperW address(s.ToString(1), _AL(":"));
					if (address.Count() < 2) continue;
					ServerInfo info;
					info.group_id = group_id;

					//	����	
					info.server_name = s.ToString(0);

					//	29000-29005:10.68.16.108:1
					CECSplitHelperW ports(address.ToString(0), _AL("-"));
					if (ports.Count() == 0){
						info.port_min = info.port_max = 0;
					}else{
						int pmin = ports.ToInt(0);
						int pmax = pmin;							
						if (ports.Count() > 1){
							pmax = ports.ToInt(1);
						}							
						if (pmin > pmax){
							std::swap(pmin, pmax);
						}							
						info.port_min = pmin;
						info.port_max = pmax;
					}
					info.RandPort();
					info.address = AC2AS(address.ToString(1));
					info.line = address.Count() > 2 ? address.ToInt(2) : 0;

					//	&1 &2
					for (int u = 2; u < s.Count(); ++ u)
					{
						const AWString &cur = s.ToString(u);
						if (cur == szHotFlag)
							info.flag = 1;
						else if (cur == szCrossServerFlag)
							bCrossServer = true;
					}

					//	zoneid
					info.zone_id = (s.Count() > 2) ? s.ToInt(2) : 0;
					
					//	���ӿ��������
					if (bCrossServer){
						CECCrossServerList::ServerInfo crossInfo;
						crossInfo.port		= info.port;
						crossInfo.port_max	= info.port_max;
						crossInfo.port_min	= info.port_min;
						crossInfo.address	= info.address;
						crossInfo.zoneid	= info.zone_id;
						crossInfo.server_name= info.server_name;
						crossInfo.line		= info.line;
						CECCrossServerList::GetSingleton().PutOneServer(crossInfo);
						continue;
					}

					//	���ӵ���ͨ�������б�
					m_serverVec.push_back(info);
					BOLA_INFO("ServerList::Init - Added server: port=%d addr=%s zone=%d", info.port, (const char*)info.address, info.zone_id);					
					if(group_id>=0)
						m_groupVec[group_id].server_vec.push_back(m_serverVec.size()-1);					
					if(info.flag==1)
						hotGroup.server_vec.push_back(m_serverVec.size()-1);
				}
				else if( a_strlen(szLine) > 0 ){
					//	���ߵ���
					GroupInfo info;
					info.group_name = szLine;
					m_groupVec.push_back(info);
					group_id++;
				}
			}
		} while(true);
		
		// ����л��·�
		if (!hotGroup.server_vec.empty())
		{
			//	��Ϊ��һ�� group������ʾ�ڷ������б���һ��
			for (int i=0;i<(int)m_serverVec.size();i++)
			{
				m_serverVec[i].group_id += 1;
			}			
			m_groupVec.insert(m_groupVec.begin(),hotGroup);
		}
		
		fclose(fStream);
	}
	else
		AUI_ReportError(__LINE__, 1,"CECServerList::Init(), server list file error!");
	
	// ping info
	ResetPing();
	
}

void CECServerList::Export()
{
	if (m_strServerList.IsEmpty()) return;

	//	���� serverlist.txt ����Ŀ¼���������ļ�������ͬĿ¼��
	int pos = m_strServerList.ReverseFind('\\');
	if (pos < 0) pos = m_strServerList.ReverseFind('/');
	AString strServerListPath;
	if (pos >= 0) strServerListPath = m_strServerList.Left(pos+1);

	//	�� GT �ļ���ʼ����
	AString strSaveFile = strServerListPath + "serverlist.sev";
	FILE * fp = fopen(strSaveFile, "wb");
	if (!fp)
	{
		a_LogOutput(1, "CECServerList::Export,Failed to open for save %s", strSaveFile);
		return;
	}

	typedef std::set<int>		ZoneIDSet;
	ZoneIDSet zoneids;

	typedef std::set<ACString>	NameSet;
	NameSet names;

	for (size_t u(0); u < m_serverVec.size(); ++ u)
	{
		const ServerInfo & server = m_serverVec[u];

		//	��֤ zoneid �Ϸ�
		if (server.zone_id <= 0 ||
			zoneids.find(server.zone_id) != zoneids.end())
		{
			a_LogOutput(1, "CECServerList::Export,%s ignored for invalid/duplicated zoneid(%d)", AC2AS(server.server_name), server.zone_id);
			continue;
		}

		//	���� TrimLeft ��������������и���ո��ַ�������Ӱ��������ơ�����Ӱ���������
		ACString strExportName;
		ACString strName = server.server_name;
		while (!strName.IsEmpty())
		{
			strName.TrimLeft();
			if (!strName.IsEmpty())
			{
				strExportName += strName.Left(1);
				strName.CutLeft(1);
			}
		}

		//	��֤���ƺϷ�
		if (strExportName.IsEmpty() ||
			names.find(strExportName) != names.end())
		{
			a_LogOutput(1, "CECServerList::Export,%s ignored for empty/duplicated name %s(after processed)", AC2AS(server.server_name), AC2AS(strExportName));
			continue;
		}

		//	д���ļ�
		fwrite(&server.zone_id, sizeof(server.zone_id), 1, fp);

		int len = strExportName.GetLength();
		fwrite(&len, 1, sizeof(len), fp);
		fwrite((const ACHAR *)strExportName, len, sizeof(ACHAR), fp);

		//	���ӽ�������
		zoneids.insert(server.zone_id);
		names.insert(strExportName);
	}

	fflush(fp);
	fclose(fp);

	//	���ļ����������־
	fp = fopen(strSaveFile, "rb");
	if (!fp)
	{
		a_LogOutput(1, "CECServerList::Export,Failed to read %s", strSaveFile);
		return;
	}
	int zoneid(0), namelen(0);
	ACString strName;
	while (fread(&zoneid, sizeof(zoneid), 1, fp) == 1)
	{
		if (fread(&namelen, sizeof(namelen), 1, fp) != 1)
		{
			a_LogOutput(1, "CECServerList::Export,Read name len error");
			break;
		}
		if (namelen <= 0)
		{
			a_LogOutput(1, "CECServerList::Export,name len invalid(%d)", namelen);
			break;
		}
		wchar_t *pBuf = strName.GetBuffer(namelen+1);
		if (fread(pBuf, sizeof(wchar_t), namelen, fp) != namelen)
		{
			a_LogOutput(1, "CECServerList::Export,Read name error");
			break;
		}
		pBuf[namelen] = _AL('\0');
		a_LogOutput(1, "CECServerList::Export,zoneid=%d, name=%s", zoneid, AC2AS(strName));
	}
	fclose(fp);
}

int CECServerList::FindGroup(int iServer, bool bHot)
{
	if (iServer >= 0){
		if (bHot){
			int nHotGroup = 0;
			const GroupInfo &g = GetGroup(nHotGroup);
			if (g.bHot && std::find(g.server_vec.begin(), g.server_vec.end(), iServer) != g.server_vec.end()){
				return nHotGroup;
			}
		}		
		const ServerInfo &info = GetServer(iServer);
		return info.group_id;
	}
	return -1;
}

int CECServerList::FindServerByName(const ACString& name, int defaultLine)
{
	//	������ FindServerByAddress�������Ʋ���ʱ��ʹ�� port �Ƚ���������
	int iServer(-1);
	for (int i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = m_serverVec[i];
		if(info.server_name == name)
		{
			iServer = i;
			if (info.line == defaultLine){
				//	���ҵ�ָͬ�� line ��ͬ���򷵻���ͬ�ģ������ҵ��򷵻����һ����ַ�Ͷ˿�ƥ���
				break;
			}
		}
	}
	return iServer;
}
int CECServerList::FindServerByAddress(const AString& address, int defaultLine, int port)
{
	int iServer(-1);
	for (int i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = m_serverVec[i];
		if(info.address == address && info.HasPort(port))
		{
			iServer = i;
			if (info.line == defaultLine){
				//	���ҵ�ָͬ�� line ��ͬ���򷵻���ͬ�ģ������ҵ��򷵻����һ����ַ�Ͷ˿�ƥ���
				break;
			}
		}
	}
	return iServer;
}

void CECServerList::ResetPing()
{
	if (!m_pingInfo){
		m_pingInfo = new SERVERPINGINFO[GetServerCount()];
	}
	for(int i = 0; i < GetServerCount(); i++ )
	{
		m_pingInfo[i].port = 0;
		ServerInfo & info = GetServer(i);
		info.addr_valid = false;
		info.need_ping = false;
		info.ping_done = false;
	}
	g_pGame->GetGameSession()->resetPing();
	CECGetHostByName::Instance().Clear();
}

void CECServerList::SetNeedPing(int iServer, bool bNeed)
{
	if (iServer >= 0 && iServer < GetServerCount()){
		ServerInfo &info = GetServer(iServer);
		info.need_ping = bNeed;
		if (bNeed){
			CECGetHostByName::Instance().Add(info.address);
		}
	}
}

void CECServerList::PingServer()
{
	int i(0);
	for(i = 0; i < GetServerCount(); i++)
	{
		const ServerInfo &info = GetServer(i);
		SERVERPINGINFO &pingInfo = m_pingInfo[i];
		if( info.need_ping && pingInfo.port > 0 && pingInfo.num < 4 &&
			CECTimeSafeChecker::ElapsedTimeFor(pingInfo.lasttime) > pingInfo.num * 10000 )
		{
			pingInfo.lasttime = GetTickCount();
			pingInfo.num++;
			g_pGame->GetGameSession()->server_Ping(i, &pingInfo.addr, pingInfo.port);
		}
	}
}
void CECServerList::OnTick()
{
	//	������Ҫ���� Ping ��ȡ�����������Ϣ
	//	���ڻὨ�������ӣ��п�������������ͬʱ���в����������ӽ��и��ţ����뿪������������
	//	���ֻ�ڱ�Ҫ��ʱ����У�

	int i;
	
	//	��� gethostbyname �Ƿ񷵻�
	for(i=0; i<GetServerCount(); ++i)
	{
		CECServerList::ServerInfo &entry = GetServer(i);
		if (!entry.addr_valid && CECGetHostByName::Instance().Get(entry.address, entry.addr))
			entry.addr_valid = true;
	}
	
	// ��ip��ַ�ķ�����������һ�� ping ����
	for(i=0; i<GetServerCount();)
	{
		CECServerList::ServerInfo& server = GetServer(i);		
		SERVERPINGINFO &pingInfo = m_pingInfo[i];
		if (server.addr_valid && server.need_ping && !server.ping_done)
		{
			pingInfo.addr = server.addr;
			pingInfo.port = server.port;
			pingInfo.lasttime = GetTickCount();
			pingInfo.num = 1;
			g_pGame->GetGameSession()->server_Ping(i, &server.addr, pingInfo.port);
			server.ping_done = true;
			break;
		}
		
		i ++;
	}

	//	��Ҫ���ڽ��� ping �����ķ����� (port ��Ϊ0) ����ping
	PingServer();
}

bool CECServerList::ApplyServerSetting()
{
	if (!CanApply()){
		return false;
	}
	int iServer = GetSelected();
	ServerInfo &info = GetServer(iServer);
	
	//	��ǰ�������ж�˿�ʱÿ�ε�¼������˿�
	info.RandPort();
	
	//	���õ�ǰ����
	CECGame::GAMEINIT &gi = g_pGame->GetGameInit();
	gi.iPort = info.port;
	strcpy(gi.szIP, info.address);
	strcpy(gi.szServerName, AC2AS(info.server_name));
	gi.iServerID =  info.zone_id;
	gi.iLine = info.line;
	
	int nGroup = FindGroup(iServer, false);
	ACString strServerGroup = GetGroup(nGroup).group_name;
	g_pGame->GetGameRun()->SetServerName(strServerGroup, info.server_name);
	
	//	��¼ԭ����ַ�����ڿ��������ص�ԭ��
	CECCrossServer::Instance().SetOrgServerIndex(iServer);

	//	��¼ԭ����ַ�����ڶ�������
	if (!CECReconnect::Instance().IsReconnecting()){
		CECReconnect::Instance().SetServerIndex(iServer);
	}

	//	���浽�ļ�
	SaveServer();
	return true;
}

int CECServerList::FindServerByAll(const AString &address, const ACString &name, int line)
{
	//	�����ļ��б������ݣ�������ƥ�������

	int iServer(-1);

	int iPort = 0;
	AString strDomain;
	if (!address.IsEmpty()){
		CECSplitHelperA ss(address, ":");
		if (ss.Count() > 1){
			iPort = ss.ToInt(0);
			strDomain = ss.ToString(1);
		}else{
			strDomain = ss.ToString(0);
		}
	}
	if (!strDomain.IsEmpty()){
		//	������Ϊ��ʱ���ƶ���������
		iServer = CECServerList::Instance().FindServerByAddress(strDomain, line, iPort);
		if (iServer < 0){
			iServer = CECServerList::Instance().FindServerByName(name, line);
		}
	}else if (!name.IsEmpty()){
		//	����Ϊ�ա����������Ʋ�Ϊ�գ����ݷ����������ƶ�
		iServer = CECServerList::Instance().FindServerByName(name, line);
	}

	return iServer;
}

void CECServerList::LoadServer()
{
	ACString strName;	//	strName ���������ƣ�����Ϊ��
	AString strAddress;	//	strAddress ����"29000:kuangshen.link.w2i.com.cn"��"kuangshen.link.w2i.com.cn"����Ϊ��
	int iLine;			//	iLine ����Ϊ0

	//	���ļ��ж�ȡ
	ACHAR szTemp[200];
	GetPrivateProfileString(_AL("Server"), _AL("CurrentServer"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	strName = szTemp;
	strName.TrimLeft();
	strName.TrimRight();

	GetPrivateProfileString(_AL("Server"), _AL("CurrentServerAddress"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	ACString strTemp = szTemp;
	strTemp.TrimLeft();
	strTemp.TrimRight();
	strAddress = AC2AS(strTemp);

	GetPrivateProfileString(_AL("Server"), _AL("CurrentLine"),
		_AL(""), szTemp, 200,  _AL("userdata\\currentserver.ini"));
	ACString strLine = szTemp;
	strLine.TrimLeft();
	strLine.TrimRight();
	iLine = strLine.ToInt();
	
	int iServer = FindServerByAll(strAddress, strName, iLine);
	SelectServer(iServer);
}

void CECServerList::SaveServer()
{
	CECGame::GAMEINIT &gi = g_pGame->GetGameInit();

	WritePrivateProfileString(_AL("Server"), _AL("CurrentServer"), AS2AC(gi.szServerName), 
		_AL("userdata\\currentserver.ini"));

	AString strAddress;
	strAddress.Format("%d:%s", gi.iPort, gi.szIP);
	WritePrivateProfileString(_AL("Server"), _AL("CurrentServerAddress"), AS2AC(strAddress),
		_AL("userdata\\currentserver.ini"));

	WritePrivateProfileString(_AL("Server"), _AL("CurrentLine"), ACString().Format(_AL("%d"), gi.iLine), 
		_AL("userdata\\currentserver.ini"));
}
