/*
 * FILE: EC_GameSession.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "gnet.h"
#include "gameclient.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GPDataType.h"
#include "EC_RTDebug.h"
#include "EC_LoginUIMan.h"
#include "EC_UIManager.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Instance.h"
#include "EC_NPC.h"
#include "EC_MsgDef.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_FixedMsg.h"
#include "EC_GameUIMan.h"
#include "EC_Team.h"
#include "EC_IvtrTypes.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EL_Precinct.h"
#include "EC_Faction.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_IvtrConsume.h"
#include "EC_GameTalk.h"
#include "EC_CommandLine.h"
#include "EC_CountryConfig.h"
#include "EC_CrossServer.h"
#include "EC_MCDownload.h"
#include "EC_AutoTeam.h"
#include "EC_RecordAccount.h"
#include "EC_ServerList.h"
#include "EC_ElsePlayer.h"
#include "EC_OfflineShopCtrl.h"
#include "EC_UIHelper.h"
#include "EC_FactionPVP.h"
#include "EC_TimeSafeChecker.h"
#include "EC_StubbornNetSender.h"

#ifdef ARC
#include "Arc/Asia/EC_ArcAsia.h"
#endif

#include "EC_RandomMapPreProcessor.h"
#include "EC_ProtocolDebug.h"
#include "EC_Reconnect.h"
#include "EC_LoginQueue.h"

#include "elementdataman.h"
#include "DlgChariot.h"
#include "DlgAutoLock.h"
#include "DlgBackOthers.h"
#include "DlgBackSelf.h"
#include "DlgMailWrite.h"
#include "DlgGuildMap.h"
#include "DlgGoldTrade.h"
#include "DlgQuestion.h"
#include "DlgChat.h"
#include "DlgWebMyShop.h"
#include "DlgCountryMap.h"
#include "DlgCountryReward.h"
#include "DlgQuickPay.h"
#include "DlgActivity.h"
#include "TaskTemplMan.h"
#include "GT/gt_overlay.h"

#include "DlgGuildMan.h"
#include "DlgFriendList.h"
#include "DlgMailToFriends.h"
#include "DlgWebViewProduct.h"

#include "DlgGivingFor.h"
#include "DlgMatchResult.h"

#include "cashlock_re.hpp"
#include "cashpasswordset_re.hpp"
#include "factionchat.hpp"
#include "factionacceptjoin_re.hpp"
#include "factionresign_re.hpp"
#include "factionresign_re.hpp"
#include "factionrename_re.hpp"
#include "factionmasterresign_re.hpp"
#include "factionleave_re.hpp"
#include "factionexpel_re.hpp"
#include "factiondelayexpelannounce.hpp"
#include "factionappoint_re.hpp"

#include "getfactionbaseinfo.hpp"
#include "setlockedlogoutuser.hpp"
#include "gethelpstates.hpp"
#include "gethelpstates_re.hpp"
#include "sethelpstates.hpp"
#include "sethelpstates_re.hpp"
#include "announceforbidinfo.hpp"
#include "worldchat.hpp"
#include "forbid.hxx"
#include "ids.hxx"
#include "autolockset_re.hpp"
#include "refgetreferencecode.hpp"
#include "refgetreferencecode_re.hpp"
#include "reflistreferrals.hpp"
#include "reflistreferrals_re.hpp"
#include "refwithdrawbonus.hpp"
#include "refwithdrawbonus_re.hpp"
#include "countrybattlesinglebattleresult.hpp"

#include "getrewardlist.hpp"
#include "getrewardlist_re.hpp"
#include "exchangeconsumepoints.hpp"
#include "exchangeconsumepoints_re.hpp"
#include "rewardmaturenotice.hpp"
#include "ssogetticket_re.hpp"
#include "countrybattlemove_re.hpp"
#include "countrybattlesyncplayerlocation.hpp"
#include "countrybattlegetkingcommandpoint_re.hpp"
#include "countrybattlekingassignassault_re.hpp"
#include "playerpositionresetrqst.hrp"

#include "playergivepresent_re.hpp"
#include "playeraskforpresent_re.hpp"

#include "qpaddcash_re.hpp"
#include "qpgetactivatedservices_re.hpp"
#include "qpannouncediscount.hpp"

#include "accountloginrecord.hpp"
#include "gmgetplayerconsumeinfo.hpp"
#include "AC.h"
#include "acremotecode.hpp"
#include "acreport.hpp"
#include "defence/defence.h"
#include "updateremaintime.hpp"
#include "globaldataman.h"
#include "acquestion.hpp"
#include "acanswer.hpp"
#include "defence/stackchecker.h"
#include "chatdata.h"
#include "auto_delete.h"
#include <AScriptFile.h>

#include "reportchat.hpp"
#include "playeraccuse.hpp"
#include "playeraccuse_re.hpp"

#include "playerrename_re.hpp"
#include "playernameupdate.hpp"
#include "playersendmassmail.hpp"

#ifdef RESOURCE_BATTLE
#include "factionresourcebattleplayerqueryresult.hpp"
#include "factionresourcebattlegetmap_re.hpp"
#include "factionresourcebattlegetrecord_re.hpp"
#include "factionresourcebattlenotifyplayerevent.hpp"
#endif

#include "waitqueuestatenotify.hpp"
#include "cancelwaitqueue.hpp"
#include "cancelwaitqueue_re.hpp"


#ifdef _MSC_VER
#include "HwLoader.h"
#endif

//#include <LogicCheck.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Base index of link error message
#define BASEIDX_LINKSEVERROR	10150

//	Message Length For PublicChat & PrivateChat
#define CHAT_ACHAR_LENGTH	120

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

extern abase::vector<ACRemoteCode*> g_DefenceCodeVec;
extern CRITICAL_SECTION g_csDefence;
extern CECConfigs g_GameCfgs;
///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

static volatile bool l_bExitConnect = false;

struct sInGameText{
	int iTextID;
	int iIsItemNeedDisplay;
};
typedef abase::hash_map<int ,sInGameText> BROADCASTMAP;
static  BROADCASTMAP l_mapBroadcast;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////
static AString GetLinkBrokenReasonDescription(CECGameSession::enumLinkBrokenReason reason){
	AString result;
	if (reason >= 0 && reason < CECGameSession::LBR_COUNT){
		static const char *s_reasonDescription[CECGameSession::LBR_COUNT] = {
			"Not Broken",
			"Challenge Overtime",
			"EnterWorld Overtime",
			"Anti Wallow",
			"Map Invalid",
			"Role Rename",
			"Passive broken before online",
			"Online passive broken",
			"Debug",
		};
		result = s_reasonDescription[reason];
	}else{
		result.Format("Unknown reason(%d)", reason);
	}
	return result;
}

static bool InitBoradcastMap()
{
	AScriptFile sf;
	if (!sf.Open("Configs\\item_msg_map.txt"))
	{
		a_LogOutput(1, "InitBoradcastMap, failed to open file item_msg_map.txt");
		return false;
	}

	l_mapBroadcast.clear();
	while (sf.PeekNextToken(true))
	{
		int iItemID = sf.GetNextTokenAsInt(true);
		
		int iTalkID = 266;//266:" "
		int iNeedDisplayItem = 0;
		if (sf.PeekNextToken(false))
		{
			iTalkID = sf.GetNextTokenAsInt(false);
		}
		else
		{
			//����3����
			a_LogOutput(1, "InitBoradcastMap(), File Format error, should be 3 parameters at line %d", sf.GetCurLine());
			sf.SkipLine();
			continue;
		}

		if (sf.PeekNextToken(false))
		{
			iNeedDisplayItem = sf.GetNextTokenAsInt(false);
		}
		else
		{
			//����3����
			a_LogOutput(1, "InitBoradcastMap(), File Format error, should be 3 parameters at line %d", sf.GetCurLine());
			sf.SkipLine();
			continue;
		}

		if (l_mapBroadcast.find(iItemID) != l_mapBroadcast.end())
		{
			//����������ͬ����Ʒid
			a_LogOutput(1, "InitBoradcastMap(), Duplicate item %d at line %d", iItemID, sf.GetCurLine());
			AUI_ReportError(__LINE__, __FILE__);
		}

		if(sf.PeekNextToken(false))
		{
			//����ַ�����
			sf.SkipLine();
		}

		l_mapBroadcast[iItemID].iTextID = iTalkID;
		l_mapBroadcast[iItemID].iIsItemNeedDisplay = iNeedDisplayItem;
	}
	
	sf.Close();	
	return true;
}

static void OnTaskChatMessage(const void* pBuf, size_t sz)
{
	const int iNameLen = 20;

	struct _data
	{
		int self_id;	//����������ҵĽ�ɫID
		int task_id;	//����ID
		int channel;	//������ѡ���Ƶ�� ��Ŀǰֻ�� ��ͨ�����ס����硢�����Ч
		int param;		//����ϵͳ�������Ĳ���
		int map_id;
		char name[iNameLen * 2];	//��󳤶�Ϊ20�ֽڵ�����
	};

	if (sz >= sizeof(_data))
	{
		_data* pData = (_data*)pBuf;
		ATaskTempl* pTempl = g_pGame->GetTaskTemplateMan()->GetTaskTemplByID(pData->task_id);

		if (pTempl && pTempl->GetTribute() )
		{
			wchar_t szName[iNameLen+1];
			szName[iNameLen] = 0;
			wcsncpy(szName, (wchar_t*)pData->name, iNameLen);
			g_pGame->GetGameRun()->AddPlayerName(pData->self_id, szName);
			ACString strMsg = pTempl->GetTribute();
			int nSub = strMsg.Find(_AL("$name"));
			while (nSub != -1)
			{
				strMsg = strMsg.Left(nSub) + _AL("&") + szName + _AL("&") + strMsg.Right(strMsg.GetLength() - nSub - 5);
				nSub = strMsg.Find(_AL("$name"));
			}

			ACString strMap = _AL("");
			CECInstance *pInstance = g_pGame->GetGameRun()->GetInstance(pData->map_id);
			if( pInstance )
				strMap = pInstance->GetName();
			nSub = strMsg.Find(_AL("$map"));
			while (nSub != -1)
			{
				strMsg = strMsg.Left(nSub) + strMap + strMsg.Right(strMsg.GetLength() - nSub - 5);
				nSub = strMsg.Find(_AL("$map"));
			}

			g_pGame->GetGameRun()->AddChatMessage(strMsg, pData->channel);
		}
	}
}

static bool OnFortressChatMessage(WorldChat* p, AArray<int, int>* pPendingFactions)
{
	if (pPendingFactions)
		pPendingFactions->RemoveAll();

	//	����Ƿ��ǻ��غ���Э��
	if (p->channel != GP_CHAT_SYSTEM)
		return false;

	switch(p->roleid)
	{
	case 44:
		break;
	default:
		return false;
	}

	//	���£����״̬�����Ծɷ��� true���Է�ֹ���������ظ�������Э��
	
	CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (!pUIMan)
	{
		ASSERT(false);
		return true;
	}
	CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
	if (!pGameUI)
	{
		ASSERT(false);
		return true;
	}
	CECFactionMan *pFMan = g_pGame->GetFactionMan();

	ACString strMsg;
	if (p->channel == GP_CHAT_SYSTEM)
	{
		switch(p->roleid)
		{
		case 44:		//	CMSG_FF_KEYBUILDINGDESTROY	ĳ���ɻ��������ؽ������ݻ�
			{
				if (p->msg.size() < sizeof(int)*2)
				{
					ASSERT(false);
					return true;
				}
				int *q = (int *)p->msg.begin();
				int factionid = q[0];
				int offense_faction = q[1];

				const Faction_Info *pInfo1 = pFMan->GetFaction(factionid);
				const Faction_Info *pInfo2 = pFMan->GetFaction(offense_faction);
				if (!pInfo1 || !pInfo2)
				{
					if (pPendingFactions)
					{
						//	���ӵ�����ѯ�����б����Ժ������������
						if (!pInfo1)
							pPendingFactions->UniquelyAdd(factionid);
						if (!pInfo2)
							pPendingFactions->UniquelyAdd(offense_faction);
					}
					//	�Ժ��ٴ���
					return false;
				}

				strMsg.Format(pGameUI->GetStringFromTable(9232), pInfo2->GetName(), pInfo1->GetName());
			}
			break;
			
		default:			
			//	����ķ�֧������Ӧ���Ѿ����ε���
			ASSERT(false);
			return false;
		}
	}
	
	if (!strMsg.IsEmpty())
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1);

	return true;
}

template <typename F>
ACString GetBuildingMaterialCount(const FACTION_BUILDING_ESSENCE *pEssence, const F & cost_fun)
{
	//	���ղ���
	ACString strCost;

	while (pEssence)
	{
		CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
		if (!pUIMan)
			break;
		CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
		if (!pGameUI)
			break;

		const int BUILDING_MATERIAL_COUNT = 5;
		ACString strTemp;
		int nMaterialReturn(0);
		for (int i = 0; i < BUILDING_MATERIAL_COUNT; ++ i)
		{
			nMaterialReturn = cost_fun(pEssence->material[i]);
			strTemp.Format(_AL("%s(%d)"), pGameUI->GetStringFromTable(9130+i), nMaterialReturn);
			if (!strCost.IsEmpty())
				strCost += _AL("");
			strCost += strTemp;
		}
		break;
	}

	return strCost;
}

struct BuildingMaterialCostOP
{
	BuildingMaterialCostOP(int accelerate)
		: m_accelerate(accelerate)
	{}

	int operator()(int count)const
	{
		int nMaterialSpeedup = count * m_accelerate;
		nMaterialSpeedup = (nMaterialSpeedup/10) + ((nMaterialSpeedup % 10) ? 1 : 0);
		return count + nMaterialSpeedup;
	}

	int m_accelerate;
};
ACString GetBuildingMaterialCost(const FACTION_BUILDING_ESSENCE *pEssence, int nAccelerate)
{
	//	�������ĵĲ��Ϸ�������
	return GetBuildingMaterialCount(pEssence, BuildingMaterialCostOP(nAccelerate));
}

struct BuildingMaterialReturnOP 
{
	int	operator()(int count)const
	{
		return count/2;
	}
};
ACString GetBuildingMaterialReturn(const FACTION_BUILDING_ESSENCE *pEssence)
{
	//	���յĲ��Ϸ�������
	return GetBuildingMaterialCount(pEssence, BuildingMaterialReturnOP());
}

static bool OnFortressChatMessage(ChatMessage* p, AArray<int, int>* pPendingFactions)
{
	if (pPendingFactions)
		pPendingFactions->RemoveAll();

	//	����Ƿ��ǻ��غ���Э��
	if (p->channel != GP_CHAT_SYSTEM)
		return false;

	if (p->srcroleid < 29 || p->srcroleid > 43 && p->srcroleid != 45)
	{
		return false;
	}

	//	���£����״̬�����Ծɷ��� true���Է�ֹ���������ظ�������Э��
	
	CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (!pUIMan)
	{
		ASSERT(false);
		return true;
	}
	CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
	if (!pGameUI)
	{
		ASSERT(false);
		return true;
	}
	CECFactionMan *pFMan = g_pGame->GetFactionMan();

	ACString strMsg;
	if (p->channel == GP_CHAT_SYSTEM)
	{
		switch(p->srcroleid)
		{
		case 29:		//	CMSG_FF_CHALLENGESTART	���ɻ�����ս��ʼ
			strMsg = pGameUI->GetStringFromTable(9230);
			g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1);
			return true;

		case 30:		//	CMSG_FF_BATTLESTART	���ɻ���ս����ʼ
			strMsg = pGameUI->GetStringFromTable(9231);
			g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1);
			return true;

		case 31:		//	CMSG_FF_CREATE	������ɻ��ش���
			strMsg = pGameUI->GetStringFromTable(9233);
			break;

		case 32:		//	CMSG_FF_HEALTHNOTIFY	���ɻ��ؽ�����
			{
				int new_health(0);
				try
				{
					GNET::Marshal::OctetsStream os(p->msg);
					os >> new_health;
				}
				catch (...)
				{
					ASSERT(false);
					return true;
				}
				if (new_health <= 0)
				{
					strMsg = pGameUI->GetStringFromTable(9234);
				}
				else if (new_health <= 30)
				{
					strMsg = pGameUI->GetStringFromTable(9235);
				}
			}
			break;

		case 33:		//	CMSG_FF_BECHALLENGED	���ɻ��ر���İ��ɳɹ���ս
			{
				int offense_faction(0);
				try
				{
					GNET::Marshal::OctetsStream os(p->msg);
					os >> offense_faction;
				}
				catch (...)
				{
					ASSERT(false);
					return true;
				}
				const Faction_Info *pInfo = pFMan->GetFaction(offense_faction);
				if (!pInfo)
				{
					if (pPendingFactions)
						pPendingFactions->UniquelyAdd(offense_faction);
					return false;
				}

				strMsg.Format(pGameUI->GetStringFromTable(9236), pInfo->GetName());
			}
			break;

		case 34:		//	CMSG_FF_CHALLENGESUCCESS	���ɻ������İ��ɳɹ���ս
			{
				int factionid(0);
				try
				{
					GNET::Marshal::OctetsStream os(p->msg);
					os >> factionid;
				}
				catch (...)
				{
					ASSERT(false);
					return true;
				}
				const Faction_Info *pInfo = pFMan->GetFaction(factionid);
				if (!pInfo)
				{
					if (pPendingFactions)
						pPendingFactions->UniquelyAdd(factionid);
					return false;
				}

				strMsg.Format(pGameUI->GetStringFromTable(9237), pInfo->GetName());
			}
			break;

		case 35:		//	CMSG_FF_BATTLESTARTNOTIFY	���ɻ���ս����ʼ
			strMsg = pGameUI->GetStringFromTable(9239);
			break;

		case 36:		//	CMSG_FF_BATTLEENDNOTIFY	���ɻ���ս������
			strMsg = pGameUI->GetStringFromTable(9240);
			break;

		case 37:		//	CMSG_FF_LEVELUP	���ɻ�������
			{
				if (p->msg.size() < sizeof(int))
				{
					ASSERT(false);
					return true;
				}
				int level = *(int *)p->msg.begin();
				strMsg.Format(pGameUI->GetStringFromTable(9241), level);
			}
			break;

		case 38:		//	CMSG_FF_TECHNOLOGYUP	���ɻ��ؿƼ�����
			{
				if (p->msg.size() < sizeof(int)*2)
				{
					ASSERT(false);
					return true;
				}
				int *q = (int *)p->msg.begin();
				int tech_index = q[0];
				int tech_level = q[1];
				strMsg.Format(pGameUI->GetStringFromTable(9242), pGameUI->GetStringFromTable(9260+tech_index), tech_level);
			}
			break;

		case 39:		//	CMSG_FF_CONSTRUCT	���ɻ�����ʩ��ʼ����
			{
#pragma pack(1)
				struct  _data
				{
					int id;
					int accelerate;
					int overlap_id;
				};
#pragma pack()
				if (p->msg.size() < sizeof(_data))
				{
					ASSERT(false);
					return true;
				}
				_data* pData = (_data*)p->msg.begin();
				const FACTION_BUILDING_ESSENCE *pEssence = g_pGame->GetGameRun()->GetHostPlayer()->GetBuildingEssence(pData->id);
				if (!pEssence)
				{
					ASSERT(false);
					return true;
				}
				strMsg.Format(pGameUI->GetStringFromTable(9243), pEssence->name, GetBuildingMaterialCost(pEssence, pData->accelerate));
				const FACTION_BUILDING_ESSENCE *pEssenceOverlap = g_pGame->GetGameRun()->GetHostPlayer()->GetBuildingEssence(pData->overlap_id);
				if (pEssenceOverlap)
				{
					ACString strMsg2;
					strMsg2.Format(pGameUI->GetStringFromTable(9247), GetBuildingMaterialReturn(pEssenceOverlap));
					strMsg += strMsg2;
				}
			}
			break;

		case 40:		//	CMSG_FF_CONSTRUCTCOMPLETE	���ɻ�����ʩ�������
			{
				if (p->msg.size() < sizeof(int))
				{
					ASSERT(false);
					return true;
				}
				int idBuilding = *(int*)p->msg.begin();
				const FACTION_BUILDING_ESSENCE *pEssence = g_pGame->GetGameRun()->GetHostPlayer()->GetBuildingEssence(idBuilding);
				if (!pEssence)
				{
					ASSERT(false);
					return true;
				}
				strMsg.Format(pGameUI->GetStringFromTable(9244), pEssence->name);
			}
			break;

		case 41:		//	CMSG_FF_HANDINCONTRIB	�����Ͻ��ɹ��׶�
			{
#pragma pack(1)
				const int USERNAME_LENGTH = 20;
				struct  _data
				{
					int contrib;
					char name[USERNAME_LENGTH * 2];
				};
#pragma pack()
				if (p->msg.size() < sizeof(_data))
				{
					ASSERT(false);
					return true;
				}
				_data* pData = (_data*)p->msg.begin();

				wchar_t szName[USERNAME_LENGTH+1];
				szName[USERNAME_LENGTH] = 0;
				wcsncpy(szName, (wchar_t*)pData->name, USERNAME_LENGTH);
				g_pGame->GetGameSession()->GetPlayerIDByName(szName, 0);

				strMsg.Format(pGameUI->GetStringFromTable(9245), szName, pData->contrib);
			}
			break;

		case 42:		//	CMSG_FF_HANDINMATERIAL	�����Ͻɲ���
			{
#pragma pack(1)
				const int USERNAME_LENGTH = 20;
				struct  _data
				{
					int material_id;
					int material_count;
					char name[USERNAME_LENGTH * 2];
				};
#pragma pack()
				if (p->msg.size() < sizeof(_data))
				{
					ASSERT(false);
					return true;
				}
				_data* pData = (_data*)p->msg.begin();

				wchar_t szName[USERNAME_LENGTH+1];
				szName[USERNAME_LENGTH] = 0;
				wcsncpy(szName, (wchar_t*)pData->name, USERNAME_LENGTH);
				g_pGame->GetGameSession()->GetPlayerIDByName(szName, 0);

				CECIvtrFactionMaterial *pMaterial = (CECIvtrFactionMaterial *)CECIvtrItem::CreateItem(pData->material_id, 0, pData->material_count);
				if (!pMaterial)
				{
					ASSERT(false);
					return true;
				}

				strMsg.Format(pGameUI->GetStringFromTable(9246), szName, pData->material_count, pMaterial->GetName());
				delete pMaterial;
			}
			break;

		case 43:		//	CMSG_FF_BATTLEPREPARECLEAR	���ɻ���ս��ǰ�峡
			strMsg = pGameUI->GetStringFromTable(9238);
			break;

		case 45:		//	CMSG_FF_DISMANTLE	���ɻ�����ʩ���գ������
			{
#pragma pack(1)
				struct _data
				{
					int id;
					int type;
				};
#pragma pack()

				if (p->msg.size() < sizeof(_data))
				{
					ASSERT(false);
					return true;
				}

				_data* pData = (_data*)p->msg.begin();
				const FACTION_BUILDING_ESSENCE *pEssence = g_pGame->GetGameRun()->GetHostPlayer()->GetBuildingEssence(pData->id);
				if (!pEssence)
				{
					ASSERT(false);
					return true;
				}
				if( pData->type == 1 )
					strMsg.Format(pGameUI->GetStringFromTable(9248), pEssence->name, GetBuildingMaterialReturn(pEssence));
				else if( pData->type == 2 )
					strMsg.Format(pGameUI->GetStringFromTable(9258), pEssence->name);
			}
			break;

		default:
			//	����ķ�֧������Ӧ���Ѿ����ε���
			ASSERT(false);
			return false;
		}
	}
	
	if (!strMsg.IsEmpty())
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_FACTION);

	return true;
}

static bool OnCountryChatMessage(ChatMessage* p)
{
	if (p->channel != GP_CHAT_SYSTEM)
		return false;
		
	//	���£����״̬�����Ծɷ��� true���Է�ֹ���������ظ�������Э��
	if (p->srcroleid < 46 || p->srcroleid > 49)
		return false;
	
	CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (!pUIMan)
	{
		ASSERT(false);
		return true;
	}
	CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
	if (!pGameUI)
	{
		ASSERT(false);
		return true;
	}

	int country_id(0), domain_id(0);
	try
	{
		Marshal::OctetsStream(p->msg)>>country_id>>domain_id;
	}
	catch (...)
	{
		ASSERT(false);
		return false;
	}
	if (!CECCountryConfig::Instance().ValidateID(country_id))
	{
		ASSERT(false);
		return false;
	}
	const DOMAIN2_INFO *pDomain = CECDomainCountryInfo::GetInstance()->Find(domain_id);
	if (!pDomain)
	{
		ASSERT(false);
		return false;
	}
	const ACString &strCountryName = *CECCountryConfig::Instance().GetName(country_id);
	
	ACString strMsg;
	int idString(-1);
	switch(p->srcroleid)
	{
	case 46:
		//strMsg.Format(pGameUI->GetStringFromTable(9984), strCountryName, pDomain->name);
		break;
	case 47:
		//strMsg.Format(pGameUI->GetStringFromTable(9985), pDomain->name, strCountryName);
		break;
	case 48:
		strMsg.Format(pGameUI->GetStringFromTable(9986), strCountryName, pDomain->name);
		break;
	case 49:
		strMsg.Format(pGameUI->GetStringFromTable(9987), strCountryName, pDomain->name);
		break;
		
	default:
		return false;
	}
	
	if (!strMsg.IsEmpty())
	{
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_COUNTRY);
		CDlgCountryMap* pDlgCountryMap = (CDlgCountryMap*)pGameUI->GetDialog("Win_CountryMap");
		if (pDlgCountryMap->IsShow())
			pDlgCountryMap->OnCommandChat(NULL);
	}
	
	return true;
}

static bool OnReportPluginMessage(const Protocol *pProtocol)
{
	bool bProcessed(false);
	if (pProtocol->GetType() == PROTOCOL_PLAYERACCUSE_RE){
		ACString s;
		const PlayerAccuse_Re *p = static_cast<const PlayerAccuse_Re*>(pProtocol);
		if (CECUIHelper::GetReportPluginResult(s, p->recode, p->dst_roleid, false)){
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->AddChatMessage(s, GP_CHAT_MISC);
			bProcessed = true;
		}
	}
	return bProcessed;
}

#ifdef RESOURCE_BATTLE

static bool OnPrtcFactionPVPResult(const Protocol *pProtocol, bool bFirst)
{
	bool bProcessed(false);
	
	if (pProtocol->GetType() == PROTOCOL_FACTIONRESOURCEBATTLEPLAYERQUERYRESULT){
		const FactionResourceBattlePlayerQueryResult *p = (const FactionResourceBattlePlayerQueryResult *)(pProtocol);
		if (g_pGame->GetFactionMan()->GetFaction(p->faction_id, bFirst)){
			CECFactionPVPModel::Instance().OnPrtcFactionPVPResult((const GNET::FactionResourceBattlePlayerQueryResult *)pProtocol);
			bProcessed = true;
		}
	}
	return bProcessed;
}

static void OnFactionPVPChatMessage(ChatMessage* p){	
	if (p->srcroleid < 60 || p->srcroleid > 64){
		return;
	}
	CECGameUIMan *pGameUIMan = CECUIHelper::GetGameUIMan();
	switch (p->srcroleid){
	case 60:
		pGameUIMan->AddChatMessage(pGameUIMan->GetStringFromTable(11272), GP_CHAT_FACTION);
		break;
	case 61:
		pGameUIMan->AddChatMessage(pGameUIMan->GetStringFromTable(11273), GP_CHAT_FACTION);
		break;
	case 62:
		pGameUIMan->AddChatMessage(pGameUIMan->GetStringFromTable(11274), GP_CHAT_FACTION);
		break;
	case 63:
		CECFactionPVPModel::Instance().OnFactionPVPOpen(true);
		break;
	case 64:
		CECFactionPVPModel::Instance().OnFactionPVPOpen(false);
		break;
	}
}
#endif
static bool OnKingChatMessage(ChatMessage* p, bool bGetPlayerName=true)
{
	if (p->channel != GP_CHAT_SYSTEM)
		return false;
	
	//	���£����״̬�����Ծɷ��� true���Է�ֹ���������ظ�������Э��
	if (p->srcroleid < 50 || p->srcroleid > 59)
		return false;
	
	CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (!pUIMan)
	{
		ASSERT(false);
		return true;
	}

	CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
	if (!pGameUI)
	{
		ASSERT(false);
		return true;
	}

	ACString strMsg;
	abase::vector<ACString> candidates;

	try
	{
		switch(p->srcroleid)
		{
		case 54:
			{
				bool bNameNotFound = false;
				size_t count;
				abase::vector<int> ids;
				Marshal::OctetsStream os(p->msg);
				os >> count;
				for( size_t i=0;i<count;i++ )
				{
					int roleid;
					os >> roleid;

					const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(roleid, false);
					if( szName )
						candidates.push_back(szName);
					else
					{
						if( bGetPlayerName )
							ids.push_back(roleid);
						bNameNotFound = true;
					}
				}

				if( bNameNotFound )
				{
					if( ids.size() )
						g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(ids.size(), &ids[0], 1);
					return false;
				}
				else
				{
					ACString strNames;
					for( size_t i=0;i<candidates.size();i++ )
						strNames += _AL("&") + candidates[i] + _AL("&, ");
					if( candidates.size() )
					{
						strNames.CutRight(2);
						strMsg.Format(pGameUI->GetStringFromTable(10354), strNames);
					}
					else
						strMsg = pGameUI->GetStringFromTable(10320);
				}
			}
			break;
		case 58:
		case 59:
			{
				int roleid, vote_count;
				Marshal::OctetsStream(p->msg) >> roleid >> vote_count;

				const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(roleid, false);
				if( szName )
				{
					if( p->srcroleid == 58 )
						strMsg.Format(pGameUI->GetStringFromTable(10358), szName, vote_count);
					else
						strMsg.Format(pGameUI->GetStringFromTable(10359), szName, vote_count);
				}
				else
				{
					if( bGetPlayerName )
						g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(1, &roleid, 1);
					return false;
				}
			}
			break;
		case 57:
			{
				int roleid;
				Marshal::OctetsStream(p->msg) >> roleid;
				if (roleid > 0){					
					const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(roleid, false);
					if( szName )
						strMsg.Format(pGameUI->GetStringFromTable(10357), szName);
					else
					{
						if( bGetPlayerName )
							g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(1, &roleid, 1);
						return false;
					}
				}
			}
			break;
		case 50:
			{
				char op;
				int roleid, domain_id;
				Marshal::OctetsStream(p->msg) >> roleid >> domain_id >> op;

				DOMAIN2_INFO* pInfo = CECDomainCountryInfo::GetInstance()->Find(domain_id);
				if( pInfo )
				{
					strMsg.Format(pGameUI->GetStringFromTable(10350), pInfo->name, pGameUI->GetStringFromTable((op == DOMAIN2_INFO::STATE_SMALLTOKEN) ? 10316 : 10317));
					g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_COUNTRY);
					CDlgCountryMap* pDlgCountryMap = (CDlgCountryMap*)pGameUI->GetDialog("Win_CountryMap");
					if (pDlgCountryMap->IsShow()) pDlgCountryMap->OnCommandChat(NULL);
					return true;
				}
			}
			break;
		case 51:
			{
				int roleid, domain_id;
				Marshal::OctetsStream(p->msg) >> roleid >> domain_id;

				DOMAIN2_INFO* pInfo = CECDomainCountryInfo::GetInstance()->Find(domain_id);
				if( pInfo )
				{
					strMsg.Format(pGameUI->GetStringFromTable(10351), pInfo->name);
					g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_COUNTRY);
					CDlgCountryMap* pDlgCountryMap = (CDlgCountryMap*)pGameUI->GetDialog("Win_CountryMap");
					if (pDlgCountryMap->IsShow()) pDlgCountryMap->OnCommandChat(NULL);
					return true;
				}
			}
			break;
		default:
			{
				strMsg = pGameUI->GetStringFromTable(10350 + p->srcroleid - 50);
			}
		}
	}
	catch (...)
	{
		ASSERT(false);
		a_LogOutput(1, "OnKingChatMessage, Message id(%d), Parsing role id error!", p->srcroleid);
		return false;
	}

	if (!strMsg.IsEmpty())
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST);

	return true;
}

static bool OnBattleChatMessage(ChatMessage* p, AArray<int, int>* pPendingFactions)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgGuildMap* pDlgGuildMap = (CDlgGuildMap*)pGameUI->GetDialog("Win_GuildMap");

	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();
	CECFactionMan *pMan = g_pGame->GetFactionMan();

	ACString strMsg;

	if(pPendingFactions)
	{
		pPendingFactions->RemoveAll();
	}

	switch(p->srcroleid)
	{
	case 1: //CMSG_BIDSTART
		{
			strMsg = pGameUI->GetStringFromTable(4001);
			break;
		}
	case 2: //CMSG_BIDEND
		{
			int group = p->msg.size() / (sizeof(short) + 2 * sizeof(unsigned int));
			strMsg = pGameUI->GetStringFromTable(4002);

			if(!group)
			{
				break;
			}

			int idFaction = g_pGame->GetGameRun()->GetHostPlayer()->GetFactionID();
			ACString strAttacker;
			ACString strDefender;
			ACString strAnnounce;

			// message will be shown by pDlgGuildMap
			pDlgGuildMap->ClearMessage();
			Marshal::OctetsStream o(p->msg);
			for(;group > 0;group--)
			{
				short id;
				unsigned int owner;
				unsigned int challenger;
				o >> id >> owner >> challenger;
								
				Faction_Info *finfo = pMan->GetFaction(challenger);
				if( (int)challenger == idFaction )
					strAttacker = pGameUI->GetStringFromTable(4500);
				else if(finfo)
					strAttacker.Format(pGameUI->GetStringFromTable(4499), finfo->GetName());
				else if(pPendingFactions)
					pPendingFactions->UniquelyAdd(challenger);

				if(strAttacker.IsEmpty())
				{
					// skip empty name
					strMsg.Empty();
					continue;
				}
				
				for(DWORD i = 0; i < info->size(); i++ )
				{
					if((*info)[i].id == id )
					{					
						if( (*info)[i].id_owner )
						{
							if((*info)[i].id_owner == idFaction )
								strDefender = pGameUI->GetStringFromTable(4500);
							else
							{
								Faction_Info *finfo = pMan->GetFaction((*info)[i].id_owner);
								if( finfo )
									strDefender.Format(pGameUI->GetStringFromTable(4499), finfo->GetName());
								else if(pPendingFactions)
									pPendingFactions->UniquelyAdd((*info)[i].id_owner);
							}
						}
						
						if( strDefender == _AL(""))
							strAnnounce.Format(	pGameUI->GetStringFromTable(4511),
							strAttacker, (*info)[i].name );
						else
							strAnnounce.Format(	pGameUI->GetStringFromTable(4512), 
							strAttacker, strDefender, (*info)[i].name);
						
						if(!pPendingFactions || pPendingFactions->GetSize() == 0)
						{
							// insert to queue
							pDlgGuildMap->AddMessage(strAnnounce);
						}
						
						break;
					}
				}

				strAttacker = _AL("");
				strDefender = _AL("");
			}

			if((pPendingFactions && pPendingFactions->GetSize() > 0) || strMsg.IsEmpty() )
			{
				// remove previous added messages
				pDlgGuildMap->ClearMessage();
				strMsg.Empty();
			}
			
			break;
		}
		
	case 3: //CMSG_BATTLESTART
		{
			if( p->msg.size() < 2 * sizeof(char) )
			{
				return false;
			}

			char id, result;
			Marshal::OctetsStream(p->msg) >> id >> result;
			for(DWORD i = 0; i < info->size(); i++ )
			{
				if((*info)[i].id == id )
				{
					strMsg.Format(pGameUI->GetStringFromTable(4003), (*info)[i].name);
					strMsg += pGameUI->GetStringFromTable(3900 + result);
					break;
				}
			}
			break;
		}

	case 4: //CMSG_BATTLEEND
		{
			if( p->msg.size() < sizeof(char) )
			{
				return false;
			}

			char id;
			Marshal::OctetsStream(p->msg) >> id;
			for(DWORD i = 0; i < info->size(); i++ )
			{
				if((*info)[i].id == id )
				{
					strMsg.Format(pGameUI->GetStringFromTable(4004), (*info)[i].name);
					break;
				}
			}
			break;
		}

	case 6: //CMSG_BONUSSEND
		{
			if( p->msg.size() < sizeof(unsigned int) * 3)
			{
				return false;
			}

			unsigned int money, id, count;
			Marshal::OctetsStream(p->msg) >> money >> id >> count;

			CECIvtrItem *pItem = CECIvtrItem::CreateItem(id, 0, count);
			strMsg.Format(pGameUI->GetStringFromTable(4006), pItem->GetName(), pItem->GetCount());
			delete pItem;

			break;
		}

	case 7: //CMSG_SPECIAL
		{
			if( p->msg.size() < sizeof(short) )
			{
				return false;
			}
			
			short id;
			Marshal::OctetsStream(p->msg) >> id;
			for(DWORD i = 0; i < info->size(); i++ )
			{
				if((*info)[i].id == id )
				{
					unsigned int fid = (*info)[i].id_owner;
					if(fid > 0)
					{
						Faction_Info *finfo = pMan->GetFaction(fid);
						if(finfo)
						{
							strMsg.Format(pGameUI->GetStringFromTable(4007), 
								(*info)[i].name, (*info)[i].name, finfo->GetName());
						}
						else if(pPendingFactions)
							pPendingFactions->UniquelyAdd(fid);
					}
					else
					{
						strMsg.Format(pGameUI->GetStringFromTable(4005), (*info)[i].name);
					}
					
					break;
				}
			}
			break;
		}

	default:
		return false;
	}

	if( (pPendingFactions && pPendingFactions->GetSize() > 0) || strMsg.IsEmpty() )
	{
		// faction info is pending
		return false;
	}
	else
	{
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, p->srcroleid, NULL, 1, p->emotion);
		return true;
	}
}

static bool OnBattleChatMessage(WorldChat* p, AArray<int, int>* pPendingFactions)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgGuildMap* pDlgGuildMap = (CDlgGuildMap*)pGameUI->GetDialog("Win_GuildMap");

	int idFaction = g_pGame->GetGameRun()->GetHostPlayer()->GetFactionID();
	CECFactionMan *pMan = g_pGame->GetFactionMan();
	abase::vector<DOMAIN_INFO> *info = globaldata_getdomaininfos();

	ACString strName((const ACHAR*)p->name.begin(), p->name.size() / sizeof (ACHAR));
	ACString strMsg = _AL("");
	ACString strAttacker = _AL("");
	ACString strDefender = _AL("");

	if(pPendingFactions)
	{
		pPendingFactions->RemoveAll();
	}

	switch(p->roleid)
	{
		case 3: //CMSG_BATTLESTART
		{
			if( p->msg.size() < sizeof(unsigned char) + sizeof(unsigned int))
			{
				return false;
			}

			unsigned char id;
			unsigned int fid;
			Marshal::OctetsStream(p->msg) >> id >> fid;
			Faction_Info *finfo = pMan->GetFaction(fid);
			if( !finfo && !strName.IsEmpty() )
				pMan->AddFaction(fid, strName, strName.GetLength() * sizeof(ACHAR), 3, 200);
			
			if( (int)fid == idFaction )
				strAttacker = pGameUI->GetStringFromTable(4500);
			else
				strAttacker.Format(pGameUI->GetStringFromTable(4499), strName);
			
			for(DWORD i = 0; i < info->size(); i++ )
				if((*info)[i].id == id )
				{
					if( pDlgGuildMap )
						if( (*info)[i].id_owner && (*info)[i].id_owner == idFaction ||
							(int)fid == idFaction )
							pDlgGuildMap->m_vecAreaType[i] |= CDlgGuildMap::AREA_TYPE_CANENTER;
						if( (*info)[i].id_owner )
						{
							if((*info)[i].id_owner == idFaction )
								strDefender = pGameUI->GetStringFromTable(4500);
							else
							{
								Faction_Info *finfo = pMan->GetFaction((*info)[i].id_owner);
								if( finfo )
									strDefender.Format(pGameUI->GetStringFromTable(4499), 
									finfo->GetName());
								else if(pPendingFactions)
									pPendingFactions->UniquelyAdd((*info)[i].id_owner);
							}
						}
						if( strDefender == _AL(""))
							strMsg.Format(pGameUI->GetStringFromTable(4501), 
							strAttacker, (*info)[i].name );
						else if( idFaction != 0 && 
							((*info)[i].id_owner == idFaction || (int)fid == idFaction) )
							strMsg.Format(pGameUI->GetStringFromTable(4502), 
							strAttacker, strDefender, (*info)[i].name );
						else
							strMsg.Format(pGameUI->GetStringFromTable(4503), 
							(*info)[i].name, strAttacker, strDefender);
						break;
					}
			break;
		}

		case 4: //CMSG_BATTLEEND
		{
			if( p->msg.size() < 2 * sizeof(unsigned int) + 3 * sizeof(unsigned char) )
			{
				return false;
			}

			unsigned char id, result, color;
			unsigned int amount, fid;
			Marshal::OctetsStream(p->msg) >> id >> result >> amount >> fid >> color;
			Faction_Info *finfo = pMan->GetFaction(fid);
			if( !finfo && !strName.IsEmpty())
				pMan->AddFaction(fid, strName, strName.GetLength() * sizeof(ACHAR), 3, 200);
			
			if( (int)fid == idFaction )
				strAttacker = pGameUI->GetStringFromTable(4500);
			else
				strAttacker.Format(pGameUI->GetStringFromTable(4499), strName);
			
			for(DWORD i = 0; i < info->size(); i++ )
			{
				if((*info)[i].id == id )
				{
					if( (*info)[i].id_owner )
					{
						if((*info)[i].id_owner == idFaction )
							strDefender = pGameUI->GetStringFromTable(4500);
						else
						{
							Faction_Info *finfo = pMan->GetFaction((*info)[i].id_owner);
							if( finfo )
								strDefender.Format(pGameUI->GetStringFromTable(4499), 
								finfo->GetName());
							else if(pPendingFactions)
								pPendingFactions->UniquelyAdd((*info)[i].id_owner);
						}
					}

					if( strDefender == _AL(""))
					{
						if( result == 1 )
							strMsg.Format(pGameUI->GetStringFromTable(4504), 
							strAttacker, (*info)[i].name );
						else
							strMsg.Format(pGameUI->GetStringFromTable(4505), 
							strAttacker, (*info)[i].name );
					}
					else
					{
						if( result == 1 )
							strMsg.Format(pGameUI->GetStringFromTable(4506), 
							strAttacker, strDefender, (*info)[i].name, strAttacker );
						else
							strMsg.Format(pGameUI->GetStringFromTable(4506), 
							strAttacker, strDefender, (*info)[i].name, strDefender );
					}
					if( result == 1 )
					{
						(*info)[i].id_owner = fid;
						(*info)[i].id_challenger = 0;
						(*info)[i].battle_time = 0;
						(*info)[i].color = color;
					}
					else
					{
						(*info)[i].id_challenger = 0;
						(*info)[i].battle_time = 0;
					}
					if( pDlgGuildMap )
						pDlgGuildMap->UpdateNormalMap();
					break;
				}
			}
			break;
		}

		case 5://CMSG_BATTLEBID
		{
			if( p->msg.size() < 4 * sizeof(unsigned int) + sizeof(unsigned char) )
			{
				return false;
			}

			unsigned char id;
			unsigned int fid, amount, loser, cutofftime;
			Marshal::OctetsStream(p->msg) >> id >> fid >> amount >> loser >> cutofftime;

// 			Faction_Info *finfo = pMan->GetFaction(fid);
// 			if( !finfo )
// 				pMan->AddFaction(fid, strName, strName.GetLength() * sizeof(ACHAR), 3, 200);
// 
// 			if( (int)fid == idFaction )
// 				strAttacker = pGameUI->GetStringFromTable(4500);
// 			else
// 				strAttacker.Format(pGameUI->GetStringFromTable(4499), strName);

			// hide the attacker's name
			strAttacker = pGameUI->GetStringFromTable(4498);

			for(DWORD i = 0; i < info->size(); i++ )
			{
				if((*info)[i].id == id )
				{
					//(*info)[i].id_tempchallenger = fid;
					//(*info)[i].deposit = amount;
					(*info)[i].cutoff_time = cutofftime;
					if( pDlgGuildMap )
						pDlgGuildMap->UpdateChallengeMap();

					if( (*info)[i].id_owner )
					{
						if((*info)[i].id_owner == idFaction )
							strDefender = pGameUI->GetStringFromTable(4500);
						else
						{
							Faction_Info *finfo = pMan->GetFaction((*info)[i].id_owner);
							if( finfo )
								strDefender.Format(pGameUI->GetStringFromTable(4499), 
									finfo->GetName());
							else if(pPendingFactions)
								pPendingFactions->UniquelyAdd((*info)[i].id_owner);
						}
					}

					if( strDefender == _AL(""))
						strMsg.Format(pGameUI->GetStringFromTable(4508), 
							strAttacker, (*info)[i].name );
					else
						strMsg.Format(pGameUI->GetStringFromTable(4509), 
							strAttacker, strDefender, (*info)[i].name);
					break;
				}
			}
			break;
		}
		default:
			return false;
	}

	
	if( (pPendingFactions && pPendingFactions->GetSize() > 0) || strMsg == _AL("") )
	{
		// faction info is pending or no message
		return false;
	}
	else
	{
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1, p->emotion);
		return true;
	}
}

static bool OnFactionDiplomacyMessage(WorldChat *pProtocol)
{
	//	���ǰ����⽻����Э�飬������Ҫ����Ϣ��Щ���ܵõ�������false
	//

	//	���Э���Ƿ�Ϊ�⽻Э��
	if (pProtocol->channel != GP_CHAT_SYSTEM)
		return false;

	switch (pProtocol->roleid)
	{
	case 25:
	case 26:
	case 27:
	case 28:
		break;
	default:
		return false;
	}

	int channel = pProtocol->channel;
	int roleid = pProtocol->roleid;

	GNET::Marshal::OctetsStream os(pProtocol->msg);

	//	��ȡ��������
	Octets fname1, fname2;
	try
	{
		os >> fname1 >> fname2;
	}
	catch (...)
	{
		//	�޷���ȷ��ȡ������������
		ASSERT(false);
		return true;		//	���� true ��ֹ�����ط��ٴ�������ͬ
	}
	
	ACString strName1((const ACHAR*)fname1.begin(), fname1.size() / sizeof (ACHAR));
	ACString strName2((const ACHAR*)fname2.begin(), fname2.size() / sizeof (ACHAR));

	CECUIManager *pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (!pUIMan)
	{
		return true;
	}
	CECGameUIMan* pGameUI = pUIMan->GetInGameUIMan();
	if (!pGameUI)
	{
		return true;
	}

	ACString strMsg;
	switch(roleid)
	{
	case 25:		//	CMSG_FACTION_BECAME_ALLIANCE
		{
			//	��������ͬ��
			strMsg.Format(pGameUI->GetStringFromTable(9210), strName1, strName2);
			strMsg = _AL("^00FF00") + strMsg;
		}
		break;

	case 26:		//	CMSG_FACTION_REMOVE_ALLIANCE
		{
			//	�������ͬ��
			int force(0);	//	�Ƿ�Ϊǿ�ƽ��
			try
			{
				os >> force;
			}
			catch (...)
			{
				ASSERT(false);
				return true;
			}
			int idString =  force ? 9212 : 9211;
			strMsg.Format(pGameUI->GetStringFromTable(idString), strName1, strName2);
		}
		break;

	case 27:		//	CMSG_FACTION_BECAME_HOSTILE
		{
			//	�������ɵж�
			strMsg.Format(pGameUI->GetStringFromTable(9213), strName1, strName2);
		}
		break;

	case 28:		//	CMSG_FACTION_REMOVE_HOSTILE
		{
			//	������ɵж�
			int force(0);	//	�Ƿ�Ϊǿ�ƽ��
			try
			{
				os >> force;
			}
			catch (...)
			{
				ASSERT(false);
				return true;
			}
			int idString =  force ? 9215 : 9214;
			strMsg.Format(pGameUI->GetStringFromTable(idString), strName1, strName2);
			strMsg = _AL("^00FF00") + strMsg;
		}
		break;

	default:
		//	����ķ�֧������Ӧ���Ѿ����ε��ǰ����⽻������
		ASSERT(false);
		return false;
	}
	
	if (!strMsg.IsEmpty())
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1);
	return true;
}

static bool OnEarthBagMessage(WorldChat *p, int *pPlayerID=NULL)
{
	//	ɳ������

#pragma pack(1)
	struct _data
	{
		int roleid;		//	���ID
		int level;		//	��ҵȼ�
		int dps;			//	
		int dph;			//	
		char rank;		//	����
	};
#pragma pack()

	bool bRet(false);

	if (pPlayerID)
		*pPlayerID = 0;

	while (p->channel == GP_CHAT_SYSTEM)
	{
		if (p->roleid != 101 || p->msg.size() != sizeof(_data))
			break;

		const _data *pData = (const _data *)(p->msg.begin());
		if (pPlayerID)
			*pPlayerID = pData->roleid;

		const ACHAR *szName = g_pGame->GetGameRun()->GetPlayerName(pData->roleid, false);
		if (!szName)
			break;

		CECGameRun *pGameRun = g_pGame->GetGameRun();

		CECGameUIMan *pGameUIMan = pGameRun->GetUIManager()->GetInGameUIMan();
		if (pGameUIMan)
		{
			ACString strMsg;
			strMsg.Format(pGameUIMan->GetStringFromTable(9611), szName, pData->dph);
			if (!strMsg.IsEmpty())
				pGameRun->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1);
		}

		bRet = true;
		break;
	}

	return bRet;
}

static void OnFactionRenameMessage(const ACString &strNewName, const ACString &strOldName){
	CECGameRun *pGameRun = g_pGame->GetGameRun();	
	CECGameUIMan *pGameUIMan = pGameRun->GetUIManager()->GetInGameUIMan();
	if (pGameUIMan){
		ACString strMsg;
		strMsg.Format(pGameUIMan->GetStringFromTable(11435), strOldName, strNewName);
		pGameRun->AddChatMessage(strMsg, GP_CHAT_BROADCAST);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGameSession
//	
///////////////////////////////////////////////////////////////////////////

CECGameSession::CECGameSession() :
m_aNewProtocols(128, 128),
m_aOldProtocols(128, 128),
m_Ping(50, 50),
m_aStubbornNetSenders(1, 1),
m_stubbornFactionInfoSender(NULL)
{
	if (!(m_pNetMan = &GNET::GameClient::GetInstance()))
	{
		ASSERT(m_pNetMan);
		a_LogOutput(1, "CECGameSession::Open, Failed to create net manager");
	}

	m_pNetMan->SetZoneID(0);
	m_pNetMan->Attach(IOCallBack);

	m_idLink			= (DWORD)(-1);
	m_iUserID			= 0;
	m_iCharID			= 0;
	m_bConnected		= false;
	m_pS2CCmdLog		= NULL;
	m_pS2CCmdBinLog		= NULL;
	m_hConnectThread	= NULL;
	m_bKickUser			= true;
	m_tokenType		=	0;
	m_iiAccountTime		= 0;
	m_iiFreeGameTime	= 0;
	m_iFreeGameEnd		= 0;
	m_dwLastAccTime		= 0;
	m_dwNewbieTime		= 0;
	m_dwCreatedTime		= 0;
	ResetBreakLinkFlag();
	ResetLinkBroken();

	m_pfnHelpStatesCallBack = NULL;

	memset(&m_ot, 0, sizeof (m_ot));
	memset(&m_SevAttr, 0, sizeof (m_SevAttr));

	InitBoradcastMap();
	
	InitializeCriticalSection(&m_csPingSection);

	m_stubbornFactionInfoSender = new CECStubbornFactionInfoSender;
	g_pGame->GetFactionMan()->RegisterObserver(m_stubbornFactionInfoSender);
	m_aStubbornNetSenders.Add(m_stubbornFactionInfoSender);
}

void CECGameSession::resetPing()
{
	for (int i = 0; i < m_Ping.GetSize(); i++)
		delete m_Ping[i];
	m_Ping.RemoveAll();
}

CECGameSession::~CECGameSession()
{
	g_pGame->GetFactionMan()->UnregisterObserver(m_stubbornFactionInfoSender);
	delete m_stubbornFactionInfoSender;
	m_stubbornFactionInfoSender = NULL;
	m_aStubbornNetSenders.RemoveAll(false);

	if( m_pNetMan )
	{
		m_pNetMan->Detach();
		m_pNetMan = NULL;
	}

	for (int i = 0; i < m_Ping.GetSize(); i++)
		delete m_Ping[i];

	m_ErrorMsgs.Release();
	DeleteCriticalSection(&m_csPingSection);
}

//	Open session
bool CECGameSession::Open()
{
	//	Connect server
	Connect();

	//	Load server error strings form file
	m_ErrorMsgs.Init("Configs\\server_error.txt", true);

	// create cmd log
	if(m_pS2CCmdLog) A3DRELEASE(m_pS2CCmdLog);
	m_pS2CCmdLog = new ALog;

	//determine log file's path
	AString str = "s2ccmd.log";
	OFSTRUCT st;
	if (OpenFile("Logs\\s2ccmd.log",&st,OF_EXIST) != HFILE_ERROR)
	{
		//�ļ�����
		HFILE file = OpenFile("Logs\\s2ccmd.log",&st,OF_READ|OF_SHARE_EXCLUSIVE);
		if(file!= HFILE_ERROR)
		{
			CloseHandle((HANDLE)file);
		}
		else
		{
			str.Format("s2ccmd%d.log",time(NULL));			
		}
	}

	if (!m_pS2CCmdLog->Init(str, "Server to Client command log!"))
	{
		A3DRELEASE(m_pS2CCmdLog)
		return true;
	}

	// create command bin log
	if(m_pS2CCmdBinLog) 
	{
		m_pS2CCmdBinLog->Close();
		delete m_pS2CCmdBinLog;
		m_pS2CCmdBinLog = NULL;
	}
	m_pS2CCmdBinLog = new AFile;

	//determine log file's path
	str = "logs\\s2ccmdlog.bin";
	if (OpenFile(str,&st,OF_EXIST) != HFILE_ERROR)
	{
		//�ļ�����
		HFILE file = OpenFile(str,&st,OF_READ|OF_SHARE_EXCLUSIVE);
		if(file!= HFILE_ERROR)
		{
			CloseHandle((HANDLE)file);
		}
		else
		{
			str.Format("logs\\s2ccmdlog%d.bin",time(NULL));
		}
	}

	if (!m_pS2CCmdBinLog->Open(str, AFILE_CREATENEW | AFILE_BINARY | AFILE_NOHEAD))
	{
		m_pS2CCmdBinLog->Close();
		delete m_pS2CCmdBinLog;
		m_pS2CCmdBinLog = NULL;
		return true;
	}

	m_nS2CCmdSize = 0;
	return true;
}

//	Close sesssion
void CECGameSession::Close()
{
	int i(0);
	for (i = 0; i < m_aStubbornNetSenders.GetSize(); ++ i){
		m_aStubbornNetSenders[i]->Reset();
	}

	m_pfnHelpStatesCallBack = NULL;

	CloseConnectThread();

	//	Close net manager
	if (m_pNetMan)
	{
		m_pNetMan->Disconnect();
		m_idLink = -1;
		PollIO::WakeUp();
	}

	if (m_pS2CCmdBinLog)
	{
		m_pS2CCmdBinLog->Close();
		delete m_pS2CCmdBinLog;
		m_pS2CCmdBinLog = NULL;
	}

	if (m_pS2CCmdLog)
	{
		m_pS2CCmdLog->Release();
		delete m_pS2CCmdLog;
		m_pS2CCmdLog = NULL;
	}

	//	Clear command cache
	m_CmdCache.RemoveAllCmds();

	EnterCriticalSection(&g_csSession);

	//	Release all unprocessed protocols
	for (int i=0; i < m_aNewProtocols.GetSize(); i++)
	{
		GNET::Protocol* p = m_aNewProtocols[i];
		p->Destroy();
	}

	m_aNewProtocols.RemoveAll();

	LeaveCriticalSection(&g_csSession);

	//	Release all processed protocols
	ClearOldProtocols();

	//	Release chat protocol array
	for (int i=0; i < m_aPendingProtocols.GetSize(); i++)
	{
		GNET::Protocol* p = m_aPendingProtocols[i];
		p->Destroy();
	}

	m_aPendingProtocols.RemoveAll();

	::EnterCriticalSection(&g_csDefence);
	for (size_t j = 0; j < g_DefenceCodeVec.size(); j++)
		g_DefenceCodeVec[j]->Destroy();
	g_DefenceCodeVec.clear();
	::LeaveCriticalSection(&g_csDefence);

	m_bConnected		= false;
	m_iiAccountTime		= 0;
	m_iiFreeGameTime	= 0;
	m_iFreeGameEnd		= 0;
}

DWORD WINAPI CECGameSession::ConnectThread(LPVOID pArg)
{
	CECGameSession* pGameSession = (CECGameSession*)pArg;

	const CECGame::GAMEINIT& gi = g_pGame->GetGameInit();
	if (gi.szIP[0] && gi.iPort)
		pGameSession->GetNetManager()->Connect(gi.szIP, (WORD)gi.iPort);
	else
		pGameSession->GetNetManager()->Connect(NULL, 0);

	//	Start overtime check
	DWORD dwTotal, dwCount;
	if( !pGameSession->GetOvertimeCnt(dwTotal, dwCount) )
	{
		// we only activate the overtime check when the timer has not been activated
		pGameSession->DoOvertimeCheck(true, CECGameSession::OT_CHALLENGE, 40000);
	}

	//	Wait until connnect is established
	while (!pGameSession->IsConnected() && !l_bExitConnect)
	{
		Sleep(100);
	}

	return 0;
}

//	Connect server
bool CECGameSession::Connect()
{
	if (!m_pNetMan)
	{
		ASSERT(m_pNetMan);
		return false;
	}

	CloseConnectThread();
	
	// we use a thread to connect with the server, this can avoid the main thread blocked by 
	// firewalls when calling socket's connect
	DWORD dwThreadID;
	l_bExitConnect = false;
	m_hConnectThread = CreateThread(NULL, 0, ConnectThread, this, 0, &dwThreadID);

	return true;
}

void CECGameSession::CloseConnectThread()
{	
	if (m_hConnectThread)
	{
		l_bExitConnect = true;
		WaitForSingleObject(m_hConnectThread, INFINITE);
		m_ot.bCheck = false;
		CloseHandle(m_hConnectThread);
		m_hConnectThread = NULL;
	}
}

//	Send net data
bool CECGameSession::SendNetData(const GNET::Protocol& p, bool bUrg)
{
	StackChecker::ACTrace(8);

	if (!m_pNetMan || m_idLink == (DWORD)(-1))
	{
		ASSERT(m_pNetMan && m_idLink != (DWORD)(-1));
		return false;
	}
	
	if (CECProtocolDebug::Instance().IsClientProtocolIgnored(&p)){
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, _AL("CLIENT - %s ignored"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(p, false)));
		return true;
	}

#ifdef LOG_PROTOCOL
	if( !g_pGame->GetRTDebug()->IsProtocolHide(p, false) )
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, _AL("CLIENT - %s"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(p, false)));
#endif

	return m_pNetMan->Send(m_idLink, p, bUrg);
}

//	Update
bool CECGameSession::Update(DWORD dwDeltaTime)
{
	//	Check overtime
	if (m_ot.bCheck)
	{
		if ((m_ot.dwTimeCnt += dwDeltaTime) >= m_ot.dwTime)
		{
			m_ot.bCheck = false;
			OnOvertimeHappen();
		}
	}

	m_CmdCache.Tick(dwDeltaTime);

	if (m_bLinkBroken){
		OnLinkBroken();
	}

	//	Update left account game time
	if (m_iFreeGameEnd == -1)
	{
		// ��ǰ�����״̬�����üƷ�
	}
	else if (m_iiFreeGameTime)
	{
		DWORD dwCurTime = a_GetTime();
		DWORD dwDeltaTime = dwCurTime - m_dwLastAccTime;
		if ((m_iiFreeGameTime -= dwDeltaTime) < 0)
			m_iiFreeGameTime = 0;
		
		m_dwLastAccTime = dwCurTime;
	}
	else if (m_iiAccountTime)
	{
		DWORD dwCurTime = a_GetTime();
		DWORD dwDeltaTime = dwCurTime - m_dwLastAccTime;
		if ((m_iiAccountTime -= dwDeltaTime) < 0)
			m_iiAccountTime = 0;

		m_dwLastAccTime = dwCurTime;
	}

	if (m_bToBreakLink){
		BreakLink();
	}
	
	for (int i = 0; i < m_aStubbornNetSenders.GetSize(); ++ i){
		CECStubbornNetSender *p = m_aStubbornNetSenders[i];
		if (!p->IsFinished()){
			p->Tick();
		}
	}

	return true;
}

//	Net pulse
void CECGameSession::NetPulse()
{
	GNET::Timer::Update();
}

//	Start / End overtime
void CECGameSession::DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime)
{
	if (bStart)
	{
		m_ot.bCheck		= bStart;
		m_ot.iCheckID	= iCheckID;
		m_ot.dwTime		= dwTime;
		m_ot.dwTimeCnt	= 0;
	}
	else
	{
		ASSERT(m_ot.iCheckID == iCheckID);
		m_ot.bCheck	= false;
	}
	
	a_LogOutput(1, "CECGameSession::DoOvertimeCheck(bStart=%s, iCheckID=%d, dwTime=%u)", bStart?"true":"false", iCheckID, dwTime);
}

//	On overtime happens
void CECGameSession::OnOvertimeHappen()
{
	a_LogOutput(1, "CECGameSession::OnOvertimeHappen(iCheckID=%d, bCheck=%s, dwTime=%u, dwTimeCnt=%u)",	m_ot.iCheckID, m_ot.bCheck?"true":"false", m_ot.dwTime, m_ot.dwTimeCnt);

	switch (m_ot.iCheckID)
	{
	case OT_CHALLENGE:
		SetBreakLinkFlag(LBR_CHALLENGE_OVERTIME);
		break;

	case OT_ENTERGAME:
		SetBreakLinkFlag(LBR_ENTERWORLD_OVERTIME);
		break;
	}
}

//	Get overtime counter
bool CECGameSession::GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt)
{
	if (m_ot.bCheck)
	{
		dwTotalTime = m_ot.dwTime;
		dwCnt = m_ot.dwTimeCnt;
		return true;
	}
	else
		return false;
}

//	Freeze host's inventory item
void CECGameSession::FreezeHostItem(int iIvtr, int iIndex, bool bFreeze)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost)
		pHost->FreezeItem(iIvtr, iIndex, bFreeze, true);
}

//	Add one player's id to a buffer in order to get his name later
void CECGameSession::AddChatPlayerID(int id)
{
	if (!g_pGame->GetGameRun()->GetPlayerName(id, false))
		m_aPendingPlayers.Add(id);
}

void CECGameSession::server_Ping(int idServer, in_addr * addr, int nPort)
{
	EnterCriticalSection(&m_csPingSection);
	if( m_Ping.GetSize() > idServer )
	{
		PINGTIME * pt = m_Ping[idServer];
		if( pt->idLink == (DWORD)-1 || CECTimeSafeChecker::ElapsedTimeFor(pt->dwLastPingTime) > 600000 )
		{
			pt->idLink = GetNetManager()->Ping(addr, nPort);
			pt->dwCreateTime = 0;
			pt->dwLastPingTime = GetTickCount();
			pt->dwTime = 99999999;
			pt->dwStatus = 0;
			g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("NET - Ping(idServer=%d,sid=%u)"), idServer, pt->idLink);
		}
		LeaveCriticalSection(&m_csPingSection);
		return;
	}
	int i;
	for( i = m_Ping.GetSize(); i < idServer; i++ )
	{
		PINGTIME* pt = new PINGTIME;
		pt->idLink = (DWORD)-1;
		pt->dwTime = 99999999;
		pt->dwLastPingTime = 0;
		pt->dwCreateTime = 0;
		pt->dwStatus = 0;
		pt->cExpRate = 10;
		m_Ping.Add(pt);
	}
	PINGTIME* pt = new PINGTIME;
	pt->idLink = GetNetManager()->Ping(addr, nPort);
	pt->dwTime = 99999999;
	pt->dwLastPingTime = GetTickCount();
	pt->dwCreateTime = 0;
	pt->dwStatus = 0;
	pt->cExpRate = 10;
	m_Ping.Add(pt);
	g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("NET - Ping(idServer=%d,sid=%u)"), idServer, pt->idLink);
	LeaveCriticalSection(&m_csPingSection);
}

int CECGameSession::IOCallBack(void* pData, unsigned int idLink, int iEvent)
{
	using namespace GNET;

	CECGameSession* pSession = g_pGame->GetGameSession();
	if (!pSession)
		return 0;

	GNET::GameClient* pNetMan = pSession->GetNetManager();
	if (!pNetMan)
	{
		OutputDebugString(_AL("CECGameSession::IOCallBack, callback when pNetMan=NULL !"));
		return 0;
	}

	switch (iEvent)
	{
	case EVENT_ADDSESSION:

		g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("NET - EVENT_ADDSESSION(sid=%u)"), idLink);
		pSession->SetLinkID(idLink);
		pSession->m_bConnected	= true;
		pSession->ResetLinkBroken();
		return 0;

	case EVENT_DELSESSION:
	{
		//	If we are in game state and were disconnected passively, 
		//	notify player and exit game
		if (pSession->m_bConnected)
		{
			//	�����Ͽ�����ʱ��m_bConnected �Ѿ���ǰ����Ϊ false
			pSession->LinkBroken(CECGameSession::LBR_PASSIVE_ONLINE);
			pSession->m_bConnected = false;
		}	
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("NET - EVENT_DELSESSION(sid=%u)"), idLink);
		a_LogOutput(1, "CECGameSession::IOCallBack, EVENT_DELSESSION, error code = %s", pSession->m_pNetMan->GetLastError());
		return 0;
	}
	case EVENT_ABORTSESSION:
	{	
		// connect to server failed, so pop up a message window
		// make overtime check fire.
		pSession->DoOvertimeCheck(true, CECGameSession::OT_CHALLENGE, 0);
		
		pSession->m_bConnected = false;
		g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("NET - EVENT_ABORTSESSION(sid=%u)"), idLink);
		return 0;
	}
	case EVENT_PINGRETURN:
	{
		EnterCriticalSection(&pSession->m_csPingSection);
		int i;
		for( i = 0; i < pSession->m_Ping.GetSize(); i++ )
			if( pSession->m_Ping[i]->idLink == idLink )
			{
				ServerStatus* status = (ServerStatus*)pData;
				double newping = (double)status->ping;
				if( status->attr.load >= 200 )
					status->attr.load = 199;
				if( status->attr.lambda != 0 )
					newping = status->ping / ( 1 - exp( - status->attr.lambda * ( 1 - status->attr.load / 200.0) ) );
				if( status->attr.anything != 0 )
					newping *= 0.4 + status->attr.anything * 0.1;
				a_Clamp(newping, 5.0 , 3000.0);
				pSession->m_Ping[i]->dwTime = (DWORD)newping;
				pSession->m_Ping[i]->dwStatus = status->attr._attr;
				pSession->m_Ping[i]->dwCreateTime = status->createtime;
				pSession->m_Ping[i]->cExpRate = status->exp_rate;
				break;
			}
		LeaveCriticalSection(&pSession->m_csPingSection);

		return 0;
	}
	case EVENT_DISCONNECT:
	{
		//	��¼���ӷ������ɹ�����������Ͽ����ӣ������򱻶��������
		if (pSession->m_bConnected)
		{
			if (pSession->GetLinkID() == idLink)
			{
				//	�߼��߳������Ͽ�����ʱ��pSession->GetLinkID()��������Чֵ
				//	�����Ǳ����Ͽ�
				pSession->LinkBroken(CECGameSession::LBR_PASSIVE_BEFOREONLINE);
			}
			pSession->m_bConnected = false;
		}
		g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("NET - EVENT_DISCONNECT(sid=%u)"), idLink);
		a_LogOutput(1, "CECGameSession::IOCallBack, EVENT_DISCONNECT, error code = %s", pSession->m_pNetMan->GetLastError());
		return 0;
	}

	}

	GNET::Protocol* pProtocol = (GNET::Protocol*)pData;
	pSession->AddNewProtocol(pProtocol);

	return 0;
}

//	Add a protocol to m_NewPrtcList and prepare to be processed
int CECGameSession::AddNewProtocol(GNET::Protocol* pProtocol)
{
	EnterCriticalSection(&g_csSession);
	int iSize = m_aNewProtocols.Add(pProtocol);
	LeaveCriticalSection(&g_csSession);
	return iSize;
}

void CECGameSession::SetBreakLinkFlag(enumLinkBrokenReason reason){
	if (m_bToBreakLink){
		return;
	}
	m_bToBreakLink = true;
	m_breakLinkReason = reason;
}

void CECGameSession::BreakLink(){	
	Close();
	LinkBroken(m_breakLinkReason);
	ResetBreakLinkFlag();
}

void CECGameSession::ResetBreakLinkFlag(){
	m_bToBreakLink = false;
	m_breakLinkReason = LBR_NONE;
}

//	When connection was broken, this function is called
void CECGameSession::LinkBroken(enumLinkBrokenReason reason){
	m_bLinkBroken = true;
	m_linkBrokenReason = reason;
}

static bool MayReconnectForLinkBrokenReason(CECGameSession::enumLinkBrokenReason reason){
	bool result(false);
	switch (reason){
	case CECGameSession::LBR_ROLE_RENAME:		//	��ɫ����������ʾ����
	case CECGameSession::LBR_PASSIVE_ONLINE:	//	���ߺ�����ԭ���Э��ԭ�������ʾ����
	case CECGameSession::LBR_DEBUG:				//	��������
		result = true;
		break;
	}
	return result;
}

void CECGameSession::OnLinkBroken(){
	a_LogOutput(1, "CECGameSession::OnLinkBroken(%s)", GetLinkBrokenReasonDescription(m_linkBrokenReason));
	if (CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager()){
		CECLoginQueue::Instance().ClearState();
		if (CECReconnect::Instance().CanRequestReconnect() &&
			MayReconnectForLinkBrokenReason(m_linkBrokenReason)){
			pUIMan->ShowReconnectMsg(FIXMSG_REMIND_RECONNECT);
		}else{
			int iMsg = (LBR_CHALLENGE_OVERTIME == m_linkBrokenReason) ? FIXMSG_FAILEDTOCONN : FIXMSG_DISCONNECTED;
			pUIMan->ShowLinkBrokenMsg(iMsg);
		}
	}
	ResetLinkBroken();
}

void CECGameSession::ResetLinkBroken(){	
	m_bLinkBroken = false;
	m_linkBrokenReason = LBR_NONE;
}

//	Process protocols in m_NewPrtcList
bool CECGameSession::ProcessNewProtocols()
{
	using namespace GNET;

	int i;
	DWORD dwRet;

	EnterCriticalSection(&g_csSession);

	for (int i=0; i < m_aNewProtocols.GetSize(); i++)
		m_aTempProtocols.Add(m_aNewProtocols[i]);

	m_aNewProtocols.RemoveAll(false);

	LeaveCriticalSection(&g_csSession);

	for (int i=0; i < m_aTempProtocols.GetSize(); i++)
	{
		bool bAddToOld = true;
		GNET::Protocol* pProtocol = m_aTempProtocols[i];
		
		if (!pProtocol)
		{
			a_LogOutput(1, "CECGameSession::ProcessNewProtocols(), Empty Protocol!");
			continue;
		}

		if (FilterProtocols(pProtocol))
		{
			m_aOldProtocols.Add(pProtocol);
			continue;
		}

		switch (pProtocol->GetType())
		{
		case PROTOCOL_CHALLENGE:				OnPrtcChallenge(pProtocol);				break;
		case PROTOCOL_KEYEXCHANGE:				OnPrtcKeyExchange(pProtocol);			break;
		case PROTOCOL_ONLINEANNOUNCE:			OnPrtcOnlineAnnounce(pProtocol);		break;
		case PROTOCOL_WAITQUEUESTATENOTIFY:		OnPrtcWaitQueueNotify(pProtocol);		break;
		case PROTOCOL_CANCELWAITQUEUE_RE:		OnPrtcCancelWaitQueueRe(pProtocol);		break;
		case PROTOCOL_ROLELIST_RE:				OnPrtcRoleListRe(pProtocol);			break;
		case RPC_PLAYERPOSITIONRESETRQST:
			OnPrtcResetPosition(pProtocol);
			bAddToOld = false;
			break;
		case PROTOCOL_SELECTROLE_RE:			OnPrtcSelectRoleRe(pProtocol);			break;
		case PROTOCOL_CREATEROLE_RE:			OnPrtcCreateRoleRe(pProtocol);			break;
		case PROTOCOL_DELETEROLE_RE:			OnPrtcDeleteRoleRe(pProtocol);			break;
		case PROTOCOL_UNDODELETEROLE_RE:		OnPtrcUndoDeleteRoleRe(pProtocol);		break;

		case PROTOCOL_GAMEDATASEND:	
			
			ProcessGameData(((GamedataSend*)pProtocol)->data);
			break;

		case PROTOCOL_CHATMESSAGE:				bAddToOld = OnPrtcChatMessage(pProtocol, false);break; //����ChatMessage���������ʽ
		case PROTOCOL_ERRORINFO:				OnPrtcErrorInfo(pProtocol);				break;
		case PROTOCOL_PLAYERLOGOUT:				OnPrtcPlayerLogout(pProtocol);			break;
		case PROTOCOL_PRIVATECHAT:				OnPrtcPrivateChat(pProtocol);			break;
		case PROTOCOL_PLAYERBASEINFO_RE:		OnPrtcPlayerBaseInfoRe(pProtocol);		break;
		case PROTOCOL_GETUICONFIG_RE:			OnPrtcGetConfigRe(pProtocol);			break;
		case PROTOCOL_SETUICONFIG_RE:			OnPrtcSetConfigRe(pProtocol);			break;
		case PROTOCOL_GETHELPSTATES_RE:			OnPrtcGetHelpStatesRe(pProtocol);		break;
		case PROTOCOL_SETHELPSTATES_RE:			OnPrtcSetHelpStatesRe(pProtocol);		break;
		case PROTOCOL_SETCUSTOMDATA_RE:			OnPrtcSetCustomDataRe(pProtocol);		break;
		case PROTOCOL_GETCUSTOMDATA_RE:			OnPrtcGetCustomDataRe(pProtocol);		break;
		case PROTOCOL_GETPLAYERBRIEFINFO_RE:	OnPrtcGetPlayerBriefInfoRe(pProtocol);	break;
		case PROTOCOL_GETPLAYERIDBYNAME_RE:		OnPrtcGetPlayerIDByNameRe(pProtocol);	break;
		case PROTOCOL_ROLESTATUSANNOUNCE:		OnPrtcRoleStatusAnnounce(pProtocol);	break;
		case PROTOCOL_ANNOUNCEFORBIDINFO:		OnPrtcAnnounceForbidInfo(pProtocol);	break;
		case PROTOCOL_UPDATEREMAINTIME:			OnPrtcUpdateRemainTime(pProtocol);		break;
		case PROTOCOL_WORLDCHAT:				bAddToOld = OnPrtcWorldChat(pProtocol, false);	break;
		case PROTOCOL_REFGETREFERENCECODE_RE:	OnPrtcRefGetReferenceCodeRe(pProtocol);	break;
		case PROTOCOL_REFLISTREFERRALS_RE:		OnPrtcRefListReferralsRe(pProtocol);	break;
		case PROTOCOL_REFWITHDRAWBONUS_RE:		OnPrtcRefWithdrawBonusRe(pProtocol);	break;
		case PROTOCOL_GETREWARDLIST_RE:			OnPrtcGetRewardListRe(pProtocol);		break;
		case PROTOCOL_EXCHANGECONSUMEPOINTS_RE: OnPrtcExchangeConsumePointsRe(pProtocol); break;
		case PROTOCOL_REWARDMATURENOTICE:		OnPrtcRewardMatrueNotice(pProtocol);	break;
		case PROTOCOL_ACCOUNTLOGINRECORD:	OnPrtcAccountLoginRecord(pProtocol);	break;
		
		//	Trade protocols
		case PROTOCOL_TRADESTART_RE:
		case PROTOCOL_TRADEMOVEOBJ_RE:
		case PROTOCOL_TRADEADDGOODS_RE:
		case PROTOCOL_TRADEREMOVEGOODS_RE:
		case PROTOCOL_TRADEDISCARD_RE:
		case PROTOCOL_TRADESUBMIT_RE:
		case PROTOCOL_TRADECONFIRM_RE:
		case PROTOCOL_TRADEEND:

			OnTradeProtocols(pProtocol);
			break;

		//  UserCoupon protocols
		case PROTOCOL_USERCOUPON_RE:
		case PROTOCOL_USERCOUPONEXCHANGE_RE:

			OnUserCouponProtocols(pProtocol);
			break;

		case PROTOCOL_USERADDCASH_RE:
			{
				CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUIMan)
					pGameUIMan->UserCashAction(pProtocol);
			}
			break;
			
			//  web Trade protocols
		case PROTOCOL_WEBTRADELIST_RE:
		case PROTOCOL_WEBTRADEATTENDLIST_RE:
		case PROTOCOL_WEBTRADEGETITEM_RE:
		case PROTOCOL_WEBTRADEGETDETAIL_RE:
		case PROTOCOL_WEBTRADEPREPOST_RE:
		case PROTOCOL_WEBTRADEPRECANCELPOST_RE:
		case PROTOCOL_WEBTRADEUPDATE_RE:
			
			OnWebTradeProtocols(pProtocol);
			break;

		// Sys Auction Protocols
		case PROTOCOL_SYSAUCTIONACCOUNT_RE:
		case PROTOCOL_SYSAUCTIONBID_RE:
		case PROTOCOL_SYSAUCTIONGETITEM_RE:
		case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
		case PROTOCOL_SYSAUCTIONLIST_RE:

			OnSysAuctionProtocols(pProtocol);
			break;

		case PROTOCOL_CREATEFACTIONFORTRESS_RE:
		case PROTOCOL_FACTIONFORTRESSLIST_RE:
		case PROTOCOL_FACTIONFORTRESSCHALLENGE_RE:
		case PROTOCOL_FACTIONFORTRESSBATTLELIST_RE:
		case PROTOCOL_FACTIONFORTRESSGET_RE:
			OnFactionFortressProtocols(pProtocol);
			break;
			
		case PROTOCOL_STOCKACCOUNT_RE:
		case PROTOCOL_STOCKBILL_RE:
		case PROTOCOL_STOCKCOMMISSION_RE:
		case PROTOCOL_STOCKTRANSACTION_RE:
		case PROTOCOL_STOCKCANCEL_RE:

			OnStockProtocols(pProtocol);
			break;

		//	GM protocols
		case PROTOCOL_GMONLINENUM_RE:		OnPrtcGMOnlineNumberRe(pProtocol);		break;	
		case PROTOCOL_GMLISTONLINEUSER_RE:	OnPrtcGMListOnlineUserRe(pProtocol);	break;
		case PROTOCOL_GMKICKOUTUSER_RE:		OnPrtcGMKickOutUserRe(pProtocol);		break;	
		case PROTOCOL_GMSHUTUP_RE:			OnPrtcGMShutupUserRe(pProtocol);		break;
		case PROTOCOL_GMRESTARTSERVER_RE:	OnPrtcGMRestartServerRe(pProtocol);		break;
		case PROTOCOL_GMRESTARTSERVER:		OnPrtcGMRestartServer(pProtocol);		break;
		case PROTOCOL_GMKICKOUTROLE_RE:		OnPrtcGMKickOutRoleRe(pProtocol);		break;
		case PROTOCOL_GMSHUTUPROLE_RE:		OnPrtcGMShutupRoleRe(pProtocol);		break;
		case PROTOCOL_GMFORBIDROLE_RE:		OnPrtcGMForbidRoleRe(pProtocol);		break;
		case PROTOCOL_GMTOGGLECHAT_RE:		OnPrtcGMToggleChatRe(pProtocol);		break;
		case PROTOCOL_GMSETTIMELESSAUTOLOCK_RE:OnPrtcGMPermanentLockRe(pProtocol); break;

		//	Friend protocols
		case PROTOCOL_ADDFRIEND_RE:
		case PROTOCOL_GETFRIENDS_RE:
		case PROTOCOL_SETGROUPNAME_RE:
		case PROTOCOL_SETFRIENDGROUP_RE:
		case PROTOCOL_DELFRIEND_RE:
		case PROTOCOL_FRIENDSTATUS:
		case PROTOCOL_GETSAVEDMSG_RE:
		case PROTOCOL_FRIENDEXTLIST:
		case PROTOCOL_SENDAUMAIL_RE:
		case PROTOCOL_ADDFRIENDREMARKS_RE:

			OnFriendProtocols(pProtocol);
			break;
			
		//	Faction protocols
		case PROTOCOL_FACTIONCHAT:

			if (g_pGame->GetFactionMan()->OnChat(static_cast<FactionChat*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);
				AddChatPlayerID(static_cast<FactionChat*>(pProtocol)->src_roleid);
				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONACCEPTJOIN_RE:

			if (dwRet = g_pGame->GetFactionMan()->OnAcceptJoin(static_cast<FactionAcceptJoin_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);

				if (dwRet & FO_OPERATOR_UNK)
					AddChatPlayerID(static_cast<FactionAcceptJoin_Re*>(pProtocol)->operater);

				if (dwRet & FO_OPERANT_UNK)
					AddChatPlayerID(static_cast<FactionAcceptJoin_Re*>(pProtocol)->newmember);

				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONRESIGN_RE:

			if (g_pGame->GetFactionMan()->OnResign(static_cast<FactionResign_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);
				AddChatPlayerID(static_cast<FactionResign_Re*>(pProtocol)->resigned_role);
				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONRENAME_RE:

			if (dwRet = g_pGame->GetFactionMan()->OnRename(static_cast<FactionRename_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);

				if (dwRet & FO_OPERATOR_UNK)
					AddChatPlayerID(static_cast<FactionRename_Re*>(pProtocol)->operater);

				if (dwRet & FO_OPERANT_UNK)
					AddChatPlayerID(static_cast<FactionRename_Re*>(pProtocol)->renamed_roleid);

				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONMASTERRESIGN_RE:

			if (g_pGame->GetFactionMan()->OnMasterResign(static_cast<FactionMasterResign_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);
				AddChatPlayerID(static_cast<FactionMasterResign_Re*>(pProtocol)->newmaster);
				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONLEAVE_RE:

			if (g_pGame->GetFactionMan()->OnLeave(static_cast<FactionLeave_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);
				AddChatPlayerID(static_cast<FactionLeave_Re*>(pProtocol)->leaved_role);
				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONEXPEL_RE:

			if (dwRet = g_pGame->GetFactionMan()->OnExpel(static_cast<FactionExpel_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);

				if (dwRet & FO_OPERATOR_UNK)
					AddChatPlayerID(static_cast<FactionExpel_Re*>(pProtocol)->operater);

				if (dwRet & FO_OPERANT_UNK)
					AddChatPlayerID(static_cast<FactionExpel_Re*>(pProtocol)->expelroleid);

				bAddToOld = false;
			}

			break;
			
		case PROTOCOL_FACTIONDELAYEXPELANNOUNCE:
			
			if (dwRet = g_pGame->GetFactionMan()->OnDelayExpel(static_cast<FactionDelayExpelAnnounce *>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);
				
				if (dwRet & FO_OPERATOR_UNK)
					AddChatPlayerID(static_cast<FactionDelayExpelAnnounce*>(pProtocol)->operater);
				
				if (dwRet & FO_OPERANT_UNK)
					AddChatPlayerID(static_cast<FactionDelayExpelAnnounce*>(pProtocol)->expelroleid);
				
				bAddToOld = false;
			}
			
			break;

		case PROTOCOL_FACTIONAPPOINT_RE:

			if (dwRet = g_pGame->GetFactionMan()->OnAppoint(static_cast<FactionAppoint_Re*>(pProtocol)))
			{
				m_aPendingProtocols.Add(pProtocol);

				if (dwRet & FO_OPERATOR_UNK)
					AddChatPlayerID(static_cast<FactionAppoint_Re*>(pProtocol)->operater);

				if (dwRet & FO_OPERANT_UNK)
					AddChatPlayerID(static_cast<FactionAppoint_Re*>(pProtocol)->dstroleid);

				bAddToOld = false;
			}

			break;

		case PROTOCOL_FACTIONOPREQUEST_RE:
		case PROTOCOL_FACTIONCREATE_RE:
		case PROTOCOL_FACTIONBROADCASTNOTICE_RE:
		case PROTOCOL_FACTIONUPGRADE_RE:
		case PROTOCOL_FACTIONDEGRADE_RE:
		case PROTOCOL_FACTIONLISTMEMBER_RE:
		case PROTOCOL_GETFACTIONBASEINFO_RE:
		case PROTOCOL_FACTIONDISMISS_RE:
		case PROTOCOL_GETPLAYERFACTIONINFO_RE:
		case PROTOCOL_FACTIONCHANGPROCLAIM_RE:
		case PROTOCOL_FACTIONRENAMEANNOUNCE:

			OnFactionProtocols(pProtocol);
			break;
		case PROTOCOL_PLAYERNAMEUPDATE: 
			
			OnPrtcPlayerNameUpdate(pProtocol);
			break;

		case PROTOCOL_PLAYERGIVEPRESENT_RE: 
		case PROTOCOL_PLAYERASKFORPRESENT_RE: 
			{
				CECGameUIMan*pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if( pGameUI)
				{
					CDlgGivingFor*pDlg = dynamic_cast<CDlgGivingFor*>(pGameUI->GetDialog("Win_GivingFor"));
					if( pDlg)
						pDlg->OnPrtcPresentRe(pProtocol);
				}
			}
			break;
			
		case PROTOCOL_FACTIONALLIANCEAPPLY_RE:
		case PROTOCOL_FACTIONALLIANCEREPLY_RE:
		case PROTOCOL_FACTIONHOSTILEAPPLY_RE:
		case PROTOCOL_FACTIONHOSTILEREPLY_RE:
		case PROTOCOL_FACTIONREMOVERELATIONAPPLY_RE:
		case PROTOCOL_FACTIONREMOVERELATIONREPLY_RE:
		case PROTOCOL_FACTIONLISTRELATION_RE:
		case PROTOCOL_FACTIONLISTONLINE_RE:
		case PROTOCOL_FACTIONRELATIONRECVAPPLY:
		case PROTOCOL_FACTIONRELATIONRECVREPLY:

			if (!OnFactionDiplomacyProtocols(pProtocol, true))
			{
				m_aPendingProtocols.Add(pProtocol);
				bAddToOld = false;
			}
			break;

		//	Chatroom protocols
		case PROTOCOL_CHATROOMCREATE_RE:
		case PROTOCOL_CHATROOMINVITE:
		case PROTOCOL_CHATROOMINVITE_RE:
		case PROTOCOL_CHATROOMJOIN_RE:
		case PROTOCOL_CHATROOMLEAVE:
		case PROTOCOL_CHATROOMEXPEL:
		case PROTOCOL_CHATROOMSPEAK:
		case PROTOCOL_CHATROOMLIST_RE:

			OnChatRoomProtocols(pProtocol);
			break;

		//	Mail protocols
		case PROTOCOL_ANNOUNCENEWMAIL:
		case PROTOCOL_GETMAILLIST_RE:
		case PROTOCOL_GETMAIL_RE:
		case PROTOCOL_GETMAILATTACHOBJ_RE:
		case PROTOCOL_DELETEMAIL_RE:
		case PROTOCOL_PRESERVEMAIL_RE:
		case PROTOCOL_PLAYERSENDMAIL_RE:

			OnMailProtocols(pProtocol);
			break;

		//	Vendue protocols
		case PROTOCOL_AUCTIONOPEN_RE:
		case PROTOCOL_AUCTIONBID_RE:
		case PROTOCOL_AUCTIONLIST_RE:
		case PROTOCOL_AUCTIONLISTUPDATE_RE:
		case PROTOCOL_AUCTIONCLOSE_RE:
		case PROTOCOL_AUCTIONGET_RE:
		case PROTOCOL_AUCTIONATTENDLIST_RE:
		case PROTOCOL_AUCTIONEXITBID_RE:
		case PROTOCOL_AUCTIONGETITEM_RE:

			OnVendueProtocols(pProtocol);
			break;

		//	Battle protocols
		case PROTOCOL_BATTLECHALLENGE_RE:
		case PROTOCOL_BATTLECHALLENGEMAP_RE:
		case PROTOCOL_BATTLEGETMAP_RE:
		case PROTOCOL_BATTLEENTER_RE:
		case PROTOCOL_BATTLESTATUS_RE:

			OnBattleProtocols(pProtocol);
			break;

		//	Account protocols
		case PROTOCOL_SELLPOINT_RE:
		case PROTOCOL_GETSELLLIST_RE:
		case PROTOCOL_FINDSELLPOINTINFO_RE:
		case PROTOCOL_SELLCANCEL_RE:
		case PROTOCOL_BUYPOINT_RE:
		case PROTOCOL_ANNOUNCESELLRESULT:
			
			OnAccountProtocols(pProtocol);
			break;

		//  King protocols
		case PROTOCOL_KECANDIDATEAPPLY_RE:
		case PROTOCOL_KEVOTING_RE:
		case PROTOCOL_KEGETSTATUS_RE:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				pGameUI->KingAction(pProtocol);
			}
			break;

		//	RPCs...
		case RPC_FACTIONINVITEJOIN:
			OnFactionProtocols(pProtocol);
			bAddToOld = false;
			break;

		case RPC_TRADESTARTRQST:

			OnTradeProtocols(pProtocol);
			bAddToOld = false;
			break;

		case RPC_ADDFRIENDRQST:

			OnFriendProtocols(pProtocol);
			bAddToOld = false;
			break;

		case PROTOCOL_KEEPALIVE:

			//	Do nothing ...
			break;

		case PROTOCOL_MATRIXCHALLENGE:

			OnPrtcMatrixChallenge(pProtocol);
			break;

		case PROTOCOL_CASHLOCK_RE:
		case PROTOCOL_CASHPASSWORDSET_RE:

			OnPrtcCashOpt(pProtocol);
			break;

		case PROTOCOL_ACREMOTECODE:

#ifdef USING_DEFENCE
			::EnterCriticalSection(&g_csDefence);
			g_DefenceCodeVec.push_back(static_cast<ACRemoteCode*>(pProtocol));
			::LeaveCriticalSection(&g_csDefence);
			bAddToOld = false;
#endif
			break;

		case PROTOCOL_ACQUESTION:

			OnPrtcACQuestion(pProtocol);
			break;

		case PROTOCOL_AUTOLOCKSET_RE:

			OnPrtcAutoLockSetRe(pProtocol);
			break;

		case PROTOCOL_GMGETPLAYERCONSUMEINFO_RE:
			OnPrtcGMGetConsumeInfoRe(pProtocol);
			break;

		case PROTOCOL_PLAYERRENAME_RE:
			OnPrtcPlayerRenameRe(pProtocol);
			break;

		case PROTOCOL_SSOGETTICKET_RE:
			{
				CECBaseUIMan* pBaseUI = g_pGame->GetGameRun()->GetUIManager()->GetBaseUIMan();
				if (pBaseUI)
					pBaseUI->OnSSOGetTicket_Re(dynamic_cast<SSOGetTicket_Re *>(pProtocol));
			}
			break;

		case PROTOCOL_COUNTRYBATTLEMOVE_RE:
		case PROTOCOL_COUNTRYBATTLESYNCPLAYERLOCATION:
		case PROTOCOL_COUNTRYBATTLEGETMAP_RE:
		case PROTOCOL_COUNTRYBATTLEGETSCORE_RE:
		case PROTOCOL_COUNTRYBATTLEGETCONFIG_RE:
		case PROTOCOL_COUNTRYBATTLEPREENTERNOTIFY:
		case PROTOCOL_COUNTRYBATTLEGETBATTLELIMIT_RE:
			{				
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CDlgCountryMap *pDlg = (CDlgCountryMap *)pGameUI->GetDialog("Win_CountryMap");
					pDlg->OnPrtcCountry(pProtocol);
				}
			}
			break;
		case PROTOCOL_COUNTRYBATTLERESULT:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CDlgCountryReward *pDlgCountryReward = (CDlgCountryReward*)pGameUI->GetDialog("Win_CountryReward");
					pDlgCountryReward->OnPrtcCountryResult(pProtocol);
				}
			}
			break;

		case PROTOCOL_COUNTRYBATTLESINGLEBATTLERESULT:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CountryBattleSingleBattleResult *p = dynamic_cast<CountryBattleSingleBattleResult *>(pProtocol);

					ACString strMsg;
					const char cChannel = GP_CHAT_SYSTEM;

					const DOMAIN2_INFO *pDomain = CECDomainCountryInfo::GetInstance()->Find(p->domain_id);
					if (!pDomain)
					{
						ASSERT(false);
						a_LogOutput(1, "CECGameSession::ProcessNewProtocols, Invalid domain id=%d", p->domain_id);
					}
					else
					{
						pGameUI->AddChatMessage(strMsg.Format(pGameUI->GetStringFromTable(9990), pDomain->name), cChannel);
					}

					pGameUI->AddChatMessage(strMsg.Format(pGameUI->GetStringFromTable(9991), p->single_battle_total_score), cChannel);
					pGameUI->AddChatMessage(strMsg.Format(pGameUI->GetStringFromTable(9992), p->player_rank), cChannel);
					pGameUI->AddChatMessage(strMsg.Format(pGameUI->GetStringFromTable(9993), p->player_single_battle_score), cChannel);
				}
			}
			break;

		case PROTOCOL_COUNTRYBATTLEKINGASSIGNASSAULT_RE:
		case PROTOCOL_COUNTRYBATTLEGETKINGCOMMANDPOINT_RE:
			OnPrtcKingCommandPoint(pProtocol);
			break;
			
		case PROTOCOL_QPADDCASH_RE:
		case PROTOCOL_QPGETACTIVATEDSERVICES_RE:
		case PROTOCOL_QPANNOUNCEDISCOUNT:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CDlgQuickPay* pDlg = (CDlgQuickPay*)pGameUI->GetDialog("Win_QuickPay");
					pDlg->OnPrtcQuickPay(pProtocol);
				}
			}
			break;
		case PROTOCOL_PLAYERPROFILEGETPROFILEDATA_RE:
		case PROTOCOL_PLAYERPROFILEGETMATCHRESULT_RE:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CDlgMatchInfoHint* pDlg = (CDlgMatchInfoHint*)pGameUI->GetDialog("Win_MatchHint");
					pDlg->OnServerNotify(pProtocol);
				}
			}
			break;

		case PROTOCOL_CHANGEDS_RE:
			CECCrossServer::Instance().OnPrtcChangeDS_Re(pProtocol);
			break;
		case PROTOCOL_PLAYERCHANGEDS_RE:
			CECCrossServer::Instance().OnPrtcPlayerChangeDS_Re(pProtocol);
			break;
		
		case PROTOCOL_PSHOPCREATE_RE:
		case PROTOCOL_PSHOPBUY_RE:
		case PROTOCOL_PSHOPSELL_RE:
		case PROTOCOL_PSHOPCANCELGOODS_RE:
		case PROTOCOL_PSHOPPLAYERBUY_RE:
		case PROTOCOL_PSHOPPLAYERSELL_RE:
		case PROTOCOL_PSHOPSETTYPE_RE:
		case PROTOCOL_PSHOPACTIVE_RE:
		case PROTOCOL_PSHOPMANAGEFUND_RE:
		case PROTOCOL_PSHOPDRAWITEM_RE:
		case PROTOCOL_PSHOPCLEARGOODS_RE:
		case PROTOCOL_PSHOPSELFGET_RE:
		case PROTOCOL_PSHOPPLAYERGET_RE:
		case PROTOCOL_PSHOPLIST_RE:
		case PROTOCOL_PSHOPLISTITEM_RE:
			{
				COfflineShopCtrl* pCtrl = g_pGame->GetGameRun()->GetHostPlayer()->GetOfflineShopCtrl();
				if(pCtrl)
					pCtrl->OnServerNotify(pProtocol->GetType(),pProtocol);
			}
			break;
		case PROTOCOL_UNIQUEDATAMODIFYBROADCAST:
			{
				g_pGame->GetGameRun()->OnUniqueDataBroadcast(pProtocol);
			}
			break;
		case PROTOCOL_AUTOTEAMPLAYERLEAVE:
			{
				CECAutoTeam* pAutoTeam = g_pGame->GetGameRun()->GetHostPlayer()->GetAutoTeam();
				if( pAutoTeam )
					pAutoTeam->OnPrtcPlayerLeave(pProtocol);
			}
			break;
		case PROTOCOL_AUTOTEAMSETGOAL_RE:
			{
				CECAutoTeam* pAutoTeam = g_pGame->GetGameRun()->GetHostPlayer()->GetAutoTeam();
				if( pAutoTeam )
					pAutoTeam->OnPrtcAutoTeamSetGoalRe(pProtocol);
			}
			break;

		case PROTOCOL_TANKBATTLEPLAYERAPPLY_RE:
		case PROTOCOL_TANKBATTLEPLAYERGETRANK_RE:
			{
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pGameUI)
				{
					CDlgChariotRankList* pDlg = dynamic_cast<CDlgChariotRankList*>(pGameUI->GetDialog("Win_battlerank"));
					if(pDlg)
						pDlg->OnTankBattleNotify(pProtocol);
				}
			}
			break;
		case PROTOCOL_GETCNETSERVERCONFIG_RE:
			break;
		case PROTOCOL_PLAYERACCUSE_RE:
			if( !OnReportPluginMessage(pProtocol) )
			{
				AddChatPlayerID(static_cast<PlayerAccuse_Re*>(pProtocol)->dst_roleid);
				m_aPendingProtocols.Add(pProtocol);
				bAddToOld = false;
			}
			break;
#ifdef RESOURCE_BATTLE
		case PROTOCOL_FACTIONRESOURCEBATTLEPLAYERQUERYRESULT:
			if (!OnPrtcFactionPVPResult(pProtocol, true)){
				m_aPendingProtocols.Add(pProtocol);
				bAddToOld = false;
			}
			break;
		case PROTOCOL_FACTIONRESOURCEBATTLEGETMAP_RE:
			CECFactionPVPModel::Instance().OnPrtcFactionPVPGetMapRe((const FactionResourceBattleGetMap_Re *)pProtocol);
			break;
		case PROTOCOL_FACTIONRESOURCEBATTLEGETRECORD_RE:
			CECFactionPVPModel::Instance().OnPrtcFactionPVPGetRecordRe((const FactionResourceBattleGetRecord_Re *)pProtocol);
			break;
		case PROTOCOL_FACTIONRESOURCEBATTLENOTIFYPLAYEREVENT:
			CECFactionPVPModel::Instance().OnPrtcFactionPVPNotifyPlayerEvent((const FactionResourceBattleNotifyPlayerEvent *)pProtocol);
			break;
#endif
		default:
			g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("SERVER - Unknown %s"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)));
			break;
		}

		//	Add this protocol to old list
		if (bAddToOld)
			m_aOldProtocols.Add(pProtocol);
	}

	m_aTempProtocols.RemoveAll(false);

	//	If there are some players who said something and we still don't 
	//	know their names, try to get that from server
	if (m_aPendingPlayers.GetSize())
	{
		CacheGetPlayerBriefInfo(m_aPendingPlayers.GetSize(), m_aPendingPlayers.GetData(), 1);
		m_aPendingPlayers.RemoveAll(false);
	}

	return true;
}

//	Clear all processed protocols in m_OldPrtcList
void CECGameSession::ClearOldProtocols()
{
	EnterCriticalSection(&g_csSession);

	for (int i=0; i < m_aOldProtocols.GetSize(); i++)
	{
		GNET::Protocol* p = m_aOldProtocols[i];
		p->Destroy();
	}

	m_aOldProtocols.RemoveAll(false);

	LeaveCriticalSection(&g_csSession);
}

//	Filter protocols. Return true if we shouldn't handle this protocol in
//	current state
bool CECGameSession::FilterProtocols(GNET::Protocol* pProtocol)
{
	bool bFilter = false;

	switch (pProtocol->GetType())
	{
	case PROTOCOL_CHALLENGE:
	case PROTOCOL_KEYEXCHANGE:
	case PROTOCOL_ONLINEANNOUNCE:
	case PROTOCOL_WAITQUEUESTATENOTIFY:
	case PROTOCOL_CANCELWAITQUEUE_RE:
	case PROTOCOL_ROLELIST_RE:	
	case RPC_PLAYERPOSITIONRESETRQST:
	case PROTOCOL_SELECTROLE_RE:
	case PROTOCOL_CREATEROLE_RE:
	case PROTOCOL_DELETEROLE_RE:
	case PROTOCOL_UNDODELETEROLE_RE:
	case PROTOCOL_MATRIXCHALLENGE:

		//	If we aren't in login state, ignore this data
		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_LOGIN)
			bFilter = true;

		break;

	case PROTOCOL_GAMEDATASEND:
	case PROTOCOL_CHATMESSAGE:
	case PROTOCOL_WORLDCHAT:
	case PROTOCOL_PLAYERLOGOUT:
	case PROTOCOL_PRIVATECHAT:
	case PROTOCOL_PLAYERBASEINFO_RE:
	case PROTOCOL_GETUICONFIG_RE:
	case PROTOCOL_SETUICONFIG_RE:
	case PROTOCOL_SETHELPSTATES_RE:
	case PROTOCOL_GETHELPSTATES_RE:
	case PROTOCOL_TRADESTART_RE:
	case PROTOCOL_TRADEMOVEOBJ_RE:
	case PROTOCOL_TRADEADDGOODS_RE:
	case PROTOCOL_TRADEREMOVEGOODS_RE:
	case PROTOCOL_TRADEDISCARD_RE:
	case PROTOCOL_TRADESUBMIT_RE:
	case PROTOCOL_TRADECONFIRM_RE:
	case PROTOCOL_TRADEEND:
	case PROTOCOL_GETPLAYERBRIEFINFO_RE:
	case PROTOCOL_ROLESTATUSANNOUNCE:
	case RPC_TRADESTARTRQST:

	case PROTOCOL_GMONLINENUM_RE:	
	case PROTOCOL_GMLISTONLINEUSER_RE:
	case PROTOCOL_GMKICKOUTUSER_RE:
	case PROTOCOL_GMSHUTUP_RE:
	case PROTOCOL_GMRESTARTSERVER_RE:
	case PROTOCOL_GMKICKOUTROLE_RE:
	case PROTOCOL_GMSHUTUPROLE_RE:

	case PROTOCOL_ADDFRIEND_RE:
	case PROTOCOL_GETFRIENDS_RE:
	case PROTOCOL_SETGROUPNAME_RE:
	case PROTOCOL_SETFRIENDGROUP_RE:
	case PROTOCOL_DELFRIEND_RE:
	case PROTOCOL_FRIENDSTATUS:
	case PROTOCOL_GETSAVEDMSG_RE:
	case RPC_ADDFRIENDRQST:

	case PROTOCOL_FACTIONOPREQUEST_RE:
	case PROTOCOL_FACTIONCREATE_RE:
	case PROTOCOL_FACTIONBROADCASTNOTICE_RE:
	case PROTOCOL_FACTIONUPGRADE_RE:
	case PROTOCOL_FACTIONDEGRADE_RE:
	case PROTOCOL_FACTIONLISTMEMBER_RE:
	case PROTOCOL_GETFACTIONBASEINFO_RE:
	case PROTOCOL_FACTIONDISMISS_RE:
	case PROTOCOL_GETPLAYERFACTIONINFO_RE:
	case PROTOCOL_FACTIONCHANGPROCLAIM_RE:
	case PROTOCOL_FACTIONCHAT:
	case PROTOCOL_FACTIONACCEPTJOIN_RE:
	case PROTOCOL_FACTIONRESIGN_RE:
	case PROTOCOL_FACTIONRENAME_RE:
	case PROTOCOL_FACTIONMASTERRESIGN_RE:
	case PROTOCOL_FACTIONLEAVE_RE:
	case PROTOCOL_FACTIONEXPEL_RE:
	case PROTOCOL_FACTIONDELAYEXPELANNOUNCE:
	case PROTOCOL_FACTIONAPPOINT_RE:
    case PROTOCOL_FACTIONALLIANCEAPPLY_RE:
    case PROTOCOL_FACTIONALLIANCEREPLY_RE:
    case PROTOCOL_FACTIONHOSTILEAPPLY_RE:
    case PROTOCOL_FACTIONHOSTILEREPLY_RE:
    case PROTOCOL_FACTIONREMOVERELATIONAPPLY_RE:
    case PROTOCOL_FACTIONREMOVERELATIONREPLY_RE:
	case PROTOCOL_FACTIONLISTRELATION_RE:
	case PROTOCOL_FACTIONLISTONLINE_RE:
	case PROTOCOL_FACTIONRELATIONRECVAPPLY:
	case PROTOCOL_FACTIONRELATIONRECVREPLY:
	case PROTOCOL_FACTIONRENAMEANNOUNCE:

	case PROTOCOL_CHATROOMCREATE_RE:
	case PROTOCOL_CHATROOMINVITE:
	case PROTOCOL_CHATROOMINVITE_RE:
	case PROTOCOL_CHATROOMJOIN_RE:
	case PROTOCOL_CHATROOMLEAVE:
	case PROTOCOL_CHATROOMEXPEL:
	case PROTOCOL_CHATROOMSPEAK:
	case PROTOCOL_CHATROOMLIST_RE:

	case PROTOCOL_ANNOUNCENEWMAIL:
	case PROTOCOL_GETMAILLIST_RE:
	case PROTOCOL_GETMAIL_RE:
	case PROTOCOL_GETMAILATTACHOBJ_RE:
	case PROTOCOL_DELETEMAIL_RE:
	case PROTOCOL_PRESERVEMAIL_RE:
	case PROTOCOL_PLAYERSENDMAIL_RE:

	case PROTOCOL_AUCTIONOPEN_RE:
	case PROTOCOL_AUCTIONBID_RE:
	case PROTOCOL_AUCTIONLIST_RE:
	case PROTOCOL_AUCTIONLISTUPDATE_RE:
	case PROTOCOL_AUCTIONCLOSE_RE:
	case PROTOCOL_AUCTIONGET_RE:
	case PROTOCOL_AUCTIONATTENDLIST_RE:
	case PROTOCOL_AUCTIONEXITBID_RE:
	case PROTOCOL_AUCTIONGETITEM_RE:

	case PROTOCOL_BATTLECHALLENGE_RE:
	case PROTOCOL_BATTLECHALLENGEMAP_RE:
	case PROTOCOL_BATTLEGETMAP_RE:
	case PROTOCOL_BATTLEENTER_RE:
	case PROTOCOL_BATTLESTATUS_RE:

	case PROTOCOL_SELLPOINT_RE:
	case PROTOCOL_GETSELLLIST_RE:
	case PROTOCOL_FINDSELLPOINTINFO_RE:
	case PROTOCOL_SELLCANCEL_RE:
	case PROTOCOL_BUYPOINT_RE:
	case PROTOCOL_ANNOUNCESELLRESULT:

	case PROTOCOL_CASHLOCK_RE:
	case PROTOCOL_CASHPASSWORDSET_RE:
	case PROTOCOL_AUTOLOCKSET_RE:
		
	case PROTOCOL_WEBTRADELIST_RE:
	case PROTOCOL_WEBTRADEATTENDLIST_RE:
	case PROTOCOL_WEBTRADEGETITEM_RE:
	case PROTOCOL_WEBTRADEUPDATE_RE:
		
	case PROTOCOL_SYSAUCTIONACCOUNT_RE:
	case PROTOCOL_SYSAUCTIONBID_RE:
	case PROTOCOL_SYSAUCTIONGETITEM_RE:
	case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
	case PROTOCOL_SYSAUCTIONLIST_RE:
		
	case PROTOCOL_CREATEFACTIONFORTRESS_RE:
	case PROTOCOL_GMGETPLAYERCONSUMEINFO_RE:
	case PROTOCOL_COUNTRYBATTLEMOVE_RE:
	case PROTOCOL_COUNTRYBATTLESYNCPLAYERLOCATION:
	case PROTOCOL_COUNTRYBATTLEGETMAP_RE:
	case PROTOCOL_COUNTRYBATTLEGETSCORE_RE:
	case PROTOCOL_COUNTRYBATTLEGETCONFIG_RE:
	case PROTOCOL_COUNTRYBATTLEPREENTERNOTIFY:
	case PROTOCOL_COUNTRYBATTLERESULT:
	case PROTOCOL_COUNTRYBATTLESINGLEBATTLERESULT:
	case PROTOCOL_CHANGEDS_RE:

	case PROTOCOL_PSHOPCREATE_RE:
	case PROTOCOL_PSHOPBUY_RE:
	case PROTOCOL_PSHOPSELL_RE:
	case PROTOCOL_PSHOPCANCELGOODS_RE:
	case PROTOCOL_PSHOPPLAYERBUY_RE:
	case PROTOCOL_PSHOPPLAYERSELL_RE:
	case PROTOCOL_PSHOPSETTYPE_RE:
	case PROTOCOL_PSHOPACTIVE_RE:
	case PROTOCOL_PSHOPMANAGEFUND_RE:
	case PROTOCOL_PSHOPDRAWITEM_RE:
	case PROTOCOL_PSHOPCLEARGOODS_RE:
	case PROTOCOL_PSHOPSELFGET_RE:
	case PROTOCOL_PSHOPPLAYERGET_RE:
	case PROTOCOL_PSHOPLIST_RE:
	case PROTOCOL_PSHOPLISTITEM_RE:
	case PROTOCOL_PLAYERPROFILEGETPROFILEDATA_RE:
	case PROTOCOL_PLAYERPROFILEGETMATCHRESULT_RE:
	case PROTOCOL_UNIQUEDATAMODIFYBROADCAST:

	case PROTOCOL_TANKBATTLEPLAYERAPPLY_RE:
	case PROTOCOL_TANKBATTLEPLAYERGETRANK_RE:

		//	If we aren't in game state, ignore this data
		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME)
			bFilter = true;

		break;

	case PROTOCOL_GETPLAYERIDBYNAME_RE:
	case PROTOCOL_WEBTRADEPREPOST_RE:
	case PROTOCOL_WEBTRADEPRECANCELPOST_RE:
	case PROTOCOL_WEBTRADEGETDETAIL_RE:
	case PROTOCOL_ERRORINFO:
	case PROTOCOL_KEEPALIVE:
	case PROTOCOL_UPDATEREMAINTIME:
	case PROTOCOL_SETCUSTOMDATA_RE:
	case PROTOCOL_GETCUSTOMDATA_RE:
	case PROTOCOL_ANNOUNCEFORBIDINFO:
	case PROTOCOL_PLAYERCHANGEDS_RE:
		
		break;

	default:
		break;
	}

	if (CECProtocolDebug::Instance().IsServerProtocolIgnored(pProtocol)){
		bFilter = true;
	}

	if (bFilter)
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("SERVER - %s filtered"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)));
#ifdef LOG_PROTOCOL
	else if (pProtocol->GetType() != PROTOCOL_GAMEDATASEND)
	{
		if( !g_pGame->GetRTDebug()->IsProtocolHide(*pProtocol, true) )
			g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("SERVER - %s"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)));
	}
#endif

	return bFilter;
}

bool CECGameSession::GetErrorMessage(int iRetCode, AWString& errMsg)
{
	bool bRet(false);

	while (true)
	{
		if (iRetCode < 0)
			break;

		const wchar_t* pErr = m_ErrorMsgs.GetWideString(iRetCode);
		if (!pErr)
			break;

		errMsg = pErr;
		bRet = true;
		break;
	}

	if (!bRet)
		errMsg.Format(_AL("Unknown error (%d)"), iRetCode);

	return bRet;
}
//	Get the server error string
bool CECGameSession::GetServerError(int iRetCode, AWString& errMsg)
{
	bool bRet(false);

	while (true)
	{
		if (iRetCode < 0)
			break;

		const wchar_t* pErr = m_ErrorMsgs.GetWideString(iRetCode + BASEIDX_LINKSEVERROR);
		if (!pErr)
			break;

		errMsg = pErr;
		bRet = true;
		break;
	}

	if(!bRet)
		errMsg.Format(_AL("Unknown error (%d)"), iRetCode);

	return bRet;
}

//	Output link server error
void CECGameSession::OutputLinkSevError(int iRetCode)
{
	AWString errMsg;
	GetServerError(iRetCode, errMsg);

	CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (pUIMan)
		pUIMan->ShowErrorMsg(errMsg);
	else
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, errMsg);

	a_LogOutput(1, "CECGameSession::OutputLinkSevError(iRetCode=%d)", iRetCode);
}

void CECGameSession::send_acanswer(int type, int seq, int reserved, int answer)
{
	StackChecker::ACTrace(0);
	
	GNET::ACAnswer p;
	p.roleid = m_iCharID;
	p.qtype = type;
	p.seq = seq;
	p.reserved = reserved;
	p.answer = answer;
	SendNetData(p, true);
}

//	Send game data
bool CECGameSession::SendGameData(void* pData, int iSize)
{
	StackChecker::ACTrace(7);
	using namespace GNET;

	GamedataSend p;
	p.data.replace(pData, iSize);
	return SendNetData(p);
}

//	Send chat data
bool CECGameSession::SendChatData(char cChannel, const ACHAR* szMsg, int iPack, int iSlot)
{
	using namespace GNET;

	if (!szMsg || !szMsg[0])
		return true;

	PublicChat p;
	p.channel	= cChannel;
	p.roleid	= m_iCharID;

	if (iPack == IVTRTYPE_CLIENT_GENERALCARD_PACK)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECInventory* clientPack = pHost ? pHost->GetPack(IVTRTYPE_CLIENT_GENERALCARD_PACK):NULL;
		if (pHost && clientPack)
		{
			CECIvtrItem* pCard = clientPack->GetItem(iSlot);
			if (pCard)
			{
				using namespace CHAT_C2S;
				chat_generalcard_collection item;
				item.cmd_id = CHAT_GENERALCARD_COLLECTION;
				item.card_id = pCard->GetTemplateID();
				p.data.replace(&item, sizeof(item));
			}
		}
	}
	else if (iPack >= 0 && iSlot >= 0)
	{
		using namespace CHAT_C2S;
		chat_equip_item item;
		item.cmd_id = CHAT_EQUIP_ITEM;
		item.where = iPack;
		item.index = iSlot;
		p.data.replace(&item, sizeof(item));
	}

	//	Fill message string
	int iMsgLen = a_strlen(szMsg);
	a_ClampRoof(iMsgLen, CHAT_ACHAR_LENGTH);

	int iSize = iMsgLen * sizeof (ACHAR);
	p.msg.replace(szMsg, iSize);

	SendNetData(p);

	//	Record these words to player
	if (cChannel == GP_CHAT_LOCAL || cChannel == GP_CHAT_FARCRY || cChannel == GP_CHAT_SUPERFARCRY ||
		cChannel == GP_CHAT_BATTLE || cChannel == GP_CHAT_COUNTRY || cChannel == GP_CHAT_GLOBAL)
	{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		int nEmotionSet = pHost->GetCurEmotionSet();
		ACString strMsg = szMsg;
		if (cChannel == GP_CHAT_SUPERFARCRY)
		{
			strMsg.CutRight(8); // ɾ�����ӵ���ɫ�������Ϣ
			nEmotionSet = SUPER_FAR_CRY_EMOTION_SET;
		}
		if (cChannel == GP_CHAT_GLOBAL)
		{
			strMsg.CutRight(8); // delete ending color code
			nEmotionSet = GLOBAL_EMOTION_SET;
		}

		CECIvtrItem *pItem = NULL;
		if (iPack >= 0 && iSlot >= 0)
		{
			CECInventory *pPack = pHost->GetPack(iPack);
			if (pPack)
			{
				pItem = pPack->GetItem(iSlot);
			}
		}
		pHost->SetLastSaidWords(strMsg, nEmotionSet, pItem);
	}

	return true;
}

//	Send private chat data
bool CECGameSession::SendPrivateChatData(const ACHAR* szDstName, const ACHAR* szMsg,
								BYTE byFlag/* 0 */, int idPlayer/* 0 */,
								int iPack/* -1 */, int iSlot /* -1 */)
{
	using namespace GNET;

	if (!szMsg || !szMsg[0])
		return true;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	pHost->AddRelatedPlayer(idPlayer);

	PrivateChat p;
	p.srcroleid = m_iCharID;
	p.dstroleid = idPlayer;
	p.channel	= byFlag;
	
	if (iPack == IVTRTYPE_CLIENT_GENERALCARD_PACK)
	{		
		CECInventory* clientPack = pHost ? pHost->GetPack(IVTRTYPE_CLIENT_GENERALCARD_PACK):NULL;
		if (pHost && clientPack)
		{
			CECIvtrItem* pCard = clientPack->GetItem(iSlot);
			if (pCard)
			{
				using namespace CHAT_C2S;
				chat_generalcard_collection item;
				item.cmd_id = CHAT_GENERALCARD_COLLECTION;
				item.card_id = pCard->GetTemplateID();
				p.data.replace(&item, sizeof(item));
			}
		}
	}
	else if (iPack >= 0 && iSlot >= 0)
	{
		using namespace CHAT_C2S;
		chat_equip_item item;
		item.cmd_id = CHAT_EQUIP_ITEM;
		item.where = iPack;
		item.index = iSlot;
		p.data.replace(&item, sizeof(item));
	}

	//	Fill host player name
	const ACHAR* szSrcName = pHost->GetName();
	int iSize = a_strlen(szSrcName) * sizeof (ACHAR);
	ASSERT(iSize);
	p.src_name.replace(szSrcName, iSize);

	//	Fill destination player's name
	iSize = a_strlen(szDstName) * sizeof (ACHAR);
	p.dst_name.replace(szDstName, iSize);

	//	Fill message string
	int iMsgLen = a_strlen(szMsg);
	a_ClampRoof(iMsgLen, CHAT_ACHAR_LENGTH);

	iSize = iMsgLen * sizeof (ACHAR);
	p.msg.replace(szMsg, iSize);

	return SendNetData(p);
}

