

#ifndef __p2spInternalStruct_h__
#define __p2spInternalStruct_h__

#pragma once
#include "engine_sdk.h"
#include "i_p2sp_download.h"
#include "p2spVersion.h"

#include <map>
#include <vector>

namespace p2sp4 
{
	typedef unsigned int GID;
	typedef P2SP_TASK_STATE	ePacketState;

	typedef std::map<std::string, TaskInfomationT>	TaskInfomationMap;

	typedef std::vector<P2SPServerParam> ServerParamVector;
	struct StartEngineParam
	{
		int game_id;                    // 启动引擎的game标识 MAGIC_GAME_ID为下载客户端
		std::string seed_url_prefix;    // 种子文件url前缀

		ushort listen_port;             // P2P侦听端口
		std::string save_path_utf8;     // 下载保存路径

		size_t max_download_speed;      // 最大下载速率(B) 0:不限速
		size_t max_upload_speed;        // 最大上传速率(B) 0:不限速
		size_t start_p2p_throttler;     // 任务启动p2p下载的阀值
		size_t max_downloading_task;    // 最大同时下载任务数
		size_t task_http_connections;   // 单任务的HTTP连接数

		std::string log_file_url;       // 日志文件上传地址
		std::string log_file_part_name; // 日志文件上传form中的part name
		std::string minidump_file_url;      // 崩溃文件上传地址
		std::string minidump_file_part_name;// 崩溃文件上传form中的part name

		ServerParamVector cis_list;
		ServerParamVector tunnel_list;

		//--------------------------------------------------------------------
		std::string s_base_seed_url;
		std::string s_game_list_url;
		std::string s_game_task_list_url;
	};

	enum ePacketType {
		ept_unkown				= 0,
		ept_client				= 1,
		ept_pre_client			= 11,
		ept_general_patch		= 2,
		ept_pre_general_patch	= 21,
		ept_latest_patch		= 3,	
		ept_pre_latest_patch	= 31,
		ept_other_patch			= 4,	//一般补丁包
		ept_resources_patch		= 5,	//资源补丁包
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct Speed 
	{
		uint32 http;
		uint32 p2p;
		uint32 total;
		uint32 upload;

		Speed() { clear(); }
		
		void clear() {
			http = 0;
			p2p = 0;
			total = 0;
			upload = 0;
		}
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct Progress 
	{
		uint64 complete_size;
		uint64 http_download;
		uint64 http_saved;
		uint64 p2p_download;
		uint64 p2p_saved;
		uint32 seconds;

		Progress() { clear(); }

		void clear() {
			complete_size = 0;
			http_download = 0;
			http_saved = 0;
			p2p_download = 0;
			p2p_saved = 0;
			seconds = 0;
		}
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct Complete 
	{
		uint64 time;

		Complete() { clear(); }

		void clear() {
			time = 0;
		}
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct SinglePacket {
		GID				gid;
		TASKID			tid;
		std::string		md5;

		ePacketType		type;
		version			sver;	//起始版本
		version			ever;	//终止版本、V093120101016

		std::wstring	wsPacketName;
		std::wstring	wsDescriptionFilesize;	//描述文件大小
		std::wstring	wsDescriptionDate;		//描述日期 //2010年10月22日
		std::wstring	wsDescription;			//其他信息，简介等

		// 下载过程中相关 ---------------------------------------
		std::wstring	wsFilename;
		std::wstring	wsSavePath;
		uint64			llFilesize;
		//uint32			seconds;

		Speed			speed;
		Progress		progress;
		Complete		complete;

		ePacketState	state;
		uint32			err;
		// 下载过程中相关 ---------------------------------------

		SinglePacket() {
			gid = 0;
			tid = 0;
			type = ept_unkown;
			llFilesize = 0;
			//seconds = 0;
			state = P2SPTS_TASK_UNKOWN;
		}
	};
	typedef std::map<std::string, SinglePacket*> SinglePacketMap;
	
	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	typedef std::map<uint32, std::string> SinglePacketIndexMap;
	struct MultiPacket
	{
		GID				gid;
		std::string		md5;
		version			sver;
		version			ever;

		// 下载过程中相关 ---------------------------------------
		Speed			speed;
		Progress		progress;
		Complete		complete;

		//uint32			seconds;
		uint32			packets; //初始化任务中子任务数

		ePacketState	state;
		uint64			llFilesize;

		SinglePacketIndexMap singlePackets;
		// 下载过程中相关 ---------------------------------------

		MultiPacket() {
			gid = 0;
			//seconds = 0;
			packets = 0;
			state = P2SPTS_TASK_UNKOWN;
			llFilesize = 0;
		}
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct Game {
		GID				gid;
		std::wstring	wsCheckeVersionValue;
		version			verLocal;
		version			verServer;
		std::wstring	wsExcuteAppPath;

		SinglePacketMap allSinglePackets;
		MultiPacket		downloadPackets;

		Game() {
			gid = 0;
		}
	};
	typedef std::map<GID, Game*> GameMap;

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	struct GameCheckValue {
		GID gid;
		const char* value;
	};
	const char* getGameCheckValue(GID gid);


}
#endif //__p2spInternalStruct_h__