

#include "p2spRoot.h"
#include "p2spEngineUpdate.h"

#include "download.h"
#include "memory_map_file.h"

//---------------------------------------------------------------------------------------
bool replaceW(std::wstring& src, const std::wstring& old, const std::wstring& now)
{
	size_t num = old.length();
	if(num == 0) return false;

	size_t index = src.find(old, 0);
	while(index != std::wstring::npos)
	{
		src.replace(index, num, now);
		index = index + now.length();
		index = src.find(old,index);
	}
	return true;
}

//---------------------------------------------------------------------------------------
template<> p2sp4::Root* fagex::Singleton<p2sp4::Root>::ms_Singleton = 0;
namespace p2sp4
{
	const UINT P2SPE_TIMEOUT = 1000;
	const UINT P2SPE_TIMEOUT_OFF = 1001;

	StartEngineParam Root::ms_StartEngineParam;
	//--------------------------------------------------------------------------------------------
	Root* Root::getSingletonPtr(void)
	{
		return ms_Singleton;
	}

	//--------------------------------------------------------------------------------------------
	Root& Root::getSingleton(void)
	{
		assert(ms_Singleton);
		return *ms_Singleton;
	}

	//--------------------------------------------------------------------------------------------
	Root::Root() : mEngineHandler(NULL), mGameID(-1), mInitOK(false)
	{
		::InitializeCriticalSection(&mTaskInfomationMapCS);
		::InitializeCriticalSection(&mTaskUserActionMapCS);
		mEngineHandler = new EngineHandler();
	}

	//--------------------------------------------------------------------------------------------
	Root::~Root()
	{
		if(mEngineHandler) {
			delete mEngineHandler;
			mEngineHandler = NULL;
		}

		GameMap::iterator i = mGameMap.begin();
		for(; i != mGameMap.end(); ++i) {
			Game* g = i->second;
			SinglePacketMap::iterator it = g->allSinglePackets.begin();
			for(; it != g->allSinglePackets.end(); ++it) {
					delete it->second;
			}
			g->allSinglePackets.clear();
			delete g;
		}
		mGameMap.clear();

		::DeleteCriticalSection(&mTaskUserActionMapCS);
		::DeleteCriticalSection(&mTaskInfomationMapCS);
	}