//	Create a role
bool CECGameSession::CreateRole(const GNET::RoleInfo& Info, const GNET::Octets& Data)
{
	GNET::CreateRole cr;
	cr.userid	= m_iUserID;
	cr.roleinfo	= Info;
	cr.referid = Data;
	return SendNetData(cr);
}

//	Select role
bool CECGameSession::SelectRole(int cid, char flag/*=0*/)
{
	SetCharacterID(cid);
	return SendNetData(GNET::SelectRole(cid, flag));
}

//	Delete role
bool CECGameSession::DeleteRole(int cid)
{
	return SendNetData(GNET::DeleteRole(cid, 0));
}

//	Restore role
bool CECGameSession::RestoreRole(int cid)
{
	return SendNetData(GNET::UndoDeleteRole(cid, 0));
}

bool CECGameSession::QuitLoginQueue(){
	return SendNetData(GNET::CancelWaitQueue(GetUserID()));
}

//	Start game
bool CECGameSession::StartGame()
{
	m_pNetMan->ChangeState(m_idLink, &GNET::state_GDataExchgClient);
	bool bRet = SendNetData(GNET::EnterWorld(m_iCharID, _SID_INVALID));
	DoOvertimeCheck(true, OT_ENTERGAME, 30000);

	if (!CECReconnect::Instance().IsReconnecting()){			//	������Ϸǰ��¼��ɫID���ڵ�¼����
		if (!CECCrossServer::Instance().IsOnSpecialServer()){	//	ֻ��¼ԭ����¼��ɫID����
			CECReconnect::Instance().SetRoleID(m_iCharID);
		}
	}

	return bRet;
}

