
#ifndef __p2spRoot_h__
#define __p2spRoot_h__

#include "p2spStableHeadres.h"
#include "p2spEngineHandler.h"
#include "p2spUtility.h"

#include "p2spInternalStruct.h"
#include "p2spEngineHandler.h"
#include "p2spPrerequisites.h"

namespace p2sp4 
{
	// 游戏ID枚举
	enum eGameID {
		game_wan_mei_shi_jie			= 1,	//完美世界、
		game_wu_lin_wai_zhuan			= 2,	//武林外传、新武林外传
		game_wan_mei_guo_ji				= 3,    //完美国际、完美前传
		game_zhu_xian					= 4,	//诛仙前传、诛仙2
		game_ci_bi						= 5,	//赤壁、
		game_re_wu_pai_dui				= 6,	//热舞派对2
		game_kou_dai_xi_you				= 7,	//口袋西游、
		game_shen_gui_chuan_qi			= 8,	//神鬼传奇
		game_meng_huan_zhu_xian			= 9,	//梦幻诛仙
		game_xiang_long_zhi_jian		= 10,	//降龙之剑
		game_shen_mo_da_lu				= 11,	//神魔大陆
		game_shen_gui_shi_jie			= 12,	//神鬼世界
		game_xiao_ao_jiang_hu			= 16,	//笑傲江湖
		game_yi_tian_tu_long_ji			= 18,	//倚天屠龙记 
		game_xiang_long_ji_zhi			= 21,	//降龙极致
		game_shen_diao_xia_lv			= 26,	//梦幻神雕侠侣
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	class Root : fagex::Singleton<Root>
	{
	public:
		Root();
		~Root();

		/// 下载、启动DLL
		bool Start(int game_id);
		
		/// 设置参数
		bool Setting(CONST SettingItemT* setting);
		bool GetSetting(SettingItemT* setting);

		bool AddDownload(const char* filemd5);
		bool StartDownload(const char* filemd5);
		bool StopDownload(const char* filemd5);
		bool DeleteDownload(const char* filemd5);
		bool GetTaskInfomation(TaskInfomationT* info);

		/// 停止、卸载DLL
		bool Stop();

		/// 设置优先下载
		bool engineSetPriorityDownloads();

		/// 设置优先上网
		bool engineSetPriorityBrowse();

		/// 删除SPI层LSP过滤，恢复到原状
		bool engineRemoveFilter();

	private:
		bool _AddDownloadImpl(const char* filemd5);
		bool _StartDownloadImpl(const char* filemd5);
		bool _StopDownloadImpl(const char* filemd5);
		bool _DeleteDownloadImpl(const char* filemd5);

	public:
		enum TaskUserAction {
			TaskUserAction_UNKNOW,
			TaskUserAction_ADD,
			TaskUserAction_START,
			TaskUserAction_STOP,
			TaskUserAction_DELETE,
		};

	private:
		typedef std::deque<TaskUserAction> TaskUserActionQueue;
		typedef std::map<std::string, TaskUserActionQueue> TaskUserActionMap;
		TaskUserActionMap mTaskUserActionMap;
		CRITICAL_SECTION mTaskUserActionMapCS;

	public:
		SinglePacket* _GetSinglePacket(const std::string& file_md5);
		void _ProcessTaskCacheCmd(const std::string& file_md5, TaskUserAction action, uint32 err);
        void _AddSinglePacketInfo(const std::string& file_md5, P2SP_TASK_STATE t);
		void _UpdateSinglePacket(const std::string& file_md5, uint32 err);
		void _CheckTaskSpeedTimeOut(const std::string& file_md5, uint32 err);
		void logMessage(const char* format, ...);

	private:
		bool isUpdateEngineDLL(std::string& xmlContext, tUpdateFileInfo* dllUpdateInfo);
		bool UpdateEngineDLL(const tUpdateFileInfo* dllUpdateInfo);

		/// 解析服务器端参数
		bool ParseStartEngineParam(const std::string& xmlContext);

		void CheckSinglePacket(Game* g);
		void CheckLocalVersion(Game* g);

		bool FillPacketList();

		void Release();
	
		void SaveComplateSinglePacketToConfig(const std::string& smd5);

		void RestoreComplateSinglePacketFormConfig();

	private:
		int mGameID; 

		EngineHandler* mEngineHandler;
		GameMap mGameMap;
		TaskInfomationMap mTaskInfomationMap;
		CRITICAL_SECTION mTaskInfomationMapCS;

		SettingItemT mSettingItem;

		struct TaskMonitorState{
			uint32 seconds;
			uint64 speed_count;
			//Progress previousRrog;
			bool timeout;
			uint32 itimeout_time;

			TaskMonitorState() { clear(); }
			void clear() {
				seconds = 0;
				speed_count = 0;
				timeout = false;
				itimeout_time = 0;
			}
		};
		typedef std::map<std::string, TaskMonitorState> TaskMonitorStateMap;
		TaskMonitorStateMap mTaskMonitorStateMap;
		bool mInitOK;

	public:
		static StartEngineParam ms_StartEngineParam;
		static Root* getSingletonPtr(void);
		static Root& getSingleton(void);
	};
}

#endif //__p2spRoot_h__