	//--------------------------------------------------------------------------------------------
	void Root::Release()
	{
		delete this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	//--------------------------------------------------------------------------------------------
	bool Root::Start(int game_id)
	{
		mGameID = game_id;

		// 加载DLL文件
		CFilePath pathNormalFile;
		PathProvider(DIR_EXE, pathNormalFile);
		pathNormalFile.Append(p2sp4::wszEngineDllName);

		// 更新DLL
		std::string xmlContext;
		tUpdateFileInfo dllUpdateInfo;
		if(this->isUpdateEngineDLL(xmlContext, &dllUpdateInfo)) {
			/// 需要更新，创建一个进程更新，此处不强制更新了。
			if(!pathNormalFile.IsExists()) {
				this->UpdateEngineDLL(&dllUpdateInfo);
			}
			else {
				bool ok = CreateProcessForUpdateEngine(&dllUpdateInfo);
				if(!ok) {
					this->UpdateEngineDLL(&dllUpdateInfo);
				} 
			}
		}
		if(!pathNormalFile.IsExists()) {
			return false;
		}
		bool ok = this->ParseStartEngineParam(xmlContext);
		if(!ok) return ok;
		
		//调整不限制下载和上传速度
		{
			ms_StartEngineParam.max_download_speed = 0;
			ms_StartEngineParam.max_upload_speed = 0;
		}

        mSettingItem.maxDownloadNum = (UINT)ms_StartEngineParam.max_downloading_task;
        mSettingItem.maxDownloadSpeed = (UINT)ms_StartEngineParam.max_download_speed;
        mSettingItem.maxUploadSpeed= (UINT)ms_StartEngineParam.max_upload_speed;
        mSettingItem.taskTimeout = 60 * 5;
        mSettingItem.taskTimeoutSpeed = 10240;
        StrCpyNW(mSettingItem.savePath, UTF8ToWide(ms_StartEngineParam.save_path_utf8).c_str(), MAX_PATH);
		
        // 加载任务列表
		Game* g = new Game();
		g->gid = game_id;
		mGameMap[game_id] = g;
		ok = this->FillPacketList();
		if(!ok) return ok;

		//获取检验游戏版本号变量
		const char* szValue = p2sp4::getGameCheckValue(g->gid);
		if(szValue) {
			USES_CONVERSION;
			g->wsCheckeVersionValue = CT2CW(CA2CT(szValue));
		} 

		// 分析任务属性，及本地游戏版本
		CheckSinglePacket(g);
		CheckLocalVersion(g);

		// 加载下载完成任务
		this->RestoreComplateSinglePacketFormConfig();

		P2SPStartEngineParam param = {
			//ms_StartEngineParam.game_id,
            0,
			ms_StartEngineParam.s_base_seed_url.c_str(),
			ms_StartEngineParam.listen_port,
			ms_StartEngineParam.save_path_utf8.c_str(),
			ms_StartEngineParam.max_download_speed,
			ms_StartEngineParam.max_upload_speed,
			ms_StartEngineParam.start_p2p_throttler,
			ms_StartEngineParam.max_downloading_task,
			ms_StartEngineParam.task_http_connections,
			ms_StartEngineParam.log_file_url.c_str(),
			ms_StartEngineParam.log_file_part_name.c_str(),
			ms_StartEngineParam.minidump_file_url.c_str(),
			ms_StartEngineParam.minidump_file_part_name.c_str(),
			&ms_StartEngineParam.cis_list[0],
			(int)ms_StartEngineParam.cis_list.size(),
			&ms_StartEngineParam.tunnel_list[0],
			(int)ms_StartEngineParam.tunnel_list.size()
		};
		mInitOK = !!mEngineHandler->init(pathNormalFile, param);
		ok &= mInitOK;

		return ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::Setting(CONST SettingItemT* setting) 
	{
		if(!setting) return false;
		
		if(setting->mask & P2P_SETTING_ITEM_FILE_SAVE_PATH)
		{
			CFilePath path(setting->savePath);
			CFilePath tmp_path(setting->savePath);
			tmp_path.StripToRoot();
			if(tmp_path.value() == std::wstring(L"\\") || tmp_path.value() == std::wstring(L"/")) return false;
			if(tmp_path.Empty()) return false;
			
			path.AddBackslash();
			
			std::wstring wsPath(path.value());
			if(wsPath.find(L":\\") == std::wstring::npos) return false;

			if(!path.IsExists()) {
				EnsureDirectoryForFile(path.value(), NULL);
			}
			if(!path.IsExists()) {
				return false;
			}

			WCHAR PATH[MAX_PATH] = {0};
			PathCanonicalizeW(PATH, path.value());
			path = CFilePath(PATH);
		}

		if(setting->mask & P2P_SETTING_ITEM_MAX_DOWNLOAD_NUM) {
			if(setting->maxDownloadNum == 0) mSettingItem.maxDownloadNum = 1;
			else if(setting->maxDownloadNum >= 10) mSettingItem.maxDownloadNum = 10;
			else mSettingItem.maxDownloadNum = setting->maxDownloadNum;
			mEngineHandler->P2SP_UserAction(P2P_SETTING_ITEM_MAX_DOWNLOAD_NUM, (WPARAM)mSettingItem.maxDownloadNum, 0);
		}

		if(setting->mask & P2P_SETTING_ITEM_MAX_DOWNLOAD_SPEED) {
			if(setting->maxDownloadSpeed > 0 && setting->maxDownloadSpeed < 10240) mSettingItem.maxDownloadSpeed = 10240;
			else mSettingItem.maxDownloadSpeed = setting->maxDownloadSpeed;
			mEngineHandler->P2SP_UserAction(P2P_SETTING_ITEM_MAX_DOWNLOAD_SPEED, (WPARAM)mSettingItem.maxDownloadSpeed, 0);
		}

		if(setting->mask & P2P_SETTING_ITEM_MAX_UPLOAD_SPEED) {
			if(setting->maxUploadSpeed > 0 && setting->maxUploadSpeed < 10240) mSettingItem.maxUploadSpeed = 10240;
			else mSettingItem.maxUploadSpeed = setting->maxUploadSpeed;
			mEngineHandler->P2SP_UserAction(P2P_SETTING_ITEM_MAX_UPLOAD_SPEED, (WPARAM)mSettingItem.maxUploadSpeed, 0);
		}

		if(setting->mask & P2P_SETTING_ITEM_FILE_SAVE_PATH) {
			std::wstring ws(setting->savePath);
			if(!ws.empty() && ws[ws.length() - 1] != L'/' && ws[ws.length() - 1] != L'\\')
			{
				ws += L"\\";
			}
			replaceW(ws, L"/", L"\\");
			replaceW(ws, L"\\\\", L"\\");
			CFilePath path(ws.c_str());

			std::string* save_path_utf8 = new std::string(WideToUTF8(path.value()));
			mEngineHandler->P2SP_UserAction(P2P_SETTING_ITEM_FILE_SAVE_PATH, (WPARAM)save_path_utf8, 0);
			StrCpyW(mSettingItem.savePath, path.value());
		}

		if(setting->mask & P2P_SETTING_ITEM_TASK_TIMEOUT) {
			if(setting->taskTimeout < 300) mSettingItem.taskTimeout = 300;
			else mSettingItem.taskTimeout = setting->taskTimeout;
		}

		if(setting->mask & P2P_SETTING_ITEM_TASK_TIMEOUT_SPEED) {
			if(setting->taskTimeoutSpeed < 10240) mSettingItem.taskTimeoutSpeed = 10240;
			else mSettingItem.taskTimeoutSpeed = setting->taskTimeoutSpeed;
		}

		//if(setting->mask & P2P_SETTING_ITEM_MAX_MD5_RETRYS) {
		//	if(setting->md5_max_retrys < 20) mSettingItem.md5_max_retrys = 20;
		//	else mSettingItem.md5_max_retrys = setting->md5_max_retrys;
		//	mEngineHandler->P2SP_UserAction(P2P_SETTING_ITEM_MAX_MD5_RETRYS, (WPARAM)mSettingItem.md5_max_retrys, 0);
		//}
		return true;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::GetSetting(SettingItemT* setting)
	{
		assert(setting);

		setting->mask = P2P_SETTING_ITEM_ALL;
		setting->maxDownloadNum = mSettingItem.maxDownloadNum;
		setting->maxDownloadSpeed = mSettingItem.maxDownloadSpeed;
		setting->maxUploadSpeed = mSettingItem.maxUploadSpeed;
		setting->taskTimeout = mSettingItem.taskTimeout;
		setting->taskTimeoutSpeed = mSettingItem.taskTimeoutSpeed;
		StrCpyW(setting->savePath, mSettingItem.savePath);

		return true;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::AddDownload(const char* filemd5)
	{
		TaskInfomationT info = {0};
		StrCpyA(info.md5, filemd5);
		bool ok = this->GetTaskInfomation(&info);
		if(ok) {
			while (P2P_TASK_STATE_NOREADY == info.state) {
				ok = this->GetTaskInfomation(&info);
				if(!ok) break;
				Sleep(1000);
				continue;
			}
			if(P2P_TASK_STATE_ERROR == info.state) {
				return false;
			}
		}

		ok = true;
		::EnterCriticalSection(&mTaskUserActionMapCS);
		TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
		if(i != mTaskUserActionMap.end()) {
			if(!i->second.empty()) {
				ok = false;
				i->second.push_back(TaskUserAction_ADD);
			}
		}
		::LeaveCriticalSection(&mTaskUserActionMapCS);
		if(!ok) return true;

		return _AddDownloadImpl(filemd5);
	}

	//--------------------------------------------------------------------------------------------
	bool Root::_AddDownloadImpl(const char* filemd5)
	{
		SinglePacket* t = _GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		switch(t->state) {
			case P2SPTS_TASK_UNKOWN:
			case P2SPTS_READY:
				{
					ok = mEngineHandler->P2SP_AddTask(filemd5, WideToUTF8(mSettingItem.savePath).c_str());
					break;
				}

			case P2SPTS_LOAD_RESUME:		// 断点续传待恢复状态, 恢复成功状态切换到P2SPTS_READY
			case P2SPTS_DOWNLOADING:		// 正在下载
			case P2SPTS_QUEUING:			// 正在排队等待下载
			case P2SPTS_STOP:				// 停止状态
			case P2SPTS_COMPLETE:			// 完成状态
			case P2SPTS_ALLOCATE_FREESPACE:	// 空间分配
				{
					break;
				}
			default:
				assert(false);
				ok = false;
		}

		if(ok) {
			//建立任务信息
			::EnterCriticalSection(&mTaskInfomationMapCS);
			TaskInfomationMap::iterator it = mTaskInfomationMap.find(filemd5);
			if(it == mTaskInfomationMap.end()) {
				TaskInfomationT task = { 0 };
				mTaskInfomationMap[filemd5] = task;
				TaskInfomationT& tt = mTaskInfomationMap[filemd5];

				StrCpyNA(tt.md5, filemd5, sizeof(tt.md5));
				tt.err = 0;
				tt.state = P2P_TASK_STATE_READY;
			}
			::LeaveCriticalSection(&mTaskInfomationMapCS);

			//建立命令缓存
			::EnterCriticalSection(&mTaskUserActionMapCS);
			TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
			if(i == mTaskUserActionMap.end()) {
				std::pair<TaskUserActionMap::iterator, bool> ret = 
					mTaskUserActionMap.insert(TaskUserActionMap::value_type(std::make_pair(filemd5, TaskUserActionQueue())));
				assert(ret.second);
				ret.first->second.push_back(TaskUserAction_ADD);
			}
			::LeaveCriticalSection(&mTaskUserActionMapCS);
		}
		return ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::StartDownload(const char* filemd5)
	{
		SinglePacket* t = _GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		::EnterCriticalSection(&mTaskInfomationMapCS);
		TaskInfomationMap::iterator it = mTaskInfomationMap.find(filemd5);
		if(it == mTaskInfomationMap.end()) {
			ok = false;
		}
		::LeaveCriticalSection(&mTaskInfomationMapCS);
		if(!ok) return false;

		::EnterCriticalSection(&mTaskUserActionMapCS);
		TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
		if(i != mTaskUserActionMap.end()) {
			if(!i->second.empty()) {
				ok = false;
				i->second.push_back(TaskUserAction_START);
			}
		}
		::LeaveCriticalSection(&mTaskUserActionMapCS);
		if(!ok) return true;

		return _StartDownloadImpl(filemd5);
	}

	//--------------------------------------------------------------------------------------------
	bool Root::_StartDownloadImpl(const char* filemd5)
	{
		SinglePacket* t = _GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		switch(t->state) {
			case P2SPTS_STOP:
			case P2SPTS_READY:
			case P2SPTS_LOAD_RESUME:		// 断点续传待恢复状态, 恢复成功状态切换到P2SPTS_READY			
				{
					ok = mEngineHandler->P2SP_StartTask(filemd5);
					break;
				}

			case P2SPTS_DOWNLOADING:		// 正在下载
			case P2SPTS_QUEUING:			// 正在排队等待下载
			case P2SPTS_COMPLETE:			// 完成状态
			case P2SPTS_ALLOCATE_FREESPACE:	// 空间分配
				{
					break;
				}

			default:
				{
					assert(false);
					ok = false;
					break;
				}
		}
		return ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::StopDownload(const char* filemd5)
	{
		SinglePacket* t = _GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		::EnterCriticalSection(&mTaskInfomationMapCS);
		TaskInfomationMap::iterator it = mTaskInfomationMap.find(filemd5);
		if(it == mTaskInfomationMap.end()) {
			ok = false;
		}
		::LeaveCriticalSection(&mTaskInfomationMapCS);
		if(!ok) return false;

		ok = true;
		::EnterCriticalSection(&mTaskUserActionMapCS);
		TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
		if(i != mTaskUserActionMap.end()) {
			if(!i->second.empty()) {
				ok = false;
				i->second.push_back(TaskUserAction_STOP);
			}
		}
		::LeaveCriticalSection(&mTaskUserActionMapCS);
		if(!ok) return true;

		return _StopDownloadImpl(filemd5);
	}

	//--------------------------------------------------------------------------------------------
	bool Root::_StopDownloadImpl(const char* filemd5)
	{
		SinglePacket* t = _GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		switch(t->state) {
			case P2SPTS_STOP:
			case P2SPTS_READY:
				{
					_ProcessTaskCacheCmd(filemd5, TaskUserAction_STOP, P2SPE_OK);
				}
				break;
			case P2SPTS_DOWNLOADING:		// 正在下载
			case P2SPTS_QUEUING:			// 正在排队等待下载
			case P2SPTS_COMPLETE:			// 完成状态
			case P2SPTS_ALLOCATE_FREESPACE:	// 空间分配
				{
					ok = mEngineHandler->P2SP_StopTask(filemd5);
					break;
				}

			case P2SPTS_LOAD_RESUME:		// 断点续传待恢复状态, 恢复成功状态切换到P2SPTS_READY
				{
					assert(false);
					ok = false;
					break;
				}
		}
		return ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::DeleteDownload(const char* filemd5)
	{	
		SinglePacket* t = this->_GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = true;
		::EnterCriticalSection(&mTaskUserActionMapCS);
		TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
		if(i != mTaskUserActionMap.end()) {
			if(!i->second.empty()) {
				ok = false;
				i->second.push_back(TaskUserAction_DELETE);
			}
		}
		::LeaveCriticalSection(&mTaskUserActionMapCS);
		if(!ok) return true;

		return _DeleteDownloadImpl(filemd5);
	}

	//--------------------------------------------------------------------------------------------
	bool Root::_DeleteDownloadImpl(const char* filemd5)
	{
		SinglePacket* t = this->_GetSinglePacket(filemd5);
		if(!t) return false;

		bool ok = false;
		::EnterCriticalSection(&mTaskInfomationMapCS);
		do {
			TaskInfomationMap::iterator i = mTaskInfomationMap.find(t->md5);
			if(i != mTaskInfomationMap.end()) {
				i = mTaskInfomationMap.erase(i);
				ok = true;
			}
		} while(false);
		::LeaveCriticalSection(&mTaskInfomationMapCS);	

		//无后续命令了。
		::EnterCriticalSection(&mTaskUserActionMapCS);
		TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
		if(i != mTaskUserActionMap.end() && i->second.empty()) {
			i = mTaskUserActionMap.erase(i);
		}
		::LeaveCriticalSection(&mTaskUserActionMapCS);

		return mEngineHandler->P2SP_DeleteTask(filemd5);
	}

	//--------------------------------------------------------------------------------------------
	bool Root::GetTaskInfomation(TaskInfomationT* info)
	{
		assert(info);
		assert(strlen(info->md5) != 0);

		bool ok = false;
		::EnterCriticalSection(&mTaskInfomationMapCS);
		do {
			TaskInfomationMap::iterator i = mTaskInfomationMap.find(info->md5);
			if(i != mTaskInfomationMap.end()) {
				info->complateSize = i->second.complateSize;
				info->dspeed = i->second.dspeed;
				info->err = i->second.err;
				if(0 != StrCmpW(info->savePath, i->second.savePath)) {
					StrCpyW(info->savePath, i->second.savePath);
				}
				info->size = i->second.size;
				info->state = i->second.state;
				info->uspeed = i->second.uspeed;
				ok = true;
			}
		} while(false);
		::LeaveCriticalSection(&mTaskInfomationMapCS);

		std::stringstream sss;
		sss << info->state << ".," << info->err << ".," << (ok? "true" : "false");
		::OutputDebugStringA(sss.str().c_str());

		if(info->state == P2P_TASK_STATE_ERROR/* && info->err == P2SPE_HTTPDOWNLOAD_TRY_TIMES_OVERRUN*/) {
			this->mEngineHandler->P2SP_DeleteTask(info->md5);
		}
		return ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::Stop()
	{
		if(mInitOK) {
			mEngineHandler->uninit();
		}
		Release();
		
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	SinglePacket* Root::_GetSinglePacket(const std::string& file_md5)
	{
		for(GameMap::iterator i = mGameMap.begin(); i != mGameMap.end(); ++i)
		{
			Game* g = i->second;
			if(mGameID == g->gid && mGameID != MAGIC_GAME_ID) {
				SinglePacketMap::iterator it = g->allSinglePackets.find(file_md5);
				if(it != g->allSinglePackets.end()) 
				{
					return it->second;
				}
			}
		}
		return NULL;
	}

	//--------------------------------------------------------------------------------------------
	void Root::_ProcessTaskCacheCmd(const std::string& filemd5, TaskUserAction action, uint32 err)
	{
		bool ok = true;
		TaskUserAction next_ac = TaskUserAction_UNKNOW;
		
		::EnterCriticalSection(&mTaskUserActionMapCS);
		do {
			TaskUserActionMap::iterator i = mTaskUserActionMap.find(filemd5);
			if(i == mTaskUserActionMap.end()) { ok = false; break; }
			if(i->second.empty()) { ok = false; break; }

			TaskUserAction& ac = i->second.front();
			assert(ac == action);
			i->second.pop_front();

			if(err != P2SPE_OK) {
				i->second.clear();
				ok = false; 
				break; 
			}

			if(!i->second.empty()) {
				next_ac = i->second.front();
			}
		}while(false);
		::LeaveCriticalSection(&mTaskUserActionMapCS);
		
		if(!ok) return;
		if(next_ac == TaskUserAction_UNKNOW) return;

		// 执行缓存命令
		switch(next_ac)
		{
		case TaskUserAction_ADD:
			ok = this->_AddDownloadImpl(filemd5.c_str());
			break;

		case TaskUserAction_DELETE:
			ok = this->_DeleteDownloadImpl(filemd5.c_str());
			break;

		case TaskUserAction_START:
			ok = this->_StartDownloadImpl(filemd5.c_str());
			break;

		case TaskUserAction_STOP:
			ok = this->_StopDownloadImpl(filemd5.c_str());
			break;
		}
	}

	//--------------------------------------------------------------------------------------------
	void Root::_AddSinglePacketInfo(const std::string& file_md5, P2SP_TASK_STATE t)
	{
		::EnterCriticalSection(&mTaskInfomationMapCS);
		TaskInfomationMap::iterator i = mTaskInfomationMap.find(file_md5);
		if(i == mTaskInfomationMap.end()) {
			TaskInfomationT task = {0};
			StrCpyNA(task.md5, file_md5.c_str(), (int)file_md5.length());
			task.state = P2P_TASK_STATE_NOREADY;
			mTaskInfomationMap.insert(std::make_pair(file_md5, task));
		}
		::LeaveCriticalSection(&mTaskInfomationMapCS);
	}

	//--------------------------------------------------------------------------------------------
	void Root::_UpdateSinglePacket(const std::string& file_md5, uint32 err)
	{
		SinglePacket* t = _GetSinglePacket(file_md5);
		if(!t) return;

		::EnterCriticalSection(&mTaskInfomationMapCS);
		TaskInfomationMap::iterator i = mTaskInfomationMap.find(file_md5);
		if(i != mTaskInfomationMap.end()) {
			TaskInfomationT& task = i->second;
			task.complateSize = t->progress.complete_size;
			task.dspeed = t->speed.total;
			task.uspeed = t->speed.upload;
			task.size = t->llFilesize;

			// 拷贝所在路径
			if(!t->wsFilename.empty() && !t->wsSavePath.empty()) {
				CFilePath path(t->wsSavePath.c_str());
				path.Append(t->wsFilename.c_str());
				std::wstring wsFilepath(path.value());
				if(wsFilepath.compare(task.savePath) != 0) {
					StrCpyNW(task.savePath, wsFilepath.c_str(), sizeof(task.savePath));
					assert(wsFilepath.compare(task.savePath) == 0);
				}
			}

			do {
				//if(task.state == P2P_TASK_STATE_ERROR && err == P2SPE_OK && t->state == P2SPTS_DOWNLOADING) {
				//}
				//else if(task.state == P2P_TASK_STATE_ERROR && err != P2SPE_TIMEOUT_OFF && t->state != P2SPTS_COMPLETE) {
				//	break;
				//}
				
				if(task.state == P2P_TASK_STATE_ERROR) break;

				// 更新任务状态
				task.err = err;
				
				if(task.err == P2SPE_TIMEOUT_OFF) {
					task.err = P2SPE_OK;
				}

				if(task.err != P2SPE_OK) {
					task.state = P2P_TASK_STATE_ERROR;		
				} 
				else 
				{
					switch(t->state) 
					{
						case P2SPTS_LOAD_RESUME: // 断点续传待恢复状态, 恢复成功状态切换到P2SPTS_READY	
                            task.state = P2P_TASK_STATE_NOREADY;
                            break;
						case P2SPTS_READY:       // 任务已准备好, 可以开始下载(新任务状态从这开始)
							task.state = P2P_TASK_STATE_READY;
							break;

						case P2SPTS_QUEUING:     // 正在排队等待下载
							task.state = P2P_TASK_QUEUING;
							break;

						case P2SPTS_ALLOCATE_FREESPACE:// 增加任务状态为硬盘空间分配
						case P2SPTS_DOWNLOADING: // 正在下载
							task.state = P2P_TASK_STATE_DOWNLOADING;
							break;

						case P2SPTS_STOP:        // 停止状态
							task.state = P2P_TASK_STATE_STOP;
							task.dspeed = 0;
							task.uspeed = 0;
							break;

						case P2SPTS_COMPLETE:    // 完成状态
							task.state = P2P_TASK_STATE_COMPLETE;
							break;

						case P2SPTS_TASK_UNKOWN:

						default:
							{		
								assert(false);
								task.state = P2P_TASK_STATE_ERROR;
							}
							break;
					}//switch
				} //else

				if(task.state == P2P_TASK_STATE_COMPLETE) {
					SaveComplateSinglePacketToConfig(task.md5);
				}

				std::stringstream sss;
				sss << "task: " << task.state << ".," << task.err << ".,";
				::OutputDebugStringA(sss.str().c_str());

			} while(false);
		}
		else 
		{
			std::stringstream sss;
			sss << "task: null!\n";
			::OutputDebugStringA(sss.str().c_str());
		}
		::LeaveCriticalSection(&mTaskInfomationMapCS);
	}

	//--------------------------------------------------------------------------------------------
	void Root::_CheckTaskSpeedTimeOut(const std::string& file_md5, uint32 err)
	{
		if(err != P2SPE_OK) return; 

		SinglePacket* t = _GetSinglePacket(file_md5);
		if(!t) return;
		assert(t->state == P2SPTS_DOWNLOADING);

		TaskMonitorStateMap::iterator i = mTaskMonitorStateMap.find(file_md5);
		if(i == mTaskMonitorStateMap.end()) {
			mTaskMonitorStateMap.insert(TaskMonitorStateMap::value_type(file_md5, TaskMonitorState()));
			i = mTaskMonitorStateMap.find(file_md5);
			i->second.clear();
		}
		assert(i != mTaskMonitorStateMap.end());

		if(i->second.seconds == 0) {
			i->second.clear();
		}

		if(i->second.itimeout_time != mSettingItem.taskTimeout) {
			i->second.clear();
			i->second.itimeout_time = mSettingItem.taskTimeout;
		}
	
		// 一个超时周期，看看速度是否没有达到预期
		if(!!i->second.seconds && i->second.seconds % i->second.itimeout_time == 0)
		{
			i->second.speed_count += t->speed.total;
			uint64& complete = i->second.speed_count;
			uint64 expected = i->second.itimeout_time * mSettingItem.taskTimeoutSpeed;

			if(complete < expected) {
				_UpdateSinglePacket(file_md5, P2SPE_TIMEOUT);
			} else {
				_UpdateSinglePacket(file_md5, P2SPE_TIMEOUT_OFF);
			}
			i->second.seconds = 0;
		} 
		else 
		{ 
			i->second.seconds++;
			i->second.speed_count += t->speed.total;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	bool Root::isUpdateEngineDLL(std::string& xmlContext, tUpdateFileInfo* dllUpdateInfo)
	{
		std::string &sContent = xmlContext;
		Utility::HttpDownloadToString(p2sp4::szConfigXML_URL, sContent);
		if(sContent.empty()) {
			cdcsr::SimpleHttp::download(p2sp4::szConfigXML_URL, &sContent);
		}
		if(sContent.empty()) { 
			
			return false;
		}
		
		TiXmlDocument doc;
		doc.SetUseMicrosoftBOM(true);
		doc.Parse(sContent.c_str(), NULL, TIXML_ENCODING_UTF8);
		if(doc.Error()) return false;
		
		const TiXmlElement* e = Utility::FindTiXmlElement(doc.RootElement(), "update/update_engine_dll_url");
		if(!e || !e->GetText()) return false;
		
		std::string sUpdateURL(e->GetText());
		sUpdateURL.append("update_client/update_client.ini");

		std::string sIniContent;
		Utility::HttpDownloadToString(sUpdateURL, sIniContent);
		if(sIniContent.empty()) {
			cdcsr::SimpleHttp::download(sUpdateURL, &sIniContent);
		}
		if(sIniContent.empty()) {
			return false;
		}

		CSimpleIniW ini(false, false, false);
		if(ini.Load(sIniContent.c_str()) < 0) {
			return false;
		}

		bool ok = false;
		tUpdateFileInfo fileinfo;
		std::wstring section(L"update_FileList");
		std::wstring key(L"file_count");
		std::wstring value = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
		int file_count = _wtoi(value.c_str());
		for(int i = 0; i < file_count; ++i) {
			{
				key.assign(L"filename_");
				key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
				value = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
			}

			if(0 == value.compare(p2sp4::wszEngineDllName)) {
				fileinfo.wsFilename = value;
				{
					key.assign(L"md5_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsMD5 = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					key.assign(L"zipfilename_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsZipFilename = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					key.assign(L"zipmd5_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsZipMD5 = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					key.assign(L"updatemode_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsUpdateMode = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					key.assign(L"action_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsAction = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					key.assign(L"restartexe_");
					key.append(ASCIIToWide(fagex::StringUtil::toString(i).c_str()));
					fileinfo.wsRestartExe = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
				}
				{
					section.assign(L"update_url");
					key.assign(L"url");
					fileinfo.wsUpdateURL = ini.GetValue(section.c_str(), key.c_str(), p2sp4::wsNullString.c_str());
					size_t pos = fileinfo.wsUpdateURL.find_last_of(L'/');
					if(pos != std::wstring::npos && pos != fileinfo.wsUpdateURL.length() - 1) {
						fileinfo.wsUpdateURL += L'/';
					}
				}
				ok = true;
				break;
			}
		}
		if(!ok) return false;

		CFilePath path;
		PathProvider(DIR_EXE, path);
		path.Append(p2sp4::wszEngineDllName);
		if(!path.IsExists()) {
			if(dllUpdateInfo) { *dllUpdateInfo = fileinfo; }
			return true;
		}
		{
			std::wstring wsFileMD5(ASCIIToWide(p2sp4::Utility::FileMD5(path.value()).c_str()));
			if(0 == wsFileMD5.compare(fileinfo.wsMD5)) {
				return false;
			}
		}
		if(dllUpdateInfo) { *dllUpdateInfo = fileinfo; }

		// 检查目录下是否有最新版的文件存在
		if(dllUpdateInfo) {
			CFilePath pathNormalFile;
			PathProvider(DIR_EXE, pathNormalFile);
			pathNormalFile.Append(dllUpdateInfo->wsFilename.c_str());
			if(pathNormalFile.IsExists()) {
				USES_CONVERSION;
				std::wstring wsFileMD5(CT2CW(CA2CT(p2sp4::Utility::FileMD5(pathNormalFile.value()).c_str())));
				if(0 == wsFileMD5.compare(dllUpdateInfo->wsMD5)) {
					return false; //不需要更新了
				}
			}

			// 比对缓存目录的DLL文件
			CFilePath PathDllFile;
			PathProvider(DIR_EXE, PathDllFile);
			PathDllFile.Append(p2sp4::wszRelativelyPath);
			PathDllFile.Append(dllUpdateInfo->wsFilename.c_str());
			if(PathDllFile.IsExists()) {
				USES_CONVERSION;
				std::wstring wsFileMD5(CT2CW(CA2CT(p2sp4::Utility::FileMD5(PathDllFile.value()).c_str())));
				if(0 == wsFileMD5.compare(dllUpdateInfo->wsMD5)) {
					::DeleteFileW(pathNormalFile.value());
					BOOL ok = ::CopyFileW(PathDllFile.value(), pathNormalFile.value(), FALSE);
					if(ok) {
						::OutputDebugStringA("cache dll is copy success!\n");
					} else {
						::OutputDebugStringA("cache dll is copy failed!\n");
					}
					return !ok; 
				} else {
					::DeleteFileW(PathDllFile.value());
				}
			}

			// 比对缓存目录的压缩DLL文件
			CFilePath pathZipFile;
			PathProvider(DIR_EXE, pathZipFile);
			pathZipFile.Append(p2sp4::wszRelativelyPath);
			pathZipFile.Append(dllUpdateInfo->wsZipFilename.c_str());
			if(pathZipFile.IsExists()) {
				USES_CONVERSION;
				std::wstring wsFileMD5(CT2CW(CA2CT(p2sp4::Utility::FileMD5(pathZipFile.value()).c_str())));
				if(0 == wsFileMD5.compare(dllUpdateInfo->wsZipMD5)) {
					// 解压缩
					CFilePath pathFile(pathZipFile);
					pathFile.RemoveFileSpec();
					pathFile.Append(dllUpdateInfo->wsFilename.c_str());
					pc::myLZARI lzari;
					lzari.UnCompressW(pathZipFile.value(), pathFile.value());

					// 拷贝
					if(pathNormalFile.IsExists()) {
						BOOL ok = ::DeleteFileW(pathNormalFile.value());
						if(!ok) {
							::OutputDebugStringA("cache dll is DeleteFileW failed!\n");
							return false; //删除旧文件失败，不更新
						}
					}
					BOOL ok = ::CopyFileW(pathFile.value(), pathNormalFile.value(), FALSE);
					if(ok) {
						::DeleteFileW(pathFile.value());
						::DeleteFileW(pathZipFile.value());
					}
					if(ok) {
						::OutputDebugStringA("cache dll is copy success!\n");
					} else {
						::OutputDebugStringA("cache dll is copy failed!\n");
					}
					return !ok;
				} else {
					::DeleteFileW(pathZipFile.value());
				}
			}
		}
		return true;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::UpdateEngineDLL(const tUpdateFileInfo* dllUpdateInfo)
	{
		bool isNeedDownload = true;
		
		// 检查目录下是否有最新版的文件存在
		CFilePath pathNormalFile;
		PathProvider(DIR_EXE, pathNormalFile);
		pathNormalFile.Append(dllUpdateInfo->wsFilename.c_str());
		if(pathNormalFile.IsExists()) {
			USES_CONVERSION;
			std::wstring wsFileMD5(CT2CW(CA2CT(p2sp4::Utility::FileMD5(pathNormalFile.value()).c_str())));
			if(0 == wsFileMD5.compare(dllUpdateInfo->wsMD5)) {
				return false; //不需要更新了
			}
		}

		// 比对缓存目录的DLL文件
		CFilePath pathZipFile;
		PathProvider(DIR_EXE, pathZipFile);
		pathZipFile.Append(p2sp4::wszRelativelyPath);
		pathZipFile.Append(dllUpdateInfo->wsZipFilename.c_str());
		if(pathZipFile.IsExists()) {
			USES_CONVERSION;
			std::wstring wsFileMD5(CT2CW(CA2CT(p2sp4::Utility::FileMD5(pathZipFile.value()).c_str())));
			if(0 == wsFileMD5.compare(dllUpdateInfo->wsZipMD5)) {
				isNeedDownload = false;
			} else {
				::DeleteFileW(pathZipFile.value());
			}
		}
		
		// 下载
		if(isNeedDownload) {
			EnsureDirectoryForFile(pathZipFile.value(), NULL);

			USES_CONVERSION;
			std::string sURL = CT2CA(CW2CT((dllUpdateInfo->wsUpdateURL + dllUpdateInfo->wsZipFilename).c_str()));
			do {
				bool ok = true;
				std::string buffer;
				cdcsr::SimpleHttp::download(sURL, &buffer);
				if(MD5String(buffer) != WideToUTF8(dllUpdateInfo->wsZipMD5)) break;

				if(!buffer.empty()) {
					cdcsr::MemoryMapFile mmf;
					if(mmf.create(pathZipFile.value(), buffer.size())) {
						DWORD dw = mmf.write(0, (const LPVOID)buffer.c_str(), (DWORD)buffer.size());
						if(dw != (DWORD)buffer.size()) {
							ok = false;
						}
					}
					mmf.close();
					if(!ok) {
						::DeleteFileW(pathZipFile.value());
					}
				}
			} while(false);
		}

		if(!pathZipFile.IsExists()) {
			USES_CONVERSION;
			std::string sURL = CT2CA(CW2CT((dllUpdateInfo->wsUpdateURL + dllUpdateInfo->wsZipFilename).c_str()));
			HttpDownloadToFile(NULL, NULL, sURL.c_str(), pathZipFile.value());
		}
		if(!pathZipFile.IsExists()) {
			return false;
		}
		
		// 解压缩
		CFilePath pathFile(pathZipFile);
		pathFile.RemoveFileSpec();
		pathFile.Append(dllUpdateInfo->wsFilename.c_str());
		pc::myLZARI lzari;
		lzari.UnCompressW(pathZipFile.value(), pathFile.value());

		// 拷贝
		if(pathNormalFile.IsExists()) {
			BOOL ok = ::DeleteFileW(pathNormalFile.value());
			if(!ok) {
				::OutputDebugStringA("isupdatedll step2 delete File pathNormalFile failed!\n");
				return false; //删除旧文件失败，不更新
			}
		}
		BOOL ok = ::CopyFileW(pathFile.value(), pathNormalFile.value(), FALSE);
		if(!ok) {
			::OutputDebugStringA("isupdatedll step2 Copy File pathFile to pathNormalFile failed!\n");
		} else {
			::OutputDebugStringA("isupdatedll update success!\n");
		}
		return !!ok;
	}

	//--------------------------------------------------------------------------------------------
	bool Root::ParseStartEngineParam(const std::string& xmlContext)
	{
		/*
		<?xml version="1.0" encoding="utf-8"?>
		<p2sp>
			<update>
				<update_engine_dll_url>http://61.55.177.188/p2sp_file/update_p2spclient/</update_engine_dll_url>
			</update>
			<config>
				<cis_list>
					<cis id="10001" ip="115.182.50.73" port="7090" arga="NORTH_SOUTH" />
				</cis_list>
				<tunnel_list>
					<tunnel id="20001" ip="115.182.50.73" port="8090" arga="NORTH" />
				</tunnel_list>
				<base_seed_url>http://task.p2sp.wanmei.com/p2sp_n/</base_seed_url>
			<upload_file_log_url>http://115.182.50.72/upload_file_log.php</upload_file_log_url>
			<upload_file_minidump>http://115.182.50.72/upload_file_minidump.php</upload_file_minidump>
			<listen_port>9000</listen_port>
				<default_save_path>C:\PWRD</default_save_path>
				<max_download_speed>5242880</max_download_speed>
				<max_download_task_num>10</max_download_task_num>
				<max_upload_speed>1048576</max_upload_speed>
				<min_use_p2p_size>10485760</min_use_p2p_size>
				<game_list>http://task.p2sp.wanmei.com/p2sp_n/p2sp_game_list.xml</game_list>
				<game_task_list_url>http://task.p2sp.wanmei.com/p2sp_n/</game_task_list_url>
			</config>
		</p2sp>
		*/
		TiXmlDocument doc;
		doc.SetUseMicrosoftBOM(true);
		doc.Parse(xmlContext.c_str(), NULL, TIXML_ENCODING_UTF8);
		if(doc.Error()) return false;

		const TiXmlElement* config = Utility::FindTiXmlElement(doc, "p2sp/config");
		if(!config) return false;

		bool ok = Utility::ParseServerList(config, "cis_list/cis", &Root::ms_StartEngineParam.cis_list);
		if(!ok) return false;
				

		ok = Utility::ParseServerList(config, "tunnel_list/tunnel", &Root::ms_StartEngineParam.tunnel_list);
		if(!ok) return false;

		const char* sz = NULL;
		ok = Utility::GetElementText(config, "base_seed_url", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.s_base_seed_url = (sz);

		ok = Utility::GetElementText(config, "listen_port", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.listen_port = (unsigned short)fagex::StringUtil::parseInt(sz);

		ok = Utility::GetElementText(config, "default_save_path", &sz);
		if(!ok) return false;
		{
			CFilePath path(ASCIIToWide(sz).c_str());
			path.AddBackslash();
			Root::ms_StartEngineParam.save_path_utf8 = WideToUTF8(path.value());
		}

		ok = Utility::GetElementText(config, "upload_file_log_url", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.log_file_url = (sz);
		Root::ms_StartEngineParam.log_file_part_name = "log_file";

		ok = Utility::GetElementText(config, "upload_file_minidump", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.minidump_file_url = (sz);
		Root::ms_StartEngineParam.minidump_file_part_name = "minidump_file";

		ok = Utility::GetElementText(config, "max_download_speed", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.max_download_speed = (size_t)fagex::StringUtil::parseUnsignedInt(sz);

		ok = Utility::GetElementText(config, "max_download_task_num", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.max_downloading_task = (size_t)fagex::StringUtil::parseUnsignedInt(sz);

		ok = Utility::GetElementText(config, "max_upload_speed", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.max_upload_speed = (size_t)fagex::StringUtil::parseUnsignedInt(sz);

		ok = Utility::GetElementText(config, "min_use_p2p_size", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.start_p2p_throttler = (size_t)fagex::StringUtil::parseUnsignedInt(sz);
		Root::ms_StartEngineParam.task_http_connections = 5;
		Root::ms_StartEngineParam.game_id = mGameID;

		ok = Utility::GetElementText(config, "game_list", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.s_game_list_url = (sz);

		ok = Utility::GetElementText(config, "game_task_list_url", &sz);
		if(!ok) return false;
		Root::ms_StartEngineParam.s_game_task_list_url = (sz);

		return true;
	}
	//--------------------------------------------------------------------------------------------
	bool Root::FillPacketList()
	{
		// 获取游戏补丁包文件列表URL
		std::string sGameIndexURL;
		sGameIndexURL.assign(Root::ms_StartEngineParam.s_game_task_list_url);
		sGameIndexURL.append(fagex::StringUtil::toString(mGameID));
		sGameIndexURL.append("/index.xml"); 

		// 下载游戏的索引文件
		std::string sContent;
		Utility::HttpDownloadToString(sGameIndexURL, sContent);
		if(sContent.empty()) return false;
		
		// 获取游戏的服务器版本
		GameMap::iterator i = mGameMap.find(mGameID);
		if(i == mGameMap.end()) return false;
		Game* g = i->second;
		
		TiXmlDocument doc;
		doc.SetUseMicrosoftBOM(true);
		doc.Parse(sContent.c_str(), NULL, TIXML_ENCODING_UTF8);
		if(doc.Error()) return false;
		if(!doc.RootElement()) return false; //xml没有根元素
		const char* szValue = doc.RootElement()->Attribute("server_version");
		if(!szValue) return false;
		g->verServer.assign(szValue);

		/*
		<task pre="0" upgrade="1" tasktype="1" filemd5="6f62d386722eb6185875f4c982c002e9">
			<name>完整客户端1019</name> 
			<update_sequence_number end="1080" start="0"/> 
			<file_size>3184844760</file_size> 
			<update_date>2011-12-24</update_date> 
			<infomation>完整客户端1019</infomation> 
		</task>
		*/
		bool ok = true;
		const TiXmlElement* e = Utility::FindTiXmlElement(doc.RootElement(), "task");
		while(e) {
			SinglePacket t;
			t.gid = mGameID;

			// 任务的MD5码
			szValue = e->Attribute("filemd5");
			if(!szValue) { ok = false; break; }
			t.md5 = szValue;

			szValue = e->Attribute("tasktype");
			if(!szValue) { ok = false; break; }
			t.type = (ePacketType)fagex::StringUtil::parseInt(szValue);

			// 其他属性
			const TiXmlElement* element = e->FirstChildElement();
			while(element) {
				std::string key = element->Value();
				std::string value = element->GetText() ? element->GetText() : "";
				if(key == "name") {
					t.wsPacketName = UTF8ToWide(value);
				} 
				else if(key == "update_sequence_number") { //开始、结束版本
					szValue = element->Attribute("start");
					if(szValue == NULL) { ok = false; break; }
					t.sver.assign(szValue);
					szValue = element->Attribute("end");
					if(szValue == NULL) { ok = false; break; }
					t.ever.assign(szValue);
				} 
				else if(key == "update_date") {
					t.wsDescriptionDate = UTF8ToWide(value);
				} 
				else if(key == "file_size") {
					t.wsDescriptionFilesize = UTF8ToWide(value);
				} 
				else if(key == "infomation") {
					t.wsDescription = UTF8ToWide(value);
				}
				element = element->NextSiblingElement();
			}
			if(!ok) break;
			
			SinglePacket* pt = new SinglePacket();
			*pt = t;
			pt->state = P2SPTS_READY;
			g->allSinglePackets[pt->md5] = pt;

			e = e->NextSiblingElement("task");
		}

		return ok;
	}

	//--------------------------------------------------------------------------------------------
	void Root::CheckSinglePacket(Game* g)
	{
		//客户端最大版本号
		version clientEndVersion;
		SinglePacketMap::iterator i = g->allSinglePackets.begin();
		for(; i != g->allSinglePackets.end(); ++i) {
			SinglePacket* t = i->second;
			if(t->sver == version::zeroVersion && t->ever <= g->verServer
				&& clientEndVersion < t->ever) {
				clientEndVersion = t->ever;
			}
		}
		
		i = g->allSinglePackets.begin();
		for(; i != g->allSinglePackets.end(); ++i) {
			SinglePacket* t = i->second;
			
			// 如果是资源包等其他的包，则直接跳过
			if(t->type == ept_resources_patch || t->type == ept_other_patch 
				|| t->type == ept_unkown) {
					continue;
			}

			// 客户端版本号
			if(t->sver == version::zeroVersion && t->ever <= g->verServer) {
				t->type = ept_client;
			}
			if(t->sver == version::zeroVersion && t->ever > g->verServer) {
				t->type = ept_pre_client;
			}
			
			// 通用
			if(t->sver == clientEndVersion && t->ever == g->verServer) {
				t->type = ept_general_patch;
			}
			if(t->sver == clientEndVersion && t->ever > g->verServer) {
				t->type = ept_pre_general_patch;
			}

			// 最新
			if(t->sver > clientEndVersion && t->ever == g->verServer) {
				t->type = ept_latest_patch;
			}
			if(t->sver > clientEndVersion && t->ever > g->verServer) {
				t->type = ept_pre_latest_patch;
			}
		}
	}

	//--------------------------------------------------------------------------------------------
	void Root::CheckLocalVersion(Game* game)
	{
		Game& g = *game;
		switch(g.gid) 
		{
		case game_wan_mei_shi_jie:
		case game_wan_mei_guo_ji:
			{
				//HKEY_CLASSES_ROOT\CPWUpdatePack\DefaultIcon
				std::wstring wsExecutePath;
				std::wstring wkey;
				if(g.gid == game_wan_mei_shi_jie)
					wkey = L"PWUpdatePack\\DefaultIcon";
				else 
					wkey = L"CPWUpdatePack\\DefaultIcon";

				CRegistryKey registry;
				if(registry.Open(HKEY_CLASSES_ROOT, wkey.c_str(), KEY_ALL_ACCESS)) {
					registry.ReadValue(NULL, wsExecutePath);
					registry.Close();
				}
				if(wsExecutePath.empty()) break;

				size_t pos = wsExecutePath.find_last_of(L',');
				if(pos != std::wstring::npos) {
					wsExecutePath.erase(pos);
				}

				CFilePath wsLauncherPath(wsExecutePath.c_str());
				wsLauncherPath.RemoveFileSpec();
				wsLauncherPath.RemoveLastPart(); // 相当于 .. 操作
				wsLauncherPath.Append(L"config/element/version.sw");

				std::string sConfigContent;
				BOOL ok = fagex::WinUtility::ReadFile(wsLauncherPath.value(), sConfigContent);					
				if(!ok) return;

				fagex::StringUtil::trim(sConfigContent);
				if(sConfigContent.find_first_of(' ') != std::string::npos) {
					sConfigContent = sConfigContent.substr(0, sConfigContent.find_first_of(' '));
				}
				g.verLocal.assign(sConfigContent);
				g.wsExcuteAppPath = wsExecutePath;
			}
			return;

		default:
			break;
		}

		if(g.wsCheckeVersionValue.empty()) return;
		if(g.wsCheckeVersionValue[0] != L'.') 
		{
			const std::wstring wsBaseKey = L"Software\\PWRD\\";
			std::wstring wsKey = wsBaseKey + g.wsCheckeVersionValue;
			std::wstring wsSubKey = wsKey;
			std::wstring wsName;
			std::wstring wsExecutePath;	
			
			switch(g.gid) {
			case game_shen_gui_shi_jie:
			case game_shen_gui_chuan_qi:
				wsSubKey += L"\\Launcher";
				break;

			case game_xiang_long_zhi_jian:
			case game_xiang_long_ji_zhi:
				wsName = L"launcher";
				break;

			default:
				return;
			}

			CRegistryKey registry;
			if(registry.Open(HKEY_CURRENT_USER, wsKey.c_str(), KEY_ALL_ACCESS)) {
				CRegistryKey subRegistry;
				bool ok = subRegistry.Open(HKEY_CURRENT_USER, wsSubKey.c_str(), KEY_ALL_ACCESS);
				if(ok) {
					subRegistry.ReadValue(wsName.empty() ? NULL : wsName.c_str(), wsExecutePath);
					subRegistry.Close();
				}
				registry.Close();
			}
			if(wsExecutePath.empty()) return;

			switch(g.gid) {
			case game_shen_gui_shi_jie:
			case game_shen_gui_chuan_qi:
				{
					if(wsExecutePath.find(L"\"") != std::wstring::npos)
						wsExecutePath = fagex::StringUtil::getValueFormStringW(wsExecutePath, L"\"", L"\"");

					CFilePath path(wsExecutePath.c_str());
					path.RemoveLastPart(); //..
					path.Append(L"LauncherCfg");
					//path.Append(L"NewUpdateInfo.xml"); // 此文件读取是不对的
					path.Append(L"NewUpdateCfg.xml"); // 此文件才是正确的

					std::string strVerFileContent;
					if(!fagex::WinUtility::ReadFile(path.value(), strVerFileContent)) {
						break;
					}
					if(strVerFileContent.empty()) break;

					TiXmlDocument xml;
					xml.Parse(strVerFileContent.c_str(), NULL, TIXML_ENCODING_UTF8);
					if(xml.Error()) break;

					const TiXmlElement* e = Utility::FindTiXmlElement(xml.RootElement(), "Version/CurVersion");
					if(!e) break;

					std::string strVersion = e->GetText() ? e->GetText() : "0";
					fagex::StringUtil::trim(strVersion);
					g.verLocal.assign(strVersion);
					g.wsExcuteAppPath = wsExecutePath;
				}
				return;

			case game_xiang_long_zhi_jian:
			case game_xiang_long_ji_zhi:
				{
					CFilePath path(wsExecutePath.c_str());
					path.RemoveLastPart(); //..
					path.Append(L"update.ini");

					std::string strContent;
					if(!fagex::WinUtility::ReadFile(path.value(), strContent))
						break;

					CSimpleIniW ini(false, false, false);
					if(ini.Load(strContent.c_str()) < 0) {
						break;
					}
					std::wstring section = L"DownLoad";
					std::wstring wstrVersion = ini.GetValue(section.c_str(), L"curversion", L"");

					USES_CONVERSION;
					std::string strVersion = CT2CA(CW2CT(wstrVersion.c_str()));
					fagex::StringUtil::trim(strVersion);
					g.verLocal.assign(strVersion);
					g.wsExcuteAppPath = wsExecutePath;
				}
				return;
			}
		}

		if(g.wsCheckeVersionValue[0] != L'.') return; 
		
		//以下是按照注册表注册文件方式读取的
		WCHAR wcExecutePath[MAX_PATH + 1] = {0};
		DWORD dwLen = MAX_PATH;
		DWORD ASSOCF_INIT_IGNOREUNKNOWN = 0x00000400;
		HRESULT hr = AssocQueryStringW(
			ASSOCF_INIT_IGNOREUNKNOWN | ASSOCF_NOTRUNCATE, ASSOCSTR_EXECUTABLE,  g.wsCheckeVersionValue.c_str(), 
			NULL, wcExecutePath, &dwLen);
		if(hr != S_OK) return;

		CFilePath wsLauncherPath(wcExecutePath);
		switch(g.gid)
		{
		case game_wu_lin_wai_zhuan:			
		case game_zhu_xian:					
		case game_ci_bi:						
		case game_re_wu_pai_dui:				
		case game_kou_dai_xi_you: 
			{
				wsLauncherPath.RemoveFileSpec();
				wsLauncherPath.RemoveLastPart(); // 相当于 .. 操作
				wsLauncherPath.Append(L"config/element/version.sw");

				std::string sConfigContent;
				BOOL ok = fagex::WinUtility::ReadFile(wsLauncherPath.value(), sConfigContent);					
				if(!ok) return;

				fagex::StringUtil::trim(sConfigContent);
				if(sConfigContent.find_first_of(' ') != std::string::npos) {
					sConfigContent = sConfigContent.substr(0, sConfigContent.find_first_of(' '));
				}
				g.verLocal.assign(sConfigContent);
				g.wsExcuteAppPath = wcExecutePath;
			}
			break;

		case game_shen_mo_da_lu:
			{
				wsLauncherPath.RemoveFileSpec();
				wsLauncherPath.Append(L"update/config/element/version.sw");

				std::string sConfigContent;
				BOOL ok = fagex::WinUtility::ReadFile(wsLauncherPath.value(), sConfigContent);					
				if(!ok) return;

				fagex::StringUtil::trim(sConfigContent);
				if(sConfigContent.find_first_of(' ') != std::string::npos) {
					sConfigContent = sConfigContent.substr(0, sConfigContent.find_first_of(' '));
				}
				g.verLocal.assign(sConfigContent);
				g.wsExcuteAppPath = wcExecutePath;
			}
			break;

			// 梦幻诛仙\倚天屠龙记
		case game_meng_huan_zhu_xian:
		case game_yi_tian_tu_long_ji:
		case game_shen_diao_xia_lv:
			{
				CFilePath wsConfigPath(wsLauncherPath);
				wsConfigPath.RemoveFileSpec();
				wsConfigPath.AddBackslash();
				wsConfigPath.Append(L"launcher/config.xml");

				std::wstring wsConfigContent;
				fagex::WinUtility::ReadFileW(wsConfigPath.value(), wsConfigContent);					
				std::string sConfigContent_utf8 = WideToUTF8(wsConfigContent);

				version gameVersion;
				bool ok = false;
				do {
					TiXmlDocument doc;
					doc.Parse(sConfigContent_utf8.c_str(), NULL, TIXML_ENCODING_UTF8);
					if(doc.Error()) break;
					const TiXmlElement* e = Utility::FindTiXmlElement(doc.RootElement(), "launcher/game-client");
					CFilePath pathVersionFile;
					if(!e || e->Attribute("dir") == NULL) break;
					{
						const char* sz = e->Attribute("dir");
						std::wstring wsRelativePath = UTF8ToWide(sz);
						if(wsRelativePath.size() >= 2 && wsRelativePath.substr(0, 2) == L"..") {
							pathVersionFile = wsLauncherPath;
							pathVersionFile.AddExtension(L"del");
							pathVersionFile.RemoveFileSpec();
							if(wsRelativePath.size() > 2) {
								pathVersionFile.AddBackslash();
								pathVersionFile.Append(wsRelativePath.substr(2).c_str());
							}
						}
						else if(wsRelativePath.substr(0, 1) == L".") {
							pathVersionFile = wsLauncherPath;
							if(wsRelativePath.size() > 1) {
								pathVersionFile.AddBackslash();
								pathVersionFile.Append(wsRelativePath.substr(1).c_str());
							}
						} 
					}

					pathVersionFile.AddBackslash();
					pathVersionFile.Append(L".version.meta");
					if(!pathVersionFile.IsExists() && g.gid == game_shen_diao_xia_lv) {
						pathVersionFile = wsLauncherPath;
						pathVersionFile.RemoveFileSpec();
						pathVersionFile.RemoveLastPart();
						pathVersionFile.AddBackslash();
						pathVersionFile.AddBackslash();
						pathVersionFile.Append(L".version.meta");
					}


					// 读取版本文件
					std::wstring wsVersionContent;
					BOOL okay = fagex::WinUtility::ReadFileW(pathVersionFile.value(), wsVersionContent);					
					if(!okay) break;

					std::string sVersionContent_utf8 = WideToUTF8(wsVersionContent);
					TiXmlDocument version_doc;
					version_doc.Parse(sVersionContent_utf8.c_str(), NULL, TIXML_ENCODING_UTF8);
					if(version_doc.Error()) break;
					const TiXmlElement* version_e = version_doc.RootElement();
					std::string sdVersion = version_e->Attribute("version");
					std::string sVersion =  version_e->Attribute("versionstring");
					DWORD dwVersion = (DWORD)fagex::StringUtil::parseUnsignedInt(sdVersion);
					std::string sformat;
					fagex::WinUtility::stdstring_format(sformat, "%d.%d.%d", 
						HIBYTE(HIWORD(dwVersion)), LOBYTE(HIWORD(dwVersion)), LOWORD(dwVersion));
					if(strcmp(sformat.c_str(), sVersion.c_str()) != 0) break;
					gameVersion.assign(sformat);
					ok = true;
				} while(false);
				if(!ok) return;
				g.verLocal = gameVersion;
				g.wsExcuteAppPath.assign(wcExecutePath);
			}
			break;
		}
		return;
	}


	//--------------------------------------------------------------------------------------------
	void Root::SaveComplateSinglePacketToConfig(const std::string& smd5)
	{
		SinglePacket* t = _GetSinglePacket(smd5);
		if(t == NULL) {
			logMessage("%s[%d]: packet is not exist!\n", __FUNCTION__, __LINE__);
			return;
		}
		if(t->state != P2SPTS_COMPLETE) return;

		CFilePath outPath;
		Utility::MakeExecuteAppDir(L"", outPath);
		outPath.Append(L"P2SP\\complete.xml");

		if(!outPath.IsExists()) {
			TiXmlDocument doc;
			doc.SetUseMicrosoftBOM(true);
			TiXmlDeclaration dec("1.0", "utf-8", "");
			doc.InsertEndChild(dec);
			TiXmlElement e("p2sp");
			doc.InsertEndChild(e);

			FILE* fp = NULL;
			_wfopen_s(&fp, outPath.value(), L"wb+");
			if(fp == NULL) return;
			doc.SaveFile(fp);
			fclose(fp);
		}

		std::string strXMLContent;
		if(!fagex::WinUtility::ReadFile(outPath.value(), strXMLContent)) {
			return;
		}

		TiXmlDocument doc;
		doc.Parse(strXMLContent.c_str(), 0, TIXML_ENCODING_UTF8);
		if(doc.Error()) return;
		TiXmlElement *_r = doc.RootElement();

		// 寻找是否存在
		bool find = false;
		const std::string keyName = "packet";
		TiXmlElement *_t = Utility::FindTiXmlElement(_r, keyName);
		while(_t) {
			const char* sz = _t->Attribute("file_md5");
			if(smd5 == sz) {
				find = true; break;
			}
			_t = _t->NextSiblingElement(keyName.c_str());
		}
		if(!find) {
			fagex::StringMap attribute;
			attribute["file_md5"] = smd5;
			_t = Utility::MakeTiXmlElement(_r, keyName, &attribute, NULL, TRUE);
		}

		std::string value = WideToUTF8(t->wsFilename);
		Utility::MakeTiXmlElement(_t, "wsFilename", NULL, &value, false);

		value = fagex::StringUtil::toString((unsigned long long)t->llFilesize);
		Utility::MakeTiXmlElement(_t, "llFilesize", NULL, &value, false);

		value = WideToUTF8(t->wsSavePath);
		Utility::MakeTiXmlElement(_t, "wsSavePath", NULL, &value, false);

		value = fagex::StringUtil::toString((int)t->state);
		Utility::MakeTiXmlElement(_t, "state", NULL, &value, false);

		//时间
		//time_t t_time = (time_t)t->complete.time;
		//tm* t_tm = localtime(&t_time);

		// progress
		TiXmlElement* e = Utility::MakeTiXmlElement(_t, "progress", NULL, NULL, false);
		
		value = fagex::StringUtil::toString((unsigned long long)t->progress.complete_size);
		Utility::MakeTiXmlElement(e, "complete_size", NULL, &value, false);
		
		value = fagex::StringUtil::toString((unsigned long long)t->progress.http_download);
		Utility::MakeTiXmlElement(e, "http_download", NULL, &value, false);

		value = fagex::StringUtil::toString((unsigned long long)t->progress.http_saved);
		Utility::MakeTiXmlElement(e, "http_saved", NULL, &value, false);

		value = fagex::StringUtil::toString((unsigned long long)t->progress.p2p_download);
		Utility::MakeTiXmlElement(e, "p2p_download", NULL, &value, false);

		value = fagex::StringUtil::toString((unsigned long long)t->progress.p2p_saved);
		Utility::MakeTiXmlElement(e, "p2p_saved", NULL, &value, false);

		value = fagex::StringUtil::toString((int)t->progress.seconds);
		Utility::MakeTiXmlElement(e, "seconds", NULL, &value, false);

		//Complete
		e = Utility::MakeTiXmlElement(_t, "complete", NULL, NULL, false);
		value = fagex::StringUtil::toString((unsigned long long)t->complete.time);
		Utility::MakeTiXmlElement(e, "time", NULL, &value, false);

		FILE* fp = NULL;
		_wfopen_s(&fp, outPath.value(), L"wb+");
		if(fp == NULL) return;
		doc.SaveFile(fp);
		fclose(fp);
	}

	//--------------------------------------------------------------------------------------------
	void Root::RestoreComplateSinglePacketFormConfig()
	{
		CFilePath outPath;
		Utility::MakeExecuteAppDir(L"", outPath);
		outPath.Append(L"P2SP\\complete.xml");

		std::string strXMLContent;
		if(!fagex::WinUtility::ReadFile(outPath.value(), strXMLContent))
			return;

		TiXmlDocument doc;
		doc.Parse(strXMLContent.c_str(), 0, TIXML_ENCODING_UTF8);
		if(doc.Error()) return;

		// 正常处理流程
		std::string keyName = "packet";
		TiXmlElement* ei = Utility::FindTiXmlElement(doc.RootElement(), keyName);
		while(ei) {
			const char *szValue = ei->Attribute("file_md5");
			do {
				if(szValue == NULL) break;
				SinglePacket* t = _GetSinglePacket(szValue);
				if(t == NULL) break;

				// 如果任务的状态不是P2SPTS_COMPLETE 不恢复
				TiXmlElement* e = ei->FirstChildElement("state");
				if(NULL == (szValue = e->GetText())) break;
				P2SP_TASK_STATE t_state = (P2SP_TASK_STATE)atoi(szValue);
				if(t_state != P2SPTS_COMPLETE) break;

				//判断文件是否存在
				e = ei->FirstChildElement("wsFilename");
				if(NULL == (szValue = e->GetText())) break;
				std::wstring wsfilename = UTF8ToWide(szValue);
				if(wsfilename.empty()) break;

				e = ei->FirstChildElement("wsSavePath");
				szValue = e->GetText();
				if(NULL == szValue) break;
				std::wstring wsfilepath = UTF8ToWide(szValue);

				CFilePath path(wsfilepath.c_str());
				path.Append(wsfilename.c_str());
				if(!path.IsExists()) break;

				e = ei->FirstChildElement("llFilesize");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->llFilesize = (unsigned __int64)_atoi64(szValue);

				// progress
				e = Utility::FindTiXmlElement(ei, "progress/complete_size");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.complete_size = (unsigned __int64)_atoi64(szValue);

				e = Utility::FindTiXmlElement(ei, "progress/http_download");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.http_download = (unsigned __int64)_atoi64(szValue);

				e = Utility::FindTiXmlElement(ei, "progress/http_saved");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.http_saved = (unsigned __int64)_atoi64(szValue);

				e = Utility::FindTiXmlElement(ei, "progress/p2p_download");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.p2p_download = (unsigned __int64)_atoi64(szValue);

				e = Utility::FindTiXmlElement(ei, "progress/p2p_saved");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.p2p_saved = (unsigned __int64)_atoi64(szValue);

				e = Utility::FindTiXmlElement(ei, "progress/seconds");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->progress.seconds = (unsigned __int32)fagex::StringUtil::parseInt(szValue);

				// complete
				e = Utility::FindTiXmlElement(ei, "complete/time");
				szValue = e->GetText();
				if(NULL == szValue) break;
				t->complete.time = (uint64)fagex::StringUtil::parseUnsignedLongLong(szValue);

				t->state = t_state;
				t->wsFilename = wsfilename;
				t->wsSavePath = wsfilepath;

			} while(false);
			ei = ei->NextSiblingElement(keyName.c_str());
		}
	}
	//--------------------------------------------------------------------------------------------
	void Root::logMessage(const char* format, ...)
	{
		std::string s;
		va_list ap;
		va_start(ap, format);
		{
			size_t len = ::_vscprintf(format, ap) + 1;
			char* buffer = new char[len];
			memset(buffer, 0, len);
			::vsprintf_s(buffer, len, format, ap);
			s.assign(buffer, len);
			delete buffer;
		}
		va_end(ap);
		::OutputDebugStringA(s.c_str());
	}

	bool Root::engineSetPriorityDownloads()
	{
		if(mEngineHandler) return mEngineHandler->engineSetPriorityDownloads();
		return false;
	}

	bool Root::engineSetPriorityBrowse()
	{
		if(mEngineHandler) return mEngineHandler->engineSetPriorityBrowse();
		return false;
	}

	bool Root::engineRemoveFilter()
	{
		if(mEngineHandler) return mEngineHandler->engineRemoveFilter();
		return false;
	}

}
//////////////////////////////////////////////////////////////////////////////////////////////////
bool p2spStart(int gameID)
{
	if(p2sp4::Root::getSingletonPtr()) return true;
	p2sp4::Root* r = new p2sp4::Root();
	bool ok = r->Start(gameID);
	if(!ok) {
		p2spStop();
	}
	return ok;
}

//------------------------------------------------------------------------------------------------
bool p2spSetting(const SettingItemT* setting)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->Setting(setting);
}

//------------------------------------------------------------------------------------------------
bool p2spGetSetting(SettingItemT* setting)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->GetSetting(setting);
}

//------------------------------------------------------------------------------------------------
bool p2spAddDownload(const char* filemd5)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->AddDownload(filemd5);
}

//------------------------------------------------------------------------------------------------
bool p2spStartDownload(const char* filemd5)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->StartDownload(filemd5);
}

//------------------------------------------------------------------------------------------------
bool p2spStopDownload(const char* filemd5)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->StopDownload(filemd5);
}

//------------------------------------------------------------------------------------------------
bool p2spDeleteDownload(const char* filemd5)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->DeleteDownload(filemd5);
}

//------------------------------------------------------------------------------------------------
bool p2spGetTaskInfomation(TaskInfomationT* info)
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->GetTaskInfomation(info);
}

//------------------------------------------------------------------------------------------------
bool p2spSetLsp( LSP_SETTING_FLAG flag )
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	switch(flag)
	{
	case LSP_SETTING_PRIORITY_DOWNLOADS:
		return p2sp4::Root::getSingletonPtr()->engineSetPriorityDownloads();

	case LSP_SETTING_PRIORITY_BROWSE:
		return p2sp4::Root::getSingletonPtr()->engineSetPriorityBrowse();

	case LSP_SETTING_REMOVE:
		return p2sp4::Root::getSingletonPtr()->engineRemoveFilter();
	}
	return false;
}

//------------------------------------------------------------------------------------------------
bool p2spStop()
{
	if(!p2sp4::Root::getSingletonPtr()) return false;
	return p2sp4::Root::getSingletonPtr()->Stop();
}