//	Re-login
bool CECGameSession::ReLogin(bool bSelRole)
{
	if (!bSelRole)
	{
		//	Return to login interface, Close connect
		Close();
	}
	else	//	Return to role selecting interface
	{	
		m_pNetMan->ChangeState(m_idLink, &state_GSelectRoleClient);
		SendNetData(RoleList(m_iUserID, _SID_INVALID, _HANDLE_BEGIN));
	}

	return true;
}

//	Get player's bese info (including custom data)
void CECGameSession::GetRoleBaseInfo(int iNumRole, const int* aRoleIDs)
{
	using namespace GNET;

	if (!iNumRole || !aRoleIDs)
		return;

//	static const int iNumLimit = 240;
//	static const int iNumLimit = 100;
	static const int iNumLimit = 128;
	int iCount = 0;

	while (iCount < iNumRole)
	{
		PlayerBaseInfo p;
		p.roleid = m_iCharID;

		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNumRole)
			iNumSend = iNumRole - iCount;

		for (int i=0; i < iNumSend; i++)
			p.playerlist.add(aRoleIDs[iCount+i]);

		SendNetData(p);

		iCount += iNumSend;
	}
}

//	Get player's customized data
void CECGameSession::GetRoleCustomizeData(int iNumRole, const int* aRoleIDs)
{
	using namespace GNET;

	if (!iNumRole || !aRoleIDs)
		return;

	static const int iNumLimit = 240;
//	static const int iNumLimit = 100;
	int iCount = 0;

	while (iCount < iNumRole)
	{
		GetCustomData p;
		p.roleid = m_iCharID;

		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNumRole)
			iNumSend = iNumRole - iCount;

		for (int i=0; i < iNumSend; i++)
			p.playerlist.add(aRoleIDs[iCount+i]);

		SendNetData(p);

		iCount += iNumSend;
	}
}

//	Get player's faction info
void CECGameSession::GetFactionInfo(int iNumFaction, const int* aFactinoIDs)
{
	m_stubbornFactionInfoSender->Add(iNumFaction, aFactinoIDs);
}

void CECGameSession::GetFactionInfoImmediately(int iNumFaction, const int* aFactinoIDs)
{
	using namespace GNET;

	static const int iNumLimit = 128;
	int iCount = 0;

	while (iCount < iNumFaction)
	{
		GetFactionBaseInfo p;
		p.roleid = m_iCharID;

		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNumFaction)
			iNumSend = iNumFaction - iCount;

		for (int i=0; i < iNumSend; i++)
			p.factionlist.add(aFactinoIDs[iCount+i]);

		SendNetData(p);

		iCount += iNumSend;
	}
}

//	Set custom data
void CECGameSession::SetRoleCustomizeData(int idRole, void* pData, int iDataSize)
{
	using namespace GNET;

	if (!pData || !iDataSize)
		return;
	
	SetCustomData p;
	p.roleid = idRole;
	p.custom_data.replace(pData, iDataSize);
	SendNetData(p);
}

//	Save necessary user configs (UI, shortcut, accelerate keys) to server
void CECGameSession::SaveConfigData(const void* pDataBuf, int iSize)
{
	using namespace GNET;

	if (!pDataBuf || !iSize)
		return;
	
	SetUIConfig p;
	p.roleid = m_iCharID;
	p.ui_config.replace(pDataBuf, iSize);
	SendNetData(p);
}

//	Load necessary user configs (UI, shortcut, accelerate keys) from server
void CECGameSession::LoadConfigData()
{
	using namespace GNET;

	GetUIConfig p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

//	Save help states data to server
void CECGameSession::SaveHelpStates(const void* pDataBuf, int iSize)
{
	using namespace GNET;

	if (!pDataBuf || !iSize)
		return;
	
	SetHelpStates p;
	p.roleid = m_iCharID;
	p.help_states.replace(pDataBuf, iSize);
	SendNetData(p);
}

//	Load help states data from server, and when help states received, we will call callback function to
//	let the caller get the data
void CECGameSession::LoadHelpStates(PONRECEIVEHELPSTATES pfnHelpStatesCallBack)
{
	m_pfnHelpStatesCallBack = pfnHelpStatesCallBack;

	using namespace GNET;

	GetHelpStates p;
	p.roleid = m_iCharID;
	SendNetData(p);
}

//	Set User to locked online state
void CECGameSession::SetLockedLogoutState()
{

	using namespace GNET;

	SetLockedLogoutUser p;
	p.userid = m_iCharID;
	SendNetData(p);
}

/*	Get player's brief information

	iReason: reason to get player's information

		0: try to get host player's team member's information.
		1: try to get someone's name who just said something.
		2: just try to get players' names
		3: get player's brief infomation to handle team invitation
*/
void CECGameSession::GetPlayerBriefInfo(int iNumPlayer, int* aIDs, int iReason)
{
	if (!iNumPlayer || !aIDs)
		return;

	static const int iNumLimit = 100;
	int iCount = 0;

	while (iCount < iNumPlayer)
	{
		GNET::GetPlayerBriefInfo p;
		p.roleid = m_iCharID;
		p.reason = (BYTE)iReason;

		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iNumPlayer)
			iNumSend = iNumPlayer - iCount;

		for (int i=0; i < iNumSend; i++)
		{
			ASSERT(aIDs[iCount+i]);
			p.playerlist.add(aIDs[iCount+i]);
		}

		SendNetData(p);

		iCount += iNumSend;
	}
}

/*	Get player's ID by name

	iReason: the reason to get player's ID
		
		0: no special reason, just get it
		1: for invite a player who is very far from us to a team
*/
void CECGameSession::GetPlayerIDByName(const ACHAR* szName, int iReason)
{
	GNET::GetPlayerIDByName p;

	int iSize = a_strlen(szName) * sizeof (ACHAR);
	p.rolename.replace(szName, iSize);
	p.reason = (BYTE)iReason;
	SendNetData(p);
}

//	Report ChatSpeak to GM
void CECGameSession::SubmitChatSpeakToGM(const ACHAR* szContent, int idPluginUser, const ACHAR *szPluginUserName)
{
	if (!szContent || !szContent[0])
		return;
	
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CELPrecinct* pPrecinct = g_pGame->GetGameRun()->GetWorld()->GetCurPrecinct();
	A3DVECTOR3 v = pHost->GetPos();
	
	GNET::ReportChat p;
	
	p.roleid = m_iCharID;
	p.localsid = 0;

	if (idPluginUser != 0 && szPluginUserName)
	{
		p.dst_roleid = idPluginUser;
		p.dst_rolename.replace(szPluginUserName, a_strlen(szPluginUserName) * sizeof(ACHAR));
	}
	else
	{
		p.dst_roleid = 0;
		p.dst_rolename.clear();
	}

	//	Fill content string
	int iMsgLen = a_strlen(szContent);
	if (iMsgLen * sizeof (ACHAR) > 160)
		iMsgLen = 160 / sizeof (ACHAR);
	
	int iSize = iMsgLen * sizeof (ACHAR);
	p.content.replace(szContent, iSize);

	SendNetData(p);
}

//	Report problems to GM
void CECGameSession::SubmitProblemToGM(const ACHAR* szContent, int idPluginUser, const ACHAR *szPluginUserName)
{
	if (!szContent || !szContent[0])
		return;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CELPrecinct* pPrecinct = g_pGame->GetGameRun()->GetWorld()->GetCurPrecinct();
	A3DVECTOR3 v = pHost->GetPos();
	
	GNET::Report2GM p;

	p.roleid = m_iCharID;
	p.rolename.replace(pHost->GetName(), a_strlen(pHost->GetName()) * sizeof (ACHAR));
	p.posx = v.x;
	p.posy = v.y;
	p.posz = v.z;

	//	Fill content string
	int iMsgLen = a_strlen(szContent);
	if (iMsgLen * sizeof (ACHAR) > 160)
		iMsgLen = 160 / sizeof (ACHAR);

	int iSize = iMsgLen * sizeof (ACHAR);
	p.content.replace(szContent, iSize);

	if (pPrecinct)
		p.mapzone.replace(pPrecinct->GetName(), a_strlen(pPrecinct->GetName()) * sizeof (ACHAR));

	if (idPluginUser != 0 && szPluginUserName)
	{
		p.com_roleid = idPluginUser;
		p.com_rolename.replace(szPluginUserName, a_strlen(szPluginUserName) * sizeof(ACHAR));
	}
	else
	{
		p.com_roleid = 0;
		p.com_rolename.clear();
	}

	SendNetData(p);
}

//	Impeach else player to GM
void CECGameSession::ImpeachPlayerToGM(const ACHAR* szName, const ACHAR* szContent)
{
	if (!szName || !szName[0] || !szContent || !szContent[0])
		return;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CELPrecinct* pPrecinct = g_pGame->GetGameRun()->GetWorld()->GetCurPrecinct();
	A3DVECTOR3 v = pHost->GetPos();

	GNET::Complain2GM p;

	p.roleid = m_iCharID;
	p.rolename.replace(pHost->GetName(), a_strlen(pHost->GetName()) * sizeof (ACHAR));
	p.posx = v.x;
	p.posy = v.y;
	p.posz = v.z;
	p.comp_rolename.replace(szName, a_strlen(szName) * sizeof (ACHAR));

	//	Fill content string
	int iMsgLen = a_strlen(szContent);
	if (iMsgLen * sizeof (ACHAR) > 160)
		iMsgLen = 160 / sizeof (ACHAR);

	int iSize = iMsgLen * sizeof (ACHAR);
	p.content.replace(szContent, a_strlen(szContent) * sizeof (ACHAR));

	if (pPrecinct)
		p.mapzone.replace(pPrecinct->GetName(), a_strlen(pPrecinct->GetName()) * sizeof (ACHAR));

	SendNetData(p);
}

void CECGameSession::ReportPluginToSystem(int idPluginUser, const ACHAR* szContent)
{
	if (!idPluginUser || !szContent){
		ASSERT(false);
		return;
	}

	GNET::PlayerAccuse p;
	
	p.roleid = m_iCharID;
	p.dst_roleid = idPluginUser;
	
	//	Fill content string
	int iMsgLen = a_strlen(szContent);
	if (iMsgLen * sizeof (ACHAR) > 160)
		iMsgLen = 160 / sizeof (ACHAR);	
	int iSize = iMsgLen * sizeof (ACHAR);
	p.content.replace(szContent, a_strlen(szContent) * sizeof (ACHAR));
		
	SendNetData(p);
}

DWORD CECGameSession::GetPingTime(int idServer, DWORD& dwStatus, DWORD& dwCreateTime, unsigned char& cExpRate)
{
	EnterCriticalSection(&m_csPingSection);
	DWORD dwRet = 99999999;
	dwStatus = 0;
	cExpRate = 10;
	if( m_Ping.GetSize() > idServer && m_Ping[idServer] != NULL &&
		m_Ping[idServer]->idLink != (DWORD)-1)
	{
		dwRet = m_Ping[idServer]->dwTime;
		dwStatus = m_Ping[idServer]->dwStatus;
		dwCreateTime = m_Ping[idServer]->dwCreateTime;
		cExpRate = m_Ping[idServer]->cExpRate;
	}
	LeaveCriticalSection(&m_csPingSection);
	return dwRet;
}

void CECGameSession::RefreshPendingFactionMessage()
{
	for (int j=0; j < m_aPendingProtocols.GetSize(); j++)
	{
		Protocol* pProtocol = m_aPendingProtocols[j];
		
		bool bProcessed(false);
		switch (pProtocol->GetType())
		{
		case PROTOCOL_WORLDCHAT:
			{
				WorldChat* pChat = static_cast<WorldChat*>(pProtocol);
				
				if(pChat->channel != GP_CHAT_SYSTEM)
					continue;

				if (!OnBattleChatMessage(pChat, NULL) &&
					!OnFortressChatMessage(pChat, NULL))
				{
					continue;
				}
				bProcessed = true;
				break;
			}
		case PROTOCOL_CHATMESSAGE:
			{
				ChatMessage* pChat = (ChatMessage*)pProtocol;
				if(pChat->channel != GP_CHAT_SYSTEM)
					continue;

				if (!OnBattleChatMessage(pChat, NULL) &&
					!OnFortressChatMessage(pChat, NULL))
				{
					continue;
				}
				bProcessed = true;
				break;
			}

#ifdef RESOURCE_BATTLE
		case PROTOCOL_FACTIONRESOURCEBATTLEPLAYERQUERYRESULT:
			if (OnPrtcFactionPVPResult(pProtocol, false)){
				bProcessed = true;
			}
			break;
#endif
		default:
			if (OnFactionDiplomacyProtocols(pProtocol, false))
			{
				bProcessed = true;
				break;
			}
		}
		if (bProcessed){			
			pProtocol->Destroy();
			m_aPendingProtocols.RemoveAtQuickly(j);
			j--;
		}
	}
}

void CECGameSession::OnPrtcChallenge(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	DoOvertimeCheck(false, OT_CHALLENGE, 0);

	Challenge* p = (Challenge*)pProtocol;

	AString str((const char *)p->edition.begin(), p->edition.size());

	if (p->version != g_pGame->GetGameVersion() || stricmp(g_pGame->GetVersionString(), str))
	{
		a_LogOutput(1, "local  ver: %s", g_pGame->GetVersionString());
		a_LogOutput(1, "server ver: %s", (const char *)str);
		if (p->version == g_pGame->GetGameVersion() && CECCommandLine::GetDoNotCheckSmallVersion())
		{
			a_LogOutput(1, "CECGameSession::OnPrtcChallenge(), Wrong Data Version!");
		}
		else
		{
			if (str.IsEmpty())
			{
				// ������Ϊ��ʱ����������������� IP ��ַ����
				const SockAddr &sa = GNET::GameClient::GetInstance().GetLastAddr();
				const struct sockaddr_in *addr = sa;
				a_LogOutput(1, "ip = %s, port = %u", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
			}
			CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
			CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
			if (pUIMan)
			{
				if (p->version > g_pGame->GetGameVersion() || stricmp(g_pGame->GetVersionString(), str))
					pUIMan->ShowErrorMsg(pStrTab->GetWideString(FIXMSG_WRONGVERSION));
				else
					pUIMan->ShowErrorMsg(pStrTab->GetWideString(FIXMSG_SERVERUPDATE));
			}
			else
				::MessageBox(g_pGame->GetGameInit().hWnd, _AL("Element client"), pStrTab->GetWideString(FIXMSG_WRONGVERSION), MB_OK);
			
			//	Disconnect from server
			Close();

			//	�汾��ƥ�䣬ת����¼ʧ��
			if (CECCrossServer::Instance().IsWaitLogin()){
				CECCrossServer::Instance().OnLoginFail();
			}
			if (CECReconnect::Instance().IsReconnecting()){
				CECReconnect::Instance().OnReconnectFail();
			}
			return;
		}
	}

	if (CECCrossServer::Instance().IsWaitLogin())
	{
		m_dwNewbieTime = 0;
		CECCrossServer::Instance().OnPrtcChallenge();
		return;
	}
	
	ASSERT(sizeof (m_SevAttr) == sizeof (GNET::Attr));
	memcpy(&m_SevAttr, p->nonce.begin(), sizeof (GNET::Attr));
	m_dwNewbieTime = *((unsigned int*)p->nonce.begin()+1);

	GameClient::LoginParameter para;
	para.name = m_strUserName;
	para.passwd = m_strPassword;
	para.nonce = p->nonce;
	para.algo = p->algo;
	para.use_token = m_tokenType;
	para.token = m_strToken;

	Response res;
	m_pNetMan->SetupResponse(res, para);

#ifdef _MSC_VER
	HwLoader hl;
	// INSERT HWID CODE HERE
	std::string hwcode = hl.getDeviceFingerPrint();
#else
	// Hardware fingerprinting not available on MinGW, use placeholder
	std::string hwcode = "mingw-placeholder-hwid";
#endif

	res.hwid.replace(hwcode.c_str(), hwcode.size());

	//	���ӿͻ��˱�ʶ�����ڷ�������ͳ��
	int iClientID = g_pGame->GetConfigs()->GetClientID();
	res.cli_fingerprint.insert(res.cli_fingerprint.end(), &iClientID, sizeof(iClientID));

	if (!m_strPassword.IsEmpty())
	{
		//	�������ʱ��������루ʹ�� token ��¼ʱ�����룩
		char * szBuffer = m_strPassword.LockBuffer();
		memset(szBuffer, 0, strlen(szBuffer));
		m_strPassword.UnlockBuffer();
	}

	SendNetData(res);

	if (m_pNetMan->IsUseUsbKey()){		
		a_LogOutput(1, "CECReconnect reset for use usb key");
		CECReconnect::Instance().ResetState();
	}
}

void CECGameSession::OnPrtcKeyExchange(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	Octets oSecurity, iSecurity;
	KeyExchange* p = (KeyExchange*)pProtocol;
	p->Setup(m_pNetMan, m_idLink, m_strUserName, m_bKickUser, oSecurity, iSecurity);

	//	��¼���ܲ�������ת��
	CECCrossServer::Instance().SetOSecurity(oSecurity);
	CECCrossServer::Instance().SetISecurity(iSecurity);

	SendNetData(p);
}

void CECGameSession::OnPrtcOnlineAnnounce(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	OnlineAnnounce* p = (OnlineAnnounce*)pProtocol;
	m_iUserID = p->userid;
	m_pNetMan->SetZoneID(p->zoneid);
	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	ASSERT(pLoginUIMan);

	if (CECCrossServer::Instance().IsWaitLogin())
	{
		//	���ת����
		m_iiAccountTime = 0;
		m_iiFreeGameTime = 0;
		m_dwCreatedTime = 0;
		m_iFreeGameEnd = 0;
		m_dwLastAccTime = 0;

		pLoginUIMan->SetReferrerBound(false);
		pLoginUIMan->SetPasswdFlag(0);
		pLoginUIMan->SetUsbBind(-1);
		pLoginUIMan->SetRoleListReady(false);

		g_pGame->GetGameRun()->ResetAccountLoginInfo();
		g_pGame->GetGameRun()->SetAccountInfoFlag(0);

		CECCrossServer::Instance().OnPrtcOnlineAnnounce();
		return;
	}
	
	m_pNetMan->ChangeState(m_idLink, &state_GSelectRoleClient);

	SendNetData(RoleList(m_iUserID, _SID_INVALID, _HANDLE_BEGIN));

	m_iiAccountTime		= ((INT64)p->remain_time) * 1000;
	m_iiFreeGameTime	= ((INT64)p->free_time_left) * 1000;
	m_dwCreatedTime		= p->creatime;
	m_iFreeGameEnd		= p->free_time_end;
	m_dwLastAccTime		= a_GetTime();

	//	Enter select role interface
	pLoginUIMan->SetReferrerBound(false);
	pLoginUIMan->SetPasswdFlag(p->passwd_flag);
	pLoginUIMan->SetUsbBind(p->usbbind);
	pLoginUIMan->LaunchCharacter();

	//	Reset Account login info
	g_pGame->GetGameRun()->ResetAccountLoginInfo();
	g_pGame->GetGameRun()->SetAccountInfoFlag(p->accountinfo_flag);
	
	AString strAgentAccount = GetAgentAccount();
	if (strAgentAccount.IsEmpty()){
		//	 �ǵ�������¼�ɹ�ʱ����¼�û���
		CECRecordAccount::Instance().Add(m_strUserNameUnicode, CECServerList::Instance().GetSelected());
	}

	//	�ѳɹ����ߣ��ɸ��ĵ�¼����״̬
	if (!CECReconnect::Instance().IsReconnecting()){
		if (CECReconnect::Instance().HasReconnectPolicyCandidate()){
			CECReconnect::Instance().AcceptReconnectPolicyCandidate();
			CECReconnect::Instance().SetRoleID(0);	//	�����¼��ɫID������ѡ�˽�������
		}
	}else if (!CECReconnect::Instance().GetRoleIDToSelect()){
		CECReconnect::Instance().OnReconnectSuccess();
	}
}

void CECGameSession::OnPrtcPlayerRenameRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	PlayerRename_Re*p = (PlayerRename_Re*)pProtocol;

	if(p->retcode != ERR_SUCCESS)
	{

		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

		ACString str;
		GetServerError(p->retcode,str);
		
		pGameUI->MessageBox("",str,MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		// �����ɹ����Ͽ�����
		SetBreakLinkFlag(LBR_ROLE_RENAME);
	}
}

void CECGameSession::OnPrtcPlayerNameUpdate(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	PlayerNameUpdate*p = (PlayerNameUpdate*)pProtocol;

	ACString strNewName;
	_cp_str(strNewName, p->newname.begin(), p->newname.size());

	if(strNewName.IsEmpty())
	{
		a_LogOutput(1, "PlayerNameUpdate, newname IsEmpty.");
		return;
	}
	
	if (p->roleid == g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID())
	{
		//	�������ʱҲ�ᷢ���������������˴����Ե�
		return;
	}

	ACString strOldName = g_pGame->GetGameRun()->GetPlayerName(p->roleid,false);
	if(!strOldName.IsEmpty())
	{
		// ������ͬ
		if(strNewName == strOldName)
			return;
	}

	g_pGame->GetGameRun()->AddPlayerName(p->roleid,strNewName, true);

	CECGameUIMan * pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	bool bAddHint = !strOldName.IsEmpty();	//	ԭ����δ�鵽ʱ����ʾ
	if(bAddHint && g_pGame->GetGameRun()->GetHostPlayer()->GetSpouse() == p->roleid)
	{
		// ��ż
		bAddHint = false;
		ACString str;
		str.Format(pGameUI->GetStringFromTable(10154), strOldName, strNewName);
		g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_MISC);
	}

	// ���ѵ�����Ҫ����һ��
	CECFriendMan::FRIEND *pFriend = g_pGame->GetGameRun()->GetHostPlayer()->GetFriendMan()->GetFriendByID(p->roleid);
	if(pFriend)
	{
		if(bAddHint)
		{
			// ����
			bAddHint = false;
			ACString str;
			str.Format(pGameUI->GetStringFromTable(10155), strOldName, strNewName);
			g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_MISC);
		}

		pFriend->strName = strNewName;

		// ���ѽ�����Ҫˢ��һ��
		CDlgFriendList *pFriendDlg = (CDlgFriendList *)pGameUI->GetDialog("Win_FriendList");
		pFriendDlg->BuildFriendList();

		// ǧ�ﴫ�����Ҫˢ��һ��
		CDlgMailToFriends*pMFDlg = (CDlgMailToFriends *)pGameUI->GetDialog("Win_MailtoFriend");
		pMFDlg->BuildFriendListEx();

		// ��������������
		pGameUI->RefreshFriendChatPlayerName(p->roleid);

		// �̳����ͽ���ˢ��
		CDlgGivingFor* pGivingFor = dynamic_cast<CDlgGivingFor*>(pGameUI->GetDialog("Win_GivingFor"));
		if(pGivingFor)
			pGivingFor->PlayerNameUpdate(p->roleid,strNewName,pFriend->nLevel,pFriend->iProfession);
	}
	
	Faction_Mem_Info *pFactionMember = g_pGame->GetFactionMan()->GetMember(p->roleid);
	if(pFactionMember)
	{
		if(bAddHint)
		{
			// ���ɳ�Ա
			bAddHint = false;
			ACString str;
			str.Format(pGameUI->GetStringFromTable(10156), strOldName, strNewName);
			g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_MISC);
		}
		// ���ɽ�����Ҫˢ��һ��
		pGameUI->UpdateFactionList();
		// �̳����ͽ���ˢ��
		CDlgGivingFor* pGivingFor = dynamic_cast<CDlgGivingFor*>(pGameUI->GetDialog("Win_GivingFor"));
		if(pGivingFor)
			pGivingFor->PlayerNameUpdate(p->roleid,strNewName,pFactionMember->GetLev(),pFactionMember->GetProf());
	}
	
	// elseplayer cache
	CECEPCacheData* pCacheData = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayerCacheData(p->roleid);
	if(pCacheData)
	{
		pCacheData->m_strName = strNewName;	
	}

	if(bAddHint)
	{
		// ������
		bAddHint = false;
		ACString str;
		str.Format(pGameUI->GetStringFromTable(10157), strOldName, strNewName);
		g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_MISC);
	}

	// ˽�Ľ������
	CDlgChat * pDlgWhisper = dynamic_cast<CDlgChat*>(pGameUI->GetDialog("Win_WhisperChat"));
	pDlgWhisper->RefreshWhisperPlayerName(p->roleid);
}

void CECGameSession::OnPrtcKingCommandPoint(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	int iCommandPoint = 0;
	if( pProtocol->GetType() == PROTOCOL_COUNTRYBATTLEGETKINGCOMMANDPOINT_RE )
	{
		CountryBattleGetKingCommandPoint_Re* p = dynamic_cast<CountryBattleGetKingCommandPoint_Re*>(pProtocol);
		iCommandPoint = p->command_point;
	}
	else if( pProtocol->GetType() == PROTOCOL_COUNTRYBATTLEKINGASSIGNASSAULT_RE )
	{
		CountryBattleKingAssignAssault_Re* p = dynamic_cast<CountryBattleKingAssignAssault_Re*>(pProtocol);
		iCommandPoint = p->command_point;
	}

	g_pGame->GetGameRun()->GetHostPlayer()->SetKingPoint(iCommandPoint);
}

void CECGameSession::OnPrtcWaitQueueNotify(GNET::Protocol* pProtocol){
	const WaitQueueStateNotify *p = static_cast<GNET::WaitQueueStateNotify*>(pProtocol);
	CECLoginQueue::Instance().OnPrtcWaitQueueStateNotify(p);
}

void CECGameSession::OnPrtcCancelWaitQueueRe(GNET::Protocol* pProtocol){
	const CancelWaitQueue_Re *p = static_cast<GNET::CancelWaitQueue_Re*>(pProtocol);
	CECLoginQueue::Instance().OnPrtcCancelWaitQueue_Re(p);
}

void CECGameSession::OnPrtcRoleListRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	ASSERT(pLoginUIMan);

	RoleList_Re* p = (RoleList_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - %s return error(%d)"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)), p->result);

		if (p->handle != _HANDLE_END)
			SendNetData(RoleList(m_iUserID, _SID_INVALID, p->handle));
		else
		{
			OutputLinkSevError(p->result);
			pLoginUIMan->SelectLatestCharacter();
		}

		return;
	}

	g_pGame->GetGameRun()->ClearNameIDPairs();

	ASSERT(m_iUserID == p->userid);

	int i, iNumRole = p->rolelist.size();
	for (int i=0; i < iNumRole; i++)
		pLoginUIMan->AddCharacter(&p->rolelist[i]);

	if (p->handle != _HANDLE_END)
		SendNetData(RoleList(m_iUserID, _SID_INVALID, p->handle));
	else{
#ifdef ARC
		// ��鵱ǰ��¼�˺��Ƿ���Arc�˺���ͬ����ͬ�Ļ�����arc�ĸ����û��ӿ�
		if (g_GameCfgs.GetEnableArcAsia()){
			CECArcAsia::GetSingleton().CheckUser(AS2AC(m_strUserName));
		}
#endif
		pLoginUIMan->SetRoleListReady(true);
		pLoginUIMan->SelectLatestCharacter();
		if (CECReconnect::Instance().IsReconnecting()){
			if (!CECReconnect::Instance().GetRoleIDToSelect()){
				ASSERT(false);
				CECReconnect::Instance().OnReconnectSuccess();
			}else if (!pLoginUIMan->SelectLoginChar(pLoginUIMan->FindCharByRoleID(CECReconnect::Instance().GetRoleIDToSelect()))){
				CECReconnect::Instance().OnReconnectFail();
			}
		}
	}
}

void CECGameSession::OnPrtcResetPosition(GNET::Protocol *pProtocol)
{
	using namespace GNET;
	
	PlayerPositionResetRqst* p = (PlayerPositionResetRqst*)pProtocol;
	PlayerPositionResetRqstArg* pArg = (PlayerPositionResetRqstArg*)p->GetArgument();

	CECGameRun *pGameRun = g_pGame->GetGameRun();
	CECLoginUIMan* pUIMan = pGameRun->GetUIManager()->GetLoginUIMan();
	const RoleInfo & role = pGameRun->GetSelectedRoleInfo();
	if (role.roleid != pArg->roleid)
	{
		//	�ȴ�����������
		ASSERT(false);
		a_LogOutput(1, "CECGameSession::OnPrtcResetPosition, different roleid(%d!=%d)", role.roleid, pArg->roleid);
		p->Destroy();
		return;
	}
	
	//	��ɫ����
	ACString strName = ACString((const ACHAR*)role.name.begin(), role.name.size() / sizeof (ACHAR));

	//	�ϴ��˳�λ��
	CECInstance *pInstLast = pGameRun->GetInstance(role.worldtag);
	ACString strLast;
	if (pInstLast) strLast = pInstLast->GetName();
	else a_LogOutput(1, "CECGameSession::OnPrtcResetPosition, Invalid last worldid=%d", role.worldtag);
	if (strLast.IsEmpty()) strLast.Format(_AL("worldid = %d"), role.worldtag);

	//	����ѡ������ǰ����λ��
	CECInstance *pInstNew = pGameRun->GetInstance(pArg->worldtag);
	ACString strNew;
	if (pInstNew) strNew = pInstNew->GetName();
	else a_LogOutput(1, "CECGameSession::OnPrtcResetPosition, Invalid new worldid=%d", pArg->worldtag);
	if (strNew.IsEmpty()) strNew.Format(_AL("worldid = %d"), pArg->worldtag);

	//	�޷�����ԭ��
	ACString strReason;
	strReason = pUIMan->GetStringFromTable(236+pArg->reason-1);
	
	//	��ʾ��ʾ��
	PAUIDIALOG pMsgBox = NULL;
	ACString strMsg;
	strMsg.Format(pUIMan->GetStringFromTable(235), strName, strLast, strReason, strNew);
	pUIMan->MessageBox("Redirect_LoginPos", strMsg, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(GetTickCount());
	pMsgBox->SetDataPtr(p);

	//	������ʱ��ֱ������Ϊ�����ɹ�����ֹ�������̡��Ա��������򣬽����������ѡ����ǿ��ѡ����븱��������ܻᵼ�¶Ͽ�����
	if (CECReconnect::Instance().IsReconnecting()){
		CECReconnect::Instance().OnReconnectSuccess();
	}
	//	�Ŷ�״̬�£���Ϊ�Ŷӳɹ�
	CECLoginQueue::Instance().ClearState();
}

void CECGameSession::OnPrtcSelectRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	SelectRole_Re* p = (SelectRole_Re*)pProtocol;	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();

	//	����ǵ�¼�ض���RPC_PLAYERPOSITIONRESETRQST������֮ǰ�յ�Э��ʱ�Ѿ�������
	//	��������±����Ŷӳɹ�������������Ϸ��
	//	�д������ص�ѡ�˽��棬��ʱҲ������Ŷ�״̬
	CECLoginQueue::Instance().ClearState();

	if (p->result == 431)
	{
		//	RPC_PLAYERPOSITIONRESETRQST Э��ɹ��ض���
		RoleInfo info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
		RoleInfo redirectInfo = g_pGame->GetGameRun()->GetRedirectLoginPosRoleInfo();
		if (info.roleid != redirectInfo.roleid)
		{
			ASSERT(false);
			a_LogOutput(1, "CECGameSession::OnPrtcSelectRoleRe, different roleid(%d!=%d)", info.roleid, redirectInfo.roleid);

			//	�쳣��������������� SelectRole
			if (pLoginUIMan) pLoginUIMan->SelectRoleSent(false);
		}
		else
		{
			g_pGame->GetGameRun()->SetSelectedRoleInfo(redirectInfo);
			g_pGame->GetGameRun()->SetRedirectLoginPosRoleInfo(RoleInfo());
			g_pGame->GetGameSession()->SelectRole(info.roleid);

			//	�ȴ����ض������̵� SelectRoleRe ����
		}
		return;
	}

	//	������ʲô���ؽ������������� SelectRole
	if (pLoginUIMan) pLoginUIMan->SelectRoleSent(false);
	
	if (p->result != ERR_SUCCESS)
	{
		OutputLinkSevError(p->result);
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - %s return error(%d)"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)), p->result);
		return;
	}

	const GNET::RoleInfo& role = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	CECInstance *pInst = g_pGame->GetGameRun()->GetInstance(role.worldtag);
	if (!pInst){
		a_LogOutput(1, "CECGameSession::OnPrtcSelectRoleRe, Unknown worldtag=%d", role.worldtag);
		if (pLoginUIMan){
			pLoginUIMan->ShowErrorMsg2(pLoginUIMan->GetStringFromTable(10159), "MsgBox_LoginFail");
		}
		return;
	}

	// ɾ�������ͼ
	CECRandomMapProcess::DeleteAllRandomMapDataForSingleUser();

	g_pGame->ResetPrivilege(p->auth.GetVector().empty() ? NULL : &p->auth.GetVector()[0], p->auth.size());

	if (pLoginUIMan){
		pLoginUIMan->LaunchLoading();
	}
}

void CECGameSession::OnPrtcCreateRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	CreateRole_Re* p = (CreateRole_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		OutputLinkSevError(p->result);
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - %s return error(%d)"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)), p->result);
		return;
	}
	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	ASSERT(pLoginUIMan);
	pLoginUIMan->CreateCharacter(p->roleinfo);
}

void CECGameSession::OnPrtcDeleteRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	DeleteRole_Re* p = (DeleteRole_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		OutputLinkSevError(p->result);
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - %s return error(%d)"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)), p->result);
		return;
	}
	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	ASSERT(pLoginUIMan);
	pLoginUIMan->DelCharacter(p->result, p->roleid);
}

void CECGameSession::OnPtrcUndoDeleteRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	UndoDeleteRole_Re* p = (UndoDeleteRole_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		OutputLinkSevError(p->result);
		g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - %s return error(%d)"), AS2AC(g_pGame->GetRTDebug()->GetProtocolName(*pProtocol, true)), p->result);
		return;
	}
	
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	ASSERT(pLoginUIMan);
	pLoginUIMan->RestoreCharacter(p->result, p->roleid);
}

static bool IsPolicyChat(const GNET::ChatMessage *p)
{
	//	Э�������Ƿ�֧�ִ�NPC�������������
	bool bOK(false);
	switch (p->channel)
	{		
	case GP_CHAT_LOCAL:				//	����� NPC �ɼ������������
		if (p->srcroleid == 0 ||	//	�����Ժ����б�� $A ��
			ISNPCID(p->srcroleid)){	//	�����Ժ������ޱ��  ��
			bOK = true;
		}
		break;
	case GP_CHAT_BATTLE:		//	�����ս����ָ����Ӫ��ȫ����Һ��������Ժ����б�� $F��$T ��
		if (p->srcroleid == 0){	//	Ϊ���Ժ��б仯�����Ӵ��ж�
			bOK = true;
		}
		break;
	case GP_CHAT_BROADCAST:		//	����ȫ��������ң����Ժ����б�� $S ��
		if (p->srcroleid == 0){
			bOK = true;
		}
		break;
	case GP_CHAT_INSTANCE:		//	���򸱱�����ң����Ժ����б�� $I ���� $X ���ʱ p->srcroleid == 1����ʱֻ��������Ļ���룩
		if (p->srcroleid == 0 || p->srcroleid == 1){
			bOK = true;
		}
		break;
	}
	return bOK;
}


static bool CanFormatCoordText(const GNET::ChatMessage *p)
{
	bool bOK(false);
	switch (p->channel)	{
	case GP_CHAT_LOCAL:				//	����� NPC �ɼ������������
		if (p->srcroleid == 0 ||	//	�����Ժ����б�� $A ��
			ISNPCID(p->srcroleid)){	//	�����Ժ������ޱ��  ��
			bOK = true;
		}
		break;
	case GP_CHAT_BATTLE:		//	�����ս����ָ����Ӫ��ȫ����Һ��������Ժ����б�� $F��$T ��
		if (p->srcroleid == 0){	//	Ϊ���Ժ��б仯�����Ӵ��ж�
			bOK = true;
		}
		break;
	case GP_CHAT_BROADCAST:		//	����ȫ��������ң����Ժ����б�� $S ��
		if (p->srcroleid == 0){
			bOK = true;
		}
		break;
	case GP_CHAT_INSTANCE:		//p->srcroleid == 1 ʱ������������滻
		if (p->srcroleid == 0){
			bOK = true;
		}
		break;
	}
	return bOK;
}

//	Process chat data
bool CECGameSession::OnPrtcChatMessage(GNET::Protocol* pProtocol, bool bCalledagain)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	using namespace GNET;
	ChatMessage* p = (ChatMessage*)pProtocol;

	int levelBlock = g_pGame->GetConfigs()->GetBlackListSettings().levelBlock;
	if(p->srclevel > 0 && p->srclevel < levelBlock)
	{
		if(p->channel == GP_CHAT_LOCAL || p->channel == GP_CHAT_WHISPER || p->channel == GP_CHAT_TRADE)
		{
			if(!g_pGame->GetGameRun()->GetHostPlayer()->IsOmitBlocking(p->srcroleid))
			{
				// should be filted by level
				return true;
			}
		}
	}
	
	CECIvtrItem* pItem = CHAT_S2C::CreateChatItem(p->data);
	auto_delete<CECIvtrItem> tmp(pItem);

	ACHAR szMsg[1024];
	ACString strTemp((const ACHAR*)p->msg.begin(), p->msg.size() / sizeof (ACHAR));
	ACString strMsgOrigion = strTemp;// ԭʼ������Ϣ
	strTemp = pGameUI->FilterInvalidTags(strTemp, pItem==NULL);
	if (IsPolicyChat(p)){
		CHAT_S2C::PolicyChatParameter* pPolicyChatPara = CHAT_S2C::CreatPolicyChatParameter(p->data);
		auto_delete<CHAT_S2C::PolicyChatParameter> autoDeletePolicyChatPara(pPolicyChatPara);
		if ( !pPolicyChatPara && p->data.size() > 0 ){
			strTemp = _AL("???");
		}else{
			if (pPolicyChatPara && !pPolicyChatPara->IsNameReady()){
				pPolicyChatPara->GetNameFromServer();
				m_aPendingProtocols.Add(pProtocol);
				return false;
			}
			strTemp = CECUIHelper::PolicySpecialCharReplace(strTemp, pPolicyChatPara);
			if (CanFormatCoordText(p)){
				strTemp = CECUIHelper::FormatCoordText(strTemp);
			}
		}
		int strLen = strTemp.GetLength();
		wcsncpy(szMsg, strTemp, strLen);
		szMsg[strLen] = 0;
	}
	else{
		AUI_ConvertChatString(strTemp, szMsg, false);
	}
	
	if (p->channel == GP_CHAT_BROADCAST || p->channel == GP_CHAT_SYSTEM ||
		!p->srcroleid)
	{
		if( p->channel == GP_CHAT_SYSTEM && p->srcroleid > 0 )
		{
			ACString strMsg = _AL("");
			switch(p->srcroleid)
			{
			case 1: //CMSG_BIDSTART
			case 2: //CMSG_BIDEND
			case 3: //CMSG_BATTLESTART
			case 4: //CMSG_BATTLEEND
			case 6: //CMSG_BONUSSEND
			case 7: //CMSG_SPECIAL
			{
				if (!bCalledagain)
				{
					AArray<int, int> pending;
					if(!OnBattleChatMessage(p, &pending))
					{
						if(pending.GetSize() > 0)
						{
							GetFactionInfo(pending.GetSize(), pending.GetData());
							m_aPendingProtocols.Add(pProtocol);
							return false;
						}
					}
				}
				else
				{
					if (!OnBattleChatMessage(p, NULL))
					{
						return false;
					}
				}
				return true;
			}

			case 18: // CMSG_SYSAUCTION_FORENOTICE
			case 19: // CMSG_SYSAUCTION_START
			case 20: // CMSG_SYSAUCTION_BID
			case 21: // CMSG_SYSAUCTION_BIDINFO
			case 22: // CMSG_SYSAUCTION_END
			{
				pGameUI->AddSysAuctionMessage(p->srcroleid, p->msg.begin(), p->msg.size());
				return true;
			}
			case 24:
			{
				OnTaskChatMessage(p->msg.begin(), p->msg.size());
				return true;
			}

			case 29:		//	CMSG_FF_CHALLENGESTART
			case 30:		//	CMSG_FF_BATTLESTART
			case 31:		//	CMSG_FF_CREATE	������ɻ��ش���
			case 32:		//	CMSG_FF_HEALTHNOTIFY	���ɻ��ؽ�����
			case 33:		//	CMSG_FF_BECHALLENGED	���ɻ��ر���İ��ɳɹ���ս
			case 34:		//	CMSG_FF_CHALLENGESUCCESS	���ɻ������İ��ɳɹ���ս
			case 35:		//	CMSG_FF_BATTLESTARTNOTIFY	���ɻ���ս����ʼ
			case 36:		//	CMSG_FF_BATTLEENDNOTIFY	���ɻ���ս������
			case 37:		//	CMSG_FF_LEVELUP	���ɻ�������
			case 38:		//	CMSG_FF_TECHNOLOGYUP	���ɻ��ؿƼ�����
			case 39:		//	CMSG_FF_CONSTRUCT	���ɻ�����ʩ��ʼ����
			case 40:		//	CMSG_FF_CONSTRUCTCOMPLETE	���ɻ�����ʩ�������
			case 41:		//	CMSG_FF_HANDINCONTRIB	�����Ͻ��ɹ��׶�
			case 42:		//	CMSG_FF_HANDINMATERIAL	�����Ͻɲ���
			case 43:		//	CMSG_FF_BATTLEPREPARECLEAR	���ɻ���ս��ǰ�峡
			case 45:		//	CMSG_FF_DISMANTLE	���ɻ�����ʩ���
			{
				if (!bCalledagain)
				{
					AArray<int, int> pending;
					if (!OnFortressChatMessage(p, &pending))
					{
						if(pending.GetSize() > 0)
						{
							GetFactionInfo(pending.GetSize(), pending.GetData());
							m_aPendingProtocols.Add(pProtocol);
							//bAddToOld = false;
							return false;
						}
					}
				}
				else
				{
					if (!OnFortressChatMessage(p, NULL))
					{
						return false;
					}
				}
				break;
			}

			case 46:		//	CMSG_COUNTRYBATTLE_ATTACK	���������������
			case 47:		//	CMSG_COUNTRYBATTLE_DEFEND	�����������ҵĽ���
			case 48:		//	CMSG_COUNTRYBATTLE_WIN		Ӯ��һ����սս��ʤ��
			case 49:		//	CMSG_COUNTRYBATTLE_LOSE		һ����սս��ʧ��
				OnCountryChatMessage(p);
				break;

			case 50:		//  CMSG_COUNTRYBATTLE_KING_ASSIGN_ASSAULT		��ս����ʹ�ü�Ԯ��
			case 51:		//  CMSG_COUNTRYBATTLE_KING_SET_BATTLE_LIMIT	��ս��������ս������
			case 52:		//  CMSG_KE_CANDIDATE_APPLY_ANNOUNCE			������Ҿ��Ĺ���
			case 53:		//  CMSG_KE_CANDIDATE_APPLY_START				���Ĺ�����ʼ
			case 54:		//  CMSG_KE_CANDIDATE_APPLY_END					���Ĺ�������
			case 55:		//  CMSG_KE_VOTE_ANNOUNCE						����ѡ�ٹ���
			case 56:		//  CMSG_KE_VOTE_START							ѡ�ٹ�����ʼ
			case 57:		//  CMSG_KE_VOTE_END							ѡ�ٹ�������
			case 58:		//  CMSG_KE_CANDIDATE_VOTES100					���ѡƱ����100
			case 59:		//  CMSG_KE_CANDIDATE_VOTES_TOP					ѡ�ٹ������
				if (!bCalledagain)
				{
					if( !OnKingChatMessage(p) )
					{
						m_aPendingProtocols.Add(p);
						return false;
					}
				}
				else
				{
					if ( !OnKingChatMessage(p, false))
					{
						return false;
					}
				}
				break;

#ifdef RESOURCE_BATTLE
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
				OnFactionPVPChatMessage(p);
				break;
#endif				
			case 100:
			{
				int id1, id2;
				Octets name1, name2;
				try{ 
                    ACString strName1, strName2;
					Marshal::OctetsStream(p->msg)>>id1>>id2>>name1>>name2;
                    strName1 = ACString((const ACHAR*)name1.begin(), name1.size() / sizeof(ACHAR));
                    strName2 = ACString((const ACHAR*)name2.begin(), name2.size() / sizeof(ACHAR));
					strMsg.Format(pGameUI->GetStringFromTable(4000 + p->srcroleid), strName1, strName2);
                    g_pGame->GetGameRun()->AddPlayerName(id1, strName1);
                    g_pGame->GetGameRun()->AddPlayerName(id2, strName2);
				}catch(...) {}
				break;
			}
			}
			if( strMsg != _AL("") )
				g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, p->srcroleid, NULL, 1, p->emotion);
		}
		else
			g_pGame->GetGameRun()->AddChatMessage(szMsg, p->channel, p->srcroleid, NULL, 0, p->emotion);
	}
    else if( p->channel == GP_CHAT_INSTANCE && p->srcroleid == 1 )
    {
        AUICTranslate trans;
        g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->AddHeartBeatHint(trans.Translate(szMsg ));
    }
	else
	{
		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

		if (ISPLAYERID(p->srcroleid))
		{
			//	Try to get player's name
			const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(p->srcroleid, false);
			if (!szName )
			{
				//	Add protocol to waiting list
				if (!bCalledagain)
				{
					m_aPendingProtocols.Add(pProtocol);
					AddChatPlayerID(p->srcroleid);
				}
				return false;
			}
			else
			{				
				ACHAR szText[80];
				AUI_ConvertChatString(szName, szText);

				ACString str;
				str.Format(pStrTab->GetWideString(FIXMSG_CHAT), szText, szMsg);
				g_pGame->GetGameRun()->AddChatMessage(str, p->channel, p->srcroleid, NULL, 0, p->emotion, pItem ? pItem->Clone() : NULL,strMsgOrigion);

				//	Set player's last said words
				CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(p->srcroleid);
				if (pPlayer)
				{
					if (p->channel == GP_CHAT_SUPERFARCRY || p->channel == GP_CHAT_GLOBAL)
						strTemp.CutRight(8);  // ɾ�����ӵ���ɫ�������Ϣ
					pPlayer->SetLastSaidWords(strTemp, p->emotion, pItem);
				}
			}
		}
		else if (ISNPCID(p->srcroleid))
		{
			CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(p->srcroleid);

			if (pNPC)
			{
				ACString str;
				str.Format(pStrTab->GetWideString(FIXMSG_CHAT2), pNPC->GetName(), szMsg);
				g_pGame->GetGameRun()->AddChatMessage(str, p->channel, p->srcroleid, NULL, 0, p->emotion);

				CECUIHelper::RemoveNameFlagFromNPCChat(strTemp, szMsg);
				pNPC->SetLastSaidWords(szMsg);
			}
		}
	}
	return true;
}

static bool IsPolicyChat(const GNET::WorldChat *p)
{
	//	Э�������Ƿ�֧�ִ�NPC�������������
	bool bOK(false);
	switch (p->channel)
	{
	case GP_CHAT_FARCRY:			//	����ǰ��ͼȫ����ң����Ժ����б�� $B ��
		if (p->roleid == 0){
			bOK = true;
		}
		break;
	case GP_CHAT_BROADCAST:			//	ϵͳ��������ȫ��������ң����Ժ����б�� $S��
		if (p->roleid == 0){
			bOK = true;
		}
		break;
	}
	return bOK;
}

bool CECGameSession::OnPrtcWorldChat(GNET::Protocol* pProtocol, bool bCallagain)
{
	using namespace GNET;

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	WorldChat* p = (WorldChat*)pProtocol;

	bool result = true;

	if (p->channel == GP_CHAT_FARCRY && p->roleid == 24)
	{
		OnTaskChatMessage(p->msg.begin(), p->msg.size());
		return result;
	}

	if (p->channel == GP_CHAT_SYSTEM )
	{
		ACString strMsg = _AL("");

		switch(p->roleid)
		{
		case 3: //CMSG_BATTLESTART
		case 4: //CMSG_BATTLEEND
		case 5: //CMSG_BATTLEBID
			{
				if(!bCallagain){
					AArray<int, int> pending;
					if(!OnBattleChatMessage(p, &pending)){
						if(pending.GetSize() > 0){
							GetFactionInfo(pending.GetSize(), pending.GetData());
							m_aPendingProtocols.Add(pProtocol);
							result = false;
						}
					}
				}else{
					if (!OnBattleChatMessage(p, NULL)){
						result = false;
					}
				}
				break;
			}

		case 24:
			{
				OnTaskChatMessage(p->msg.begin(), p->msg.size());
				break;
			}

		case 25:		//	CMSG_FACTION_BECAME_ALLIANCE
		case 26:		//	CMSG_FACTION_REMOVE_ALLIANCE
		case 27:		//	CMSG_FACTION_BECAME_HOSTILE
		case 28:		//	CMSG_FACTION_REMOVE_HOSTILE
			OnFactionDiplomacyMessage(p);
			break;

		case 44:		//	CMSG_FF_KEYBUILDINGDESTROY
			{
				if (!bCallagain){
					AArray<int, int> pending;
					if(!OnFortressChatMessage(p, &pending))
					{
						if(pending.GetSize() > 0)
						{
							GetFactionInfo(pending.GetSize(), pending.GetData());
							m_aPendingProtocols.Add(pProtocol);
							result = false;
						}
					}
				}else{
					if (!OnFortressChatMessage(p, NULL)){
						result = false;
					}
				}
				break;
			}

		case 100:
			{
				const int iNameLen = 20;				
#pragma pack(1)
				struct world_chat_content
				{
					int item_id;
					wchar_t szName[iNameLen];
					int Item_proc_type;
					int Item_expire_date;
					size_t Item_content_length;
				};
#pragma pack()

				if( p->msg.size() >= sizeof(world_chat_content))
				{
					world_chat_content pWorldChatContent;
					BYTE* pData = (BYTE* )p->msg.begin();
					pWorldChatContent.item_id = *((int*)pData);
					pData += sizeof(int);

					wcsncpy(pWorldChatContent.szName, (const wchar_t*)((int*)pData), iNameLen);
					ACString itemName(pWorldChatContent.szName, iNameLen);
					pData += sizeof(pWorldChatContent.szName);

					//��Ʒ�������
					pWorldChatContent.Item_proc_type = *((int*)pData);
					pData += sizeof(int);
					pWorldChatContent.Item_expire_date = *((int*)pData);
					pData += sizeof(int);
					pWorldChatContent.Item_content_length = *((size_t*)pData);
					pData += sizeof(size_t);

					char* Item_content = (char*)malloc(pWorldChatContent.Item_content_length);
					if ( pWorldChatContent.Item_content_length >= 0 )
					{
						char * sre_Item_content = (char*)pData;
						memcpy(Item_content, sre_Item_content, pWorldChatContent.Item_content_length);
					}

					CECIvtrItem *pItem = CECIvtrItem::CreateItem(pWorldChatContent.item_id, pWorldChatContent.Item_expire_date, 1);
					if( pItem )
					{
						if(pWorldChatContent.Item_content_length > 0 )
						{
							pItem->SetItemInfo((BYTE*)Item_content, pWorldChatContent.Item_content_length);
						}
						ACString itemLinkString = CECUIHelper::MarshalLinkedItem(pItem, false);
						
						BROADCASTMAP::iterator iterMap = l_mapBroadcast.find(pWorldChatContent.item_id);
						bool bShowItem = false;
						if (iterMap != l_mapBroadcast.end())
						{
							switch((iterMap->second).iIsItemNeedDisplay)
							{
							case 0:// �����ļ��3: ����ʾ��Ʒ������ʾ�������2:����ʾ���������ʾ��Ʒ����1����ʾ�������0��������ʾ
								strMsg = pGameUI->GetStringFromTable((iterMap->second).iTextID);
								break;
							case 1:
								strMsg.Format(pGameUI->GetStringFromTable((iterMap->second).iTextID),itemName);
								break;
							case 2:
								strMsg.Format(pGameUI->GetStringFromTable((iterMap->second).iTextID),itemName,itemLinkString);
								bShowItem = true;
								break;
							case 3:
								strMsg.Format(pGameUI->GetStringFromTable((iterMap->second).iTextID),itemLinkString,itemName);
								bShowItem = true;
								break;
							default:
								break;
							}
						}
						else
						{
							strMsg.Format(pGameUI->GetStringFromTable(4520), itemName, itemLinkString);
							bShowItem = true;
						}
						//�޸�strMsg���滻�����ַ�����Ʒ����						
						//��Ҫ��GameUI������Ʒ��Ϣ
						if (bShowItem)
						{
							g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1, p->emotion, pItem ? pItem->Clone() : NULL);
							delete pItem;
							return result;
						}
						delete pItem;
					}
					if(Item_content != NULL)
						free(Item_content);
                    // ȡ��ҵ�ID
                    GetPlayerIDByName(itemName, 0);
				}
				break;
			}
		case 101:
			{
				if (!bCallagain){
					int role_id(0);
					if (!OnEarthBagMessage(p, &role_id))
					{
						m_aPendingProtocols.Add(pProtocol);
						CacheGetPlayerBriefInfo(1, &role_id, 1);
						result = false;
					}
				}
				else{
					if (!OnEarthBagMessage(p)){
						result = false;
					}
				}
				break;
			}
		case 102:
			{
				const int iNameLen = 20;
				
				if( p->msg.size() >= sizeof(unsigned short) + sizeof(wchar_t) * iNameLen)
				{
					unsigned short id = *((unsigned short*)p->msg.begin());
					
					wchar_t szName[iNameLen+1];
					szName[iNameLen] = 0;
					wcsncpy(szName, (const wchar_t*)((unsigned short*)p->msg.begin()+1), iNameLen);
					
					const TITLE_CONFIG* pTitle = CECPlayer::GetTitleConfig(id);
					if( pTitle )
					{
						strMsg.Format(pGameUI->GetStringFromTable(10610), szName, pTitle->name);
					}
				}
				break;
			}
		case 103:
			{
				OnFactionRenameMessage(
					ACString((const ACHAR*)p->msg.begin(), p->msg.size() / sizeof(ACHAR)),
					ACString((const ACHAR*)p->data.begin(), p->data.size() / sizeof(ACHAR)));
				break;
			}
		case 106:
		case 107:
			{
			const int iNameLen = 20;
#pragma pack(1)
				struct _data
				{
					int tid;
					wchar_t name[iNameLen];
					int roleid;
					wchar_t name_target[iNameLen];
				};
#pragma pack()

				if (p->data.size() == sizeof(_data)) {
					_data fireworkData;
					BYTE* pData = (BYTE*)p->data.begin();

					fireworkData.tid = *((int*)pData);
					pData += sizeof(int);

					wcsncpy(fireworkData.name, (const wchar_t*)((int*)pData), iNameLen);
					pData += sizeof(fireworkData.name);

					fireworkData.roleid = *((int*)pData);
					pData += sizeof(int);

					wcsncpy(fireworkData.name_target, (const wchar_t*)((int*)pData), iNameLen);
					pData += sizeof(fireworkData.name_target);

					DATA_TYPE DataType;
					FIREWORKS2_ESSENCE* pfireworkess = (FIREWORKS2_ESSENCE*) g_pGame->GetElementDataMan()->get_data_ptr(fireworkData.tid, ID_SPACE_ESSENCE, DataType);
					if (DataType != DT_FIREWORKS2_ESSENCE) break;

					if (pfireworkess) {
						wchar_t szName[256];
						wcsncpy(szName, (wchar_t*)pfireworkess->caster_word, 256);
						ACString fireworkMsg(szName, 256);
						

						int nSub = fireworkMsg.Find(_AL("&name1&"));
						if (nSub != -1)
						{
							fireworkMsg = fireworkMsg.Left(nSub) + _AL("&") + fireworkData.name + _AL("&") + fireworkMsg.Right(fireworkMsg.GetLength() - nSub - 7);
						}

						nSub = fireworkMsg.Find(_AL("&name2&"));
						if (nSub != -1)
						{
							fireworkMsg = fireworkMsg.Left(nSub) + _AL("&") + fireworkData.name_target + _AL("&") + fireworkMsg.Right(fireworkMsg.GetLength() - nSub - 7);
						}

						if (p->roleid == 106)
							g_pGame->GetGameRun()->AddChatMessage(fireworkMsg, GP_CHAT_BROADCAST, 0, NULL, 0, p->emotion);
						else {
							ACString myPlayername = g_pGame->GetGameRun()->GetPlayerName(m_iCharID, false);
							if (myPlayername.Compare(fireworkData.name_target) == 0)
								g_pGame->GetGameRun()->AddChatMessage(fireworkMsg, GP_CHAT_WHISPER, fireworkData.roleid, NULL, 1, p->emotion);
						}
							
						return result;
					}


				}

			break;
			}
		}
		if( strMsg != _AL("") )
			g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, 0, NULL, 1, p->emotion);
		return result;
	}

	// ����Э����LINK����Ʒ
	CECIvtrItem* pItem = CHAT_S2C::CreateChatItem(p->data);
	auto_delete<CECIvtrItem> tmp(pItem);

	ACHAR szMsg[1024];
	ACString strTemp((const ACHAR*)p->msg.begin(), p->msg.size() / sizeof (ACHAR));
	ACString strMsgOrigion = strTemp;// ԭʼ������Ϣ
	strTemp = pGameUI->FilterInvalidTags(strTemp, pItem==NULL);
	if (IsPolicyChat(p)){ //���Ժ�������
		CHAT_S2C::PolicyChatParameter* pPolicyChatPara = CHAT_S2C::CreatPolicyChatParameter(p->data);
		auto_delete<CHAT_S2C::PolicyChatParameter> autoDeletePolicyChatPara(pPolicyChatPara);
		if ( !pPolicyChatPara && p->data.size() > 0 ){
			strTemp = _AL("???");
		}else{
			if (pPolicyChatPara && !pPolicyChatPara->IsNameReady()){
				pPolicyChatPara->GetNameFromServer();
				m_aPendingProtocols.Add(pProtocol);
				return false;
			}
			strTemp = CECUIHelper::PolicySpecialCharReplace(strTemp, pPolicyChatPara);
			strTemp = CECUIHelper::FormatCoordText(strTemp);
		}
		int strLen = strTemp.GetLength();
		wcsncpy(szMsg, strTemp, strLen);
		szMsg[strLen] = 0;
	}else{	
		AUI_ConvertChatString(strTemp, szMsg, false);
	}

	if (!p->roleid)
	{
		g_pGame->GetGameRun()->AddChatMessage(szMsg, p->channel, p->roleid, NULL, 0, p->emotion);
	}
	else
	{
		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

		ACHAR szText[80];
		ACString strName((const ACHAR*)p->name.begin(), p->name.size() / sizeof (ACHAR));
		AUI_ConvertChatString(strName, szText);

		ACString str;
		bool ok(false);
		if (p->channel == GP_CHAT_SUPERFARCRY || p->channel == GP_CHAT_GLOBAL)
		{
			// �Ž�Ƶ����Ϣ�и��Ӷ�����Ϣ����ɫ

			ACString strTemp = szMsg;
			int nLen = strTemp.GetLength();
			int nAddInfoLen = 8;
			if (nLen >= nAddInfoLen)
			{
				ACString strColorEmotion = strTemp.Right(nAddInfoLen);

				while (true)
				{
					ACString strEmotion = strColorEmotion.Right(2);
					strEmotion.MakeLower();

					int nEmotion(-1);
					if ((strEmotion[0]>='0' && strEmotion[0]<='9' ||
						strEmotion[0]>='a' && strEmotion[0]<='f') &&
						(strEmotion[1]>='0' && strEmotion[1]<='9' ||
						strEmotion[1]>='a' && strEmotion[1]<='f'))
					{
						a_sscanf(strEmotion, _AL("%x"), &nEmotion);
					}
					if (nEmotion<0)
						break;
					
					ACString strColor = _AL("^");
					strColor += strColorEmotion.Left(nAddInfoLen-strEmotion.GetLength());
					
					A3DCOLOR clr;					
					if (!STRING_TO_A3DCOLOR(strColor, clr))
						break;

					strTemp.CutRight(nAddInfoLen);
					CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
					if (p->channel == GP_CHAT_GLOBAL) {
						str.Format(
							pGameUIMan->GetStringFromTable(8571),
							szText,
							L"[Evolved]", // This should be populated with cross-server name prefix
							pGameUIMan->GetStringFromTable(8540 + nEmotion),
							strColor,
							strTemp);
					}
					else {
						str.Format(
							pGameUIMan->GetStringFromTable(8570),
							szText,
							pGameUIMan->GetStringFromTable(8540 + nEmotion),
							strColor,
							strTemp);
					}
					ok = true;
					break;
				}
			}
		}
		if (!ok)
			str.Format(pStrTab->GetWideString(FIXMSG_CHAT), szText, szMsg);
		g_pGame->GetGameRun()->AddChatMessage(str, p->channel, p->roleid, NULL, 0, p->emotion, pItem,strMsgOrigion);
		tmp._ptr = NULL;

		if (ISPLAYERID(p->roleid))
			g_pGame->GetGameRun()->AddPlayerName(p->roleid, strName);
	}
	return result;
}

void CECGameSession::OnPrtcErrorInfo(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	ErrorInfo* p = (ErrorInfo*)pProtocol;
	OutputLinkSevError(p->errcode);

/*	if( p->errcode == ERR_INVALID_PASSWORD ||
		p->errcode == ERR_COMMUNICATION ||
		p->errcode == ERR_ACCOUNTLOCKED ||
		p->errcode == ERR_MULTILOGIN ||
		p->errcode == ERR_LINKISFULL)
*/	{
		Close();

		if (g_pGame->GetGameRun()->GetLogoutFlag() == 1)
		{
			a_LogOutput(1, "CECGameSession::OnPrtcErrorInfo, LogoutFlag=1 replaced by 2.");
			g_pGame->GetGameRun()->SetLogoutFlag(2);
		}
	}
}

void CECGameSession::OnPrtcPlayerLogout(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	PlayerLogout* p = (PlayerLogout*)pProtocol;

	m_CmdCache.RemoveAllCmds();
	
	int iFlag;
	switch (p->result)
	{
	case _PLAYER_LOGOUT_FULL:	iFlag = 0;	break;
	case _PLAYER_LOGOUT_HALF:	iFlag = 1;	break;
	default: 					iFlag = 2;	break;
	}

	g_pGame->GetGameRun()->SetLogoutFlag(iFlag);

	if (!IsConnected() && g_pGame->GetGameRun()->GetLogoutFlag() == 1)
	{
		a_LogOutput(1, "CECGameSession::OnPrtcPlayerLogout, LogoutFlag=1 replaced by 2.");
		g_pGame->GetGameRun()->SetLogoutFlag(2);
	}
}

void CECGameSession::OnPrtcPrivateChat(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	PrivateChat* p = (PrivateChat*)pProtocol;
	ASSERT(p->dstroleid == m_iCharID);

	int levelBlock = g_pGame->GetConfigs()->GetBlackListSettings().levelBlock;
	if(p->src_level > 0 && p->src_level < levelBlock)
	{
		if(p->channel == CHANNEL_NORMAL || p->channel == CHANNEL_NORMALRE)
		{
			if(!g_pGame->GetGameRun()->GetHostPlayer()->IsOmitBlocking(p->srcroleid))
			{
				// should be filted by level
				return;
			}
		}
	}

	//	�����յ�����ҷ���Ƶ�������ڻظ�����GT��Ƶ���ķ���ʱ����Ҫ��ѯ����Ϣ��
	//	��AddChatMessage ʱ���������Զ��ظ�����Ҫ��ѯƵ���������Ҫ����֮ǰ��
	if (p->channel == CHANNEL_NORMAL || p->channel == CHANNEL_NORMALRE ||
		p->channel == CHANNEL_FRIEND || p->channel == CHANNEL_FRIEND_RE ||
		p->channel == CHANNEL_GAMETALK)
	{
		CECGameTalk::Instance().Set(p->srcroleid, (GNET::PRIVATE_CHANNEL)p->channel);
	}

	// ����Э����LINK����Ʒ
	CECIvtrItem* pItem = CHAT_S2C::CreateChatItem(p->data);
	auto_delete<CECIvtrItem> tmp(pItem);

	ACString strMsg((const ACHAR*)p->msg.begin(), p->msg.size() / sizeof (ACHAR));	
	ACString strMsgOrigion = strMsg;// ԭʼ������Ϣ
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	strMsg = pGameUI->FilterInvalidTags(strMsg, pItem==NULL);
	ACString strSrcName((const ACHAR*)p->src_name.begin(), p->src_name.size() / sizeof (ACHAR));	

	if (p->channel == CHANNEL_NORMAL || p->channel == CHANNEL_NORMALRE)
	{
		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

		ACString str;
		ACHAR szText[80], szText1[1024];
		AUI_ConvertChatString(strSrcName, szText);
		AUI_ConvertChatString(strMsg, szText1, false);

		str.Format(pStrTab->GetWideString(FIXMSG_PRIVATECHAT1), szText, szText1);
		g_pGame->GetGameRun()->AddChatMessage(str, GP_CHAT_WHISPER, p->srcroleid, strSrcName, p->channel, p->emotion, pItem,strMsgOrigion);
		tmp._ptr = NULL;
	}
	else
	{
		g_pGame->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_WHISPER, p->srcroleid, strSrcName, p->channel, p->emotion, pItem,strMsgOrigion);
		tmp._ptr = NULL;
	}

	//	Save player name.
	g_pGame->GetGameRun()->AddPlayerName(p->srcroleid, strSrcName);

	//  Add to related people
	g_pGame->GetGameRun()->GetHostPlayer()->AddRelatedPlayer(p->srcroleid);
}

void CECGameSession::OnPrtcPlayerBaseInfoRe(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_PM_PLAYERBASEINFO, MAN_PLAYER, -1, (DWORD)pProtocol);
}

void CECGameSession::OnPrtcGetConfigRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetUIConfig_Re* p = (GetUIConfig_Re*)pProtocol;
	if( p->result != ERR_SUCCESS )
		a_LogOutput(1, "CECGameSession::OnPrtcGetConfigRe, link return error code of %d", p->result);
	else
	{
		if(!g_pGame->GetGameRun()->LoadConfigsFromServer(p->ui_config.begin(), p->ui_config.size()))
		{
			// if load failed then use current setting directly
			g_pGame->GetConfigs()->ApplyUserSetting();
		}

		//	Now, Get config data request is sent after all host initial data ready. 
		//		so when we receive this reply, we can do some last work before game
		//		really starts. Maybe it's not the best place to do these work, but
		//		now we do it here.
		//	Enalbe game UI
		CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI)
		{
			pGameUI->EnableUI(true);
			
			// Get referral name for adding friend or other display
			RoleInfo info = g_pGame->GetGameRun()->GetSelectedRoleInfo();
			if (info.referrer_role > 0)
				GetPlayerBriefInfo(1, &info.referrer_role, 2);
		}

		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		pHost->OnAllInitDataReady();

		if (pHost->IsGM())
		{
			CDlgCountryMap* pDlgCountryMap = (CDlgCountryMap*)pGameUI->GetDialog("Win_CountryMap");
			pDlgCountryMap->GetConfig();
		}

		g_pGame->GetConfigs()->ApplyOptimizeSetting();

		if( g_pGame->GetConfigs()->IsMiniClient() )
			CECMCDownload::GetInstance().SendGetDownloadOK();

#ifdef LOGIC_CHECK
		if (g_pGame->GetConfigs()->IsSendLogicCheckInfo()){
			//	����������ͻ���ʶ����
			LogicHelp::LogicCheck logicChecker;
			unsigned char buffer[1024] = {0};
			int actualSize = logicChecker.ReceiveMessage(buffer, sizeof(buffer)/sizeof(buffer[0]));
			if (actualSize <= 0){
#ifdef LOG_PROTOCOL
				a_LogOutput(1, "LogicHelp::LogicCheck ReceiveMessage returns %d", actualSize);
#endif
			}else{
#ifdef LOG_PROTOCOL
				a_LogOutput(1, "LogicHelp::LogicCheck ReceiveMessage returns %d bytes:%s", actualSize, glb_FormatOctets(Octets(buffer, actualSize)));
#endif
				sendClientMachineInfo(buffer, actualSize);
			}
		}
#endif
	}
}

void CECGameSession::OnPrtcSetConfigRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	SetUIConfig_Re* p = (SetUIConfig_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
		a_LogOutput(1, "CECGameSession::OnPrtcSetConfigRe, link return error code of %d", p->result);

	if (g_pGame->GetGameRun())
	{
		g_pGame->GetGameRun()->GetPendingLogOut().TriggerAll();
		g_pGame->GetGameRun()->GetPendingLogOut().Clear();
	}
}

void CECGameSession::OnPrtcGetHelpStatesRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetHelpStates_Re* p = (GetHelpStates_Re*)pProtocol;

	if( p->result != ERR_SUCCESS )
		a_LogOutput(1, "CECGameSession::OnPrtcGetHelpStatesRe, link return error code of %d", p->result);
	else
	{
		// set the data to the object that needs them.
		if( m_pfnHelpStatesCallBack )
		{	
			(*m_pfnHelpStatesCallBack)(p->help_states.begin(), p->help_states.size());
			m_pfnHelpStatesCallBack = NULL;
		}
	}
}

void CECGameSession::OnPrtcSetHelpStatesRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	SetHelpStates_Re* p = (SetHelpStates_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
		a_LogOutput(1, "CECGameSession::OnPrtcSetHelpStatesRe, link return error code of %d", p->result);
}

void CECGameSession::OnPrtcSetCustomDataRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	SetCustomData_Re* p = (SetCustomData_Re*)pProtocol;
	CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
	int gs = g_pGame->GetGameRun()->GetGameState();

	// the message handler will restore the customize data if failed
	if (gs == CECGameRun::GS_LOGIN)
	{
		pUIMan->GetLoginUIMan()->ChangeCustomizeData(p->roleid, p->result);
	}
	else if (gs == CECGameRun::GS_GAME)
	{
		g_pGame->GetGameRun()->PostMessage(MSG_HST_CHANGEFACE, MAN_PLAYER, 0, (DWORD)pProtocol, S2C::PROTOCOL_COMMAND, PROTOCOL_SETCUSTOMDATA_RE);
	}

	// show the error message
	if (p->result != ERR_SUCCESS)
	{
		pUIMan->ShowErrorMsg(g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_WRONGCUSTOMDATA));
		a_LogOutput(1, "CECGameSession::OnPrtcSetCustomDataRe, Failed to set custom data(result=%d)", p->result);
	}
}

void CECGameSession::OnPrtcGetCustomDataRe(GNET::Protocol* pProtocol)
{
	g_pGame->GetGameRun()->PostMessage(MSG_PM_PLAYERCUSTOM, MAN_PLAYER, -1, (DWORD)pProtocol);
}

void CECGameSession::OnPrtcGetPlayerBriefInfoRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetPlayerBriefInfo_Re* p = (GetPlayerBriefInfo_Re*)pProtocol;

	if (p->retcode != ERR_SUCCESS)
		return;

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	switch (p->reason)
	{
	case 0:		//	We try to get host player's team member's information.
	{
		CECTeam* pTeam = g_pGame->GetGameRun()->GetHostPlayer()->GetTeam();
		if (pTeam)
			pTeam->UpdateMemberInfo(*p);

		break;
	}
	case 1:		//	We try to get someone's name who just said something.
	{
		ASSERT(m_aPendingProtocols.GetSize());

		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
		ACString str;

		//	Update host's team member information
		for (int i=0; i < (int)p->playerlist.size(); i++)
		{
			const PlayerBriefInfo& Info = p->playerlist[i];
			const ACHAR* szName = NULL;
			ACString strName;
			ACHAR szText[80], szText1[1024];

			if (!(szName = pGameRun->GetPlayerName(Info.roleid, false)))
			{
				strName = ACString((const ACHAR*)Info.name.begin(), Info.name.size() / sizeof (ACHAR));
				szName	= strName;
				pGameRun->AddPlayerName(Info.roleid, strName);
			}

			for (int j=0; j < m_aPendingProtocols.GetSize(); j++)
			{
				Protocol* pProtocol = m_aPendingProtocols[j];

				bool bProcessed(false);
				switch (pProtocol->GetType())
				{
					case PROTOCOL_FACTIONCHAT:
					{
						FactionChat* pChat = static_cast<FactionChat*>(pProtocol);

						if (pChat->src_roleid != Info.roleid)
							continue;

						ACString strMsg;
						_cp_str(strMsg, pChat->msg.begin(), pChat->msg.size());
						ACString strMsgOrigion = strMsg;// ԭʼ������Ϣ
						
						CECIvtrItem* pItem = CHAT_S2C::CreateChatItem(pChat->data);
						CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
						strMsg = pGameUI->FilterInvalidTags(strMsg, pItem==NULL);

						AUI_ConvertChatString(szName, szText);
						AUI_ConvertChatString(strMsg, szText1, false);

						ACString str;
						str.Format(pStrTab->GetWideString(FIXMSG_CHAT), szText, szText1);
						pGameRun->AddChatMessage(str, GP_CHAT_FACTION, pChat->src_roleid, szName, 0, pChat->emotion, pItem,strMsgOrigion);

						bProcessed = true;
						break;
					}

					case PROTOCOL_FACTIONACCEPTJOIN_RE:
					{
						FactionAcceptJoin_Re* p = static_cast<FactionAcceptJoin_Re*>(pProtocol);

						if (Info.roleid != p->newmember && Info.roleid != p->operater)
							continue;

						const ACHAR* szOperator = NULL;
						const ACHAR* szOperant = NULL;

						if (p->newmember == Info.roleid)
						{
							g_pGame->GetFactionMan()->AddNewMember(p->newmember, szName, p->level, Info.occupation, p->gender, p->reputation, p->reincarn_times);							
							szOperant = szName;
							szOperator = pGameRun->GetPlayerName(p->operater, false);
						}
						else
						{
							szOperator = szName;
							szOperant = pGameRun->GetPlayerName(p->newmember, false);
						}

						if (szOperator && szOperant)
						{
							g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_ACCEPT_JOIN, szOperator, szOperant);
							bProcessed = true;
						}

						break;
					}

					case PROTOCOL_FACTIONRESIGN_RE:
					{
						FactionResign_Re* p = static_cast<FactionResign_Re*>(pProtocol);

						if (p->resigned_role != Info.roleid)
							continue;

						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RESIGN, szName, CECFactionMan::GetFRoleName(p->old_occup));
						bProcessed = true;
						break;
					}

					case PROTOCOL_FACTIONRENAME_RE:
					{
						FactionRename_Re* p = static_cast<FactionRename_Re*>(pProtocol);

						if (p->renamed_roleid != Info.roleid && p->operater != Info.roleid)
							continue;

						const ACHAR* szOperator = pGameRun->GetPlayerName(p->operater, false);
						const ACHAR* szOperant = pGameRun->GetPlayerName(p->renamed_roleid, false);

						if (szOperator && szOperant)
						{
							ACString strNickName;
							_cp_str(strNickName, p->new_name.begin(), p->new_name.size());
							g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_RENAME, szOperator, szOperant, strNickName);
							bProcessed = true;
						}

						break;
					}
					
					case PROTOCOL_FACTIONMASTERRESIGN_RE:
					{
						FactionMasterResign_Re* p = static_cast<FactionMasterResign_Re*>(pProtocol);

						if (p->newmaster != Info.roleid)
							continue;

						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_APPOINT, _AL(""), szName, CECFactionMan::GetFRoleName(_R_MASTER));
						bProcessed = true;
						break;
					}

					case PROTOCOL_FACTIONLEAVE_RE:
					{
						FactionLeave_Re* p = static_cast<FactionLeave_Re*>(pProtocol);

						if (p->leaved_role != Info.roleid)
							continue;

						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_LEAVE, szName);
						bProcessed = true;
						break;
					}

					case PROTOCOL_FACTIONEXPEL_RE:
					{
						FactionExpel_Re* p = static_cast<FactionExpel_Re*>(pProtocol);

						if (static_cast<int>(p->expelroleid) != Info.roleid && p->operater != Info.roleid)
							continue;

						const ACHAR* szOperator = pGameRun->GetPlayerName(p->operater, false);
						const ACHAR* szOperant = pGameRun->GetPlayerName(p->expelroleid, false);

						if (szOperator && szOperant)
						{
							g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_EXPEL, szOperator, szOperant);
							bProcessed = true;
						}
						
						break;
					}
					
					case PROTOCOL_FACTIONDELAYEXPELANNOUNCE:
						{
							FactionDelayExpelAnnounce* p = static_cast<FactionDelayExpelAnnounce*>(pProtocol);
							
							if (p->expelroleid != Info.roleid && p->operater != Info.roleid)
								continue;
							
							if (!g_pGame->GetFactionMan()->OnDelayExpelMessage(p))
							{
								bProcessed = true;
							}
							
							break;
						}

					case PROTOCOL_FACTIONAPPOINT_RE:
					{
						FactionAppoint_Re* p = static_cast<FactionAppoint_Re*>(pProtocol);

						if (p->dstroleid != Info.roleid && p->operater != Info.roleid)
							continue;

						const ACHAR* szOperator = pGameRun->GetPlayerName(p->operater, false);
						const ACHAR* szOperant = pGameRun->GetPlayerName(p->dstroleid, false);

						if (szOperator && szOperant)
						{
							g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_FC_APPOINT, szOperator, szOperant, CECFactionMan::GetFRoleName(p->newoccup));
							bProcessed = true;
						}

						break;
					}

					case PROTOCOL_CHATMESSAGE:
					{
						
						//��protocol��������ͳһ��OnPrtcChatMessage����
						bProcessed = OnPrtcChatMessage(pProtocol, true);
						break;
					}
					
					case PROTOCOL_WORLDCHAT:
					{
						WorldChat *pWorldChat = (WorldChat *)pProtocol;
						bProcessed = OnPrtcWorldChat(pWorldChat, true);
						break;
					}

					case PROTOCOL_PLAYERACCUSE_RE:
						if (OnReportPluginMessage(pProtocol)){
							bProcessed = true;
						}
						break;
				}
				if (bProcessed){
					pProtocol->Destroy();
					m_aPendingProtocols.RemoveAtQuickly(j);
					j--;
				}
			}
		}

		break;
	}
	case 2:		//	We just try to get players' names
	{
		for (int i=0; i < (int)p->playerlist.size(); i++)
		{
			//	Name-id pair got this time always be right, so we force to
			//	overwrite old pair
			const PlayerBriefInfo& Info = p->playerlist[i];
			ACString strName = ACString((const ACHAR*)Info.name.begin(), Info.name.size() / sizeof (ACHAR));
			pGameRun->AddPlayerName(Info.roleid, strName, true);
		}

		break;
	}
	case 3:		//	Get player's brief infomation to handle team invitation
	{
		CECHostPlayer* pHost = pGameRun->GetHostPlayer();
		if (!pHost)
			return;

		for (int i=0; i < (int)p->playerlist.size(); i++)
			pHost->TeamInvite(p->playerlist[i]);
		
		break;
	}
	default:
		ASSERT(0);
		break;
	}
}

void CECGameSession::OnPrtcGetPlayerIDByNameRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetPlayerIDByName_Re* p = (GetPlayerIDByName_Re*)pProtocol;

	if (p->retcode != ERR_SUCCESS)
	{
		bool bObsoleteName = (p->retcode == 433);	//	��ɫ�����ˣ���ѯ����������
		if( p->reason == 3)
		{
			CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			if (pGameUI)
			{
				CDlgMailWrite *pDlg = ((CDlgMailWrite *)pGameUI->GetDialog("Win_MailWrite"));
				if( pDlg )
					pDlg->SendMail(bObsoleteName ? -2 : -1);
			}
		}
		else if( p->reason == 4)
		{
			CDlgWebMyShop *pDlg = NULL;

			// handle invalid name response
			CECBaseUIMan* pUIMan = NULL;
			if (g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME)
			{
				pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				if (pUIMan)
				{
					pDlg = dynamic_cast<CDlgWebMyShop*>(pUIMan->GetDialog("Win_Webmyshop"));
				}
			}
			else if (g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_LOGIN)
			{
				pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
				if (pUIMan)
				{
					pDlg = dynamic_cast<CDlgWebMyShop*>(pUIMan->GetDialog("Win_Webroletrade"));
				}
			}
			
			if( pDlg )
			{
				ACString strName((const ACHAR*)p->rolename.begin(), p->rolename.size() / sizeof (ACHAR));
				pDlg->NotifyInvalidName(strName, bObsoleteName);
			}
		}
		else
		{
			g_pGame->GetGameRun()->AddFixedMessage(bObsoleteName ? FIXMSG_OBSOLETE_ROLENAME : FIXMSG_WRONGROLE);
		}

		return;
	}

	switch (p->reason)
	{
	case 0:		//	no special reason, just get it

		break;

	case 1:		//	for invite a player who is very far from us to a team

		c2s_CmdTeamInvite(p->roleid);
		break;
		
	case 2:		//	for invite a player who is very far from us to faction

		faction_accept_join(p->roleid);
		break;

	case 3:		//	for send mail to other players
	{
		CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI)
		{
			CDlgMailWrite *pDlg = ((CDlgMailWrite *)pGameUI->GetDialog("Win_MailWrite"));
			if( pDlg )
				pDlg->SendMail(p->roleid);
		}
		break;
	}

	case 4:		//	webtrade reason
		
		break;

	case 5:     //  �̳����ͺ���ȡ��ѯ����
	{
		CECGameUIMan* pGameUI = (CECGameUIMan*)g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI)
		{
			CDlgGivingFor *pDlg = ((CDlgGivingFor *)pGameUI->GetDialog("Win_GivingFor"));
			if( pDlg )
				pDlg->OnPrtcGetPlayerIDByNameRe(pProtocol);
		}
		break;
	}

	}

	//	Save ID and name pair. Name-id pair got this time always be right,
	//	so we force to overwrite old pair
	ACString strName((const ACHAR*)p->rolename.begin(), p->rolename.size() / sizeof (ACHAR));
	g_pGame->GetGameRun()->AddPlayerName(p->roleid, strName, true);
}

void CECGameSession::OnPrtcRoleStatusAnnounce(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	RoleStatusAnnounce* p = (RoleStatusAnnounce*)pProtocol;

	if (p->status == _STATUS_OFFLINE)
	{
		ACHAR szName[80];
		ACString strTemp((const ACHAR*)p->rolename.begin(), p->rolename.size() / sizeof (ACHAR));
		AUI_ConvertChatString(strTemp, szName);

		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_OFFLINE, szName);
	}
}

void CECGameSession::OnPrtcAnnounceForbidInfo(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	AnnounceForbidInfo* p = (AnnounceForbidInfo*)pProtocol;
	const GRoleForbid& rf = p->forbid;
	CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

	if (rf.type == Forbid::FBD_FORBID_SELLPTS)
	{
		//	Game points locking notify
		g_pGame->GetGameRun()->LockGamePoints(rf.time ? true : false);
		return;
	}

	ACString str1, str2, str3, str;

	//	Forbid type
	switch (rf.type)
	{
	case Forbid::FBD_FORBID_LOGIN:	str1 = pStrTab->GetWideString(FIXMSG_FORBIDLOGIN);	break;
	case Forbid::FBD_FORBID_TALK:	str1 = pStrTab->GetWideString(FIXMSG_FORBIDTALK);	break;
	case Forbid::FBD_FORBID_TRADE:	str1 = pStrTab->GetWideString(FIXMSG_FORBIDTRADE);	break;
	case Forbid::FBD_FORBID_SELL:	str1 = pStrTab->GetWideString(FIXMSG_FORBIDSELL);	break;
	}

	//	Forbid reason
	if (rf.reason.size())
	{
		str = ACString((const ACHAR*)rf.reason.begin(), rf.reason.size() / sizeof (ACHAR));
		str2.Format(pStrTab->GetWideString(FIXMSG_FORBIDREASON), str);
	}
	else
		str2 = pStrTab->GetWideString(FIXMSG_FORBIDREASON);

	//	Forbid time
	str3.Format(pStrTab->GetWideString(FIXMSG_FORBIDTIME), (rf.time + 59) / 60);

	str = str1 + _AL("\r") + str2 + _AL("\r") + str3;

	CECUIManager* pUIMan = g_pGame->GetGameRun()->GetUIManager();
	if (pUIMan)
		pUIMan->ShowErrorMsg(str);
}

void CECGameSession::OnPrtcUpdateRemainTime(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	UpdateRemainTime* p = (UpdateRemainTime*)pProtocol;
	m_iiAccountTime		= ((INT64)p->remain_time) * 1000;
	m_iiFreeGameTime	= ((INT64)p->free_time_left) * 1000;
	m_iFreeGameEnd		= p->free_time_end;
	m_dwLastAccTime		= a_GetTime();
}

void CECGameSession::OnPrtcSubmitProblemToGMRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	Report2GM_Re* p = (Report2GM_Re*)pProtocol;

	if (p->retcode == ERR_GENERAL)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_MSGSENDWAIT);
	}
	else if (p->retcode == ERR_COMMUNICATION)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_MSGSENDFAIL);
	}
}

void CECGameSession::OnPrtcImpeachPlayerToGMRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	Complain2GM_Re* p = (Complain2GM_Re*)pProtocol;

	if (p->retcode == ERR_GENERAL)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_MSGSENDWAIT);
	}
	else if (p->retcode == ERR_COMMUNICATION)
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_MSGSENDFAIL);
	}
}

void CECGameSession::OnPrtcMatrixChallenge(GNET::Protocol* pProtocol)
{
	a_LogOutput(1, "CECReconnect reset for matrix challenge");
	CECReconnect::Instance().ResetState();
	CECLoginUIMan* pLoginUIMan = g_pGame->GetGameRun()->GetUIManager()->GetLoginUIMan();
	if( pLoginUIMan )
		pLoginUIMan->LaunchMatrixChallenge(pProtocol);
}

void CECGameSession::OnPrtcACQuestion(GNET::Protocol* pProtocol)
{
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	((CDlgQuestion*)pGameUI->GetDialog("Win_Question"))->SetQuestion(pProtocol);
}

void CECGameSession::OnPrtcAutoLockSetRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	AutolockSet_Re* p = (AutolockSet_Re*)pProtocol;
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	((CDlgAutoLock*)pGameUI->GetDialog("Win_AutoLock"))->SetAutoLockTime(p);
}

static bool ProcessErrorForBackShopRelated(int retcode)
{
	if(retcode != ERROR_SUCCESS)
	{
		ACString errMsg;
		g_pGame->GetGameSession()->GetServerError(retcode, errMsg);
		
		CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) pGameUI->AddChatMessage(errMsg, GP_CHAT_SYSTEM);
		return true;
	}

	return false;
}

void CECGameSession::OnPrtcRefGetReferenceCodeRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	RefGetReferenceCode_Re* p = (RefGetReferenceCode_Re*)pProtocol;

	if (ProcessErrorForBackShopRelated(p->retcode))
		return;

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackOthers *pBackOthers = (CDlgBackOthers *)(pGameUI->GetDialog("Win_BackOthers"));
		pBackOthers->OnPrtcRefGetReferenceCodeRe(p);
	}
}

void CECGameSession::OnPrtcRefListReferralsRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	RefListReferrals_Re* p = (RefListReferrals_Re*)pProtocol;
	
	ProcessErrorForBackShopRelated(p->retcode);
	
	// Process on for later process in CDlgBackOthers

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackOthers *pBackOthers = (CDlgBackOthers *)(pGameUI->GetDialog("Win_BackOthers"));
		pBackOthers->OnPrtcRefListReferralsRe(p);
	}
}

void CECGameSession::OnPrtcRefWithdrawBonusRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	RefWithdrawBonus_Re* p = (RefWithdrawBonus_Re*)pProtocol;
	
	if (ProcessErrorForBackShopRelated(p->retcode))
		return;

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackOthers *pBackOthers = (CDlgBackOthers *)(pGameUI->GetDialog("Win_BackOthers"));
		pBackOthers->OnPrtcRefWithdrawBonusRe(p);
	}
}

void CECGameSession::OnPrtcGetRewardListRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	GetRewardList_Re* p = (GetRewardList_Re*)pProtocol;
	
	if (ProcessErrorForBackShopRelated(p->retcode))
		return;

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackSelf *pBackSelf = (CDlgBackSelf *)(pGameUI->GetDialog("Win_BackSelf"));
		pBackSelf->OnPrtcGetRewardListRe(p);
	}
}

void CECGameSession::OnPrtcExchangeConsumePointsRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	ExchangeConsumePoints_Re* p = (ExchangeConsumePoints_Re*)pProtocol;
	
	if (ProcessErrorForBackShopRelated(p->retcode))
		return;

	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackSelf *pBackSelf = (CDlgBackSelf *)(pGameUI->GetDialog("Win_BackSelf"));
		pBackSelf->OnPrtcExchangeConsumePointsRe(p);
	}
}

void CECGameSession::OnPrtcRewardMatrueNotice(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	RewardMatureNotice* p = (RewardMatureNotice*)pProtocol;
	CECGameUIMan *pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		CDlgBackSelf *pBackSelf = (CDlgBackSelf *)(pGameUI->GetDialog("Win_BackSelf"));
		pBackSelf->OnPrtcRewardMatrueNotice(p);
	}
}

void CECGameSession::OnPrtcCashOpt(GNET::Protocol* pProtocol)
{
	CECGameUIMan* pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if( pProtocol->GetType() == PROTOCOL_CASHLOCK_RE )
	{
		CashLock_Re* p = (CashLock_Re*)pProtocol;
		if( p->retcode == ERR_SUCCESS )
		{
			pGameUIMan->GetDialog("Win_GoldPwdInput")->Show(false);
			((CDlgGoldTrade*)pGameUIMan->GetDialog("Win_GoldTrade"))->SetLocked(false);
		}
		else
		{
			pGameUIMan->MessageBox("", pGameUIMan->GetStringFromTable(869), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
	else if( pProtocol->GetType() == PROTOCOL_CASHPASSWORDSET_RE )
	{
		CashPasswordSet_Re* p = (CashPasswordSet_Re*)pProtocol;
		if( p->retcode == ERR_SUCCESS )
		{
			pGameUIMan->GetDialog("Win_GoldPwdChange")->Show(false);
		}
		else if( p->retcode == ERR_STOCK_CASHLOCKED )
		{
			pGameUIMan->MessageBox("", pGameUIMan->GetStringFromTable(871), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			pGameUIMan->MessageBox("", pGameUIMan->GetStringFromTable(870), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}
}

void CECGameSession::OnPrtcAccountLoginRecord(GNET::Protocol *pProtocol)
{
	GNET::AccountLoginRecord *p = (GNET::AccountLoginRecord *)pProtocol;
	CECGameRun::AccountLoginInfo info;
	info.userid = p->userid;
	info.login_time = p->login_time;
	info.login_ip = p->login_ip;
	info.current_ip = p->current_ip;
	g_pGame->GetGameRun()->SetAccountLoginInfo(info);
}

void CECGameSession::c2s_CmdMoveIvtrItem(int iSrc, int iDest, int iAmount)
{
	FreezeHostItem(IVTRTYPE_PACK, iSrc, true);
	FreezeHostItem(IVTRTYPE_PACK, iDest, true);
	::c2s_SendCmdMoveIvtrItem(iSrc, iDest, iAmount); 
}

void CECGameSession::c2s_CmdExgIvtrItem(int iIndex1, int iIndex2)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIndex1, true);
	FreezeHostItem(IVTRTYPE_PACK, iIndex2, true);
	::c2s_SendCmdExgIvtrItem(iIndex1, iIndex2); 
}

void CECGameSession::c2s_CmdDropIvtrItem(int iIndex, int iAmount)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIndex, true);
	::c2s_SendCmdDropIvtrItem(iIndex, iAmount); 
}

void CECGameSession::c2s_CmdDropEquipItem(int iIndex)
{ 
	FreezeHostItem(IVTRTYPE_EQUIPPACK, iIndex, true);
	::c2s_SendCmdDropEquipItem(iIndex); 
}

void CECGameSession::c2s_CmdExgEquipItem(int iIndex1, int iIndex2)
{ 
	FreezeHostItem(IVTRTYPE_EQUIPPACK, iIndex1, true);
	FreezeHostItem(IVTRTYPE_EQUIPPACK, iIndex2, true);
	::c2s_SendCmdExgEquipItem(iIndex1, iIndex2); 
}

void CECGameSession::c2s_CmdEquipItem(int iIvtrIdx, int iEquipIdx)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIvtrIdx, true);
	FreezeHostItem(IVTRTYPE_EQUIPPACK, iEquipIdx, true);
	::c2s_SendCmdEquipItem(iIvtrIdx, iEquipIdx); 
}

void CECGameSession::c2s_CmdUseSharpen(int iIvtrIdx, int tid, int iEquipIdx)
{
	unsigned int weaponSlot = iEquipIdx;
	c2s_CmdUseItemWithArg((BYTE)IVTRTYPE_PACK, 1, (BYTE)iIvtrIdx, tid, &weaponSlot, sizeof(weaponSlot));
}

void CECGameSession::c2s_CmdMoveItemToEquip(int iIvtrIdx, int iEquipIdx)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIvtrIdx, true);
	FreezeHostItem(IVTRTYPE_EQUIPPACK, iEquipIdx, true);
	::c2s_SendCmdMoveItemToEquip(iIvtrIdx, iEquipIdx); 
}

void CECGameSession::c2s_CmdExgTrashBoxItem(int where, int iIndex1, int iIndex2)
{ 
	FreezeHostItem(where, iIndex1, true);
	FreezeHostItem(where, iIndex2, true);
	::c2s_SendCmdExgTrashBoxItem(where, iIndex1, iIndex2); 
}

void CECGameSession::c2s_CmdMoveTrashBoxItem(int where, int iSrc, int iDst, int iAmount)
{ 
	FreezeHostItem(where, iSrc, true);
	FreezeHostItem(where, iDst, true);
	::c2s_SendCmdMoveTrashBoxItem(where, iSrc, iDst, iAmount); 
}

void CECGameSession::c2s_CmdExgTrashBoxIvtrItem(int where, int iTrashIdx, int iIvtrIdx)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIvtrIdx, true);
	FreezeHostItem(where, iTrashIdx, true);
	::c2s_SendCmdExgTrashBoxIvtrItem(where, iTrashIdx, iIvtrIdx); 
}

void CECGameSession::c2s_CmdMoveTrashBoxToIvtr(int where, int iTrashIdx, int iIvtrIdx, int iAmount)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIvtrIdx, true);
	FreezeHostItem(where, iTrashIdx, true);
	::c2s_SendCmdMoveTrashBoxToIvtr(where, iTrashIdx, iIvtrIdx, iAmount); 
}

void CECGameSession::c2s_CmdMoveIvtrToTrashBox(int where, int iTrashIdx, int iIvtrIdx, int iAmount)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iIvtrIdx, true);
	FreezeHostItem(where, iTrashIdx, true);
	::c2s_SendCmdMoveIvtrToTrashBox(where, iTrashIdx, iIvtrIdx, iAmount); 
}

void CECGameSession::c2s_CmdChargeEquipFlySword(int iEleIdx, int iCount)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iEleIdx, true);
	FreezeHostItem(IVTRTYPE_EQUIPPACK, EQUIPIVTR_FLYSWORD, true);
	::c2s_SendCmdChargeEquipFlySword(iEleIdx, iCount);
}

void CECGameSession::c2s_CmdChargeFlySword(int iEleIdx, int iFSIdx, int iCount, int idFlySword)
{ 
	FreezeHostItem(IVTRTYPE_PACK, iEleIdx, true);
	FreezeHostItem(IVTRTYPE_PACK, iFSIdx, true);
	::c2s_SendCmdChargeFlySword(iEleIdx, iFSIdx, iCount, idFlySword); 
}

void CECGameSession::c2s_CmdNPCSevSell(int iItemNum, C2S::npc_sell_item* aItems)
{
	for (int i=0; i < iItemNum; i++)
		FreezeHostItem(IVTRTYPE_PACK, aItems[i].index, true);
	
	::c2s_SendCmdNPCSevSell(iItemNum, aItems);
}

void CECGameSession::c2s_CmdNPCSevBoothSell(int idBooth, int iItemNum, C2S::npc_booth_item* aItems)
{ 
	for (int i=0; i < iItemNum; i++)
		FreezeHostItem(IVTRTYPE_PACK, aItems[i].inv_index, true);
	
	::c2s_SendCmdNPCSevBoothSell(idBooth, iItemNum, aItems); 
}

void CECGameSession::c2s_CmdGoblinChargeEquip(int iEleIdx, int iCount)
{
	FreezeHostItem(IVTRTYPE_PACK, iEleIdx, true);
	FreezeHostItem(IVTRTYPE_EQUIPPACK, EQUIPIVTR_GOBLIN, true);
	::c2s_SendCmdGoblinChargeEquip(iEleIdx, iCount);
}
void CECGameSession::c2s_CmdGoblinCharge(int iEleIdx, int iGoblinIdx, int iCount, int idGoblin)
{
	FreezeHostItem(IVTRTYPE_PACK, iEleIdx, true);
	FreezeHostItem(IVTRTYPE_PACK, iGoblinIdx, true);
	::c2s_SendCmdGoblinCharge(iEleIdx, iGoblinIdx, iCount, idGoblin); 
}

void CECGameSession::c2s_CmdLogout(int iOutType)
{
	// clear the selling role id
	g_pGame->GetGameRun()->SetSellingRoleID(0);
	::c2s_SendCmdLogout(iOutType);
}

void CECGameSession::c2s_CmdSendMassMail(int type, const ACHAR* szTitle, const ACHAR* szContext, const abase::vector<int> &receivers)
{
	GNET::PlayerSendMassMail  p;
	p.mass_type = type;
	p.roleid = m_iCharID;
	p.receiver_list.assign(receivers.begin(), receivers.end());
	
	if (szTitle)
	{
		int iLen = a_strlen(szTitle) * sizeof (ACHAR);
		p.title.replace(szTitle, iLen);
	}
	
	if (szContext)
	{
		int iLen = a_strlen(szContext) * sizeof (ACHAR);
		p.context.replace(szContext, iLen);
	}
	Marshal::OctetsStream os; Octets o = p.marshal(os);
	
	using namespace C2S;
	int iSize = sizeof(cmd_header) + sizeof(cmd_send_mass_mail)-sizeof(char)+o.size();
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);
	if (!pBuf)
		return;
	((cmd_header*)pBuf)->cmd = SEND_MASS_MAIL;
	cmd_send_mass_mail* pCmd = (cmd_send_mass_mail*)(pBuf + sizeof(cmd_header));
	pCmd->service_id = PROTOCOL_PLAYERSENDMASSMAIL;
	memcpy(pCmd->data, o.begin(), o.size());
	SendGameData(pBuf, iSize);
	a_freetemp(pBuf);
}

void CECGameSession::c2s_CmdAstrolabeShuffleAtt(int inv_index, int itemid)
{
	using namespace C2S;

	const int iSize = sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request) + sizeof(cmd_astrolabe_opt_slot_roll);
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);

	((cmd_header*)pBuf)->cmd = C2S::ASTROLABE_OPERATE_REQUEST;

	cmd_astrolabe_operate_request* pCmd = (cmd_astrolabe_operate_request*)(pBuf + sizeof(cmd_header));
	pCmd->opttype = ASTROLABE_OPT_SLOT_ROLL;

	cmd_astrolabe_opt_slot_roll* pData = (cmd_astrolabe_opt_slot_roll*)(pBuf + sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request));
	pData->inv_index = inv_index;
	pData->itemid = itemid;

	g_pGame->GetGameSession()->SendGameData(pBuf, iSize);
	a_freetemp(pBuf);
}

void CECGameSession::c2s_CmdAstrolabeIncPoint(int inv_index, int itemid)
{
	using namespace C2S;

	const int iSize = sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request) + sizeof(cmd_astrolabe_opt_aptit_inc);
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);

	((cmd_header*)pBuf)->cmd = C2S::ASTROLABE_OPERATE_REQUEST;

	cmd_astrolabe_operate_request* pCmd = (cmd_astrolabe_operate_request*)(pBuf + sizeof(cmd_header));
	pCmd->opttype = ASTROLABE_OPT_APTIT_INC;

	cmd_astrolabe_opt_aptit_inc* pData = (cmd_astrolabe_opt_aptit_inc*)(pBuf + sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request));
	pData->inv_index = inv_index;
	pData->itemid = itemid;

	g_pGame->GetGameSession()->SendGameData(pBuf, iSize);
	a_freetemp(pBuf);
}

void CECGameSession::c2s_CmdAstrolabeRandomAddon(int count, int limit, int inv_index, int itemid)
{
	using namespace C2S;

	const int iSize = sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request) + sizeof(cmd_astrolabe_opt_addon_roll);
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);

	((cmd_header*)pBuf)->cmd = C2S::ASTROLABE_OPERATE_REQUEST;

	cmd_astrolabe_operate_request* pCmd = (cmd_astrolabe_operate_request*)(pBuf + sizeof(cmd_header));
	pCmd->opttype = ASTROLABE_OPT_ADDON_ROLL;

	cmd_astrolabe_opt_addon_roll* pData = (cmd_astrolabe_opt_addon_roll*)(pBuf + sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request));
	pData->inv_index = inv_index;
	pData->itemid = itemid;
	pData->times = count;
	pData->addon_limit = limit;

	g_pGame->GetGameSession()->SendGameData(pBuf, iSize);
	a_freetemp(pBuf);
}

void CECGameSession::c2s_CmdAstrolabeSwallow(int type, int inv_index, int itemid)
{
	using namespace C2S;

	const int iSize = sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request) + sizeof(cmd_astrolabe_opt_swallow);
	BYTE* pBuf = (BYTE*)a_malloctemp(iSize);

	((cmd_header*)pBuf)->cmd = C2S::ASTROLABE_OPERATE_REQUEST;

	cmd_astrolabe_operate_request* pCmd = (cmd_astrolabe_operate_request*)(pBuf + sizeof(cmd_header));
	pCmd->opttype = ASTROLABE_OPT_SWALLOW;

	cmd_astrolabe_opt_swallow* pData = (cmd_astrolabe_opt_swallow*)(pBuf + sizeof(cmd_header) + sizeof(cmd_astrolabe_operate_request));
	pData->inv_index = inv_index;
	pData->itemid = itemid;
	pData->type = type;

	g_pGame->GetGameSession()->SendGameData(pBuf, iSize);
	a_freetemp(pBuf);
}
//	Get player's faction info
void CECGameSession::GetPlayerConsumeInfo(int iPlayerNum, const int* aIDs)
{
	using namespace GNET;
	
	static const int iNumLimit = 128;
	int iCount = 0;
	
	while (iCount < iPlayerNum)
	{
		GMGetPlayerConsumeInfo p;
		p.gmroleid = m_iCharID;
		
		int iNumSend = iNumLimit;
		if (iCount + iNumLimit > iPlayerNum)
			iNumSend = iPlayerNum - iCount;
		
		for (int i=0; i < iNumSend; i++)
			p.playerlist.add(aIDs[iCount+i]);
		
		SendNetData(p);
		
		iCount += iNumSend;
	}
}

int CECGameSession::GetZoneID()
{
	//	ԭ zoneid Ϊ char ��ʽ���п���Ϊ��ֵ
	return m_pNetMan ? (unsigned char)m_pNetMan->GetZoneID() : -1;
}
