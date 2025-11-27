

#include "p2spEngineHandler.h"
#include "p2spRoot.h"

#define P2P_SETTING_ITEM_MAX_MD5_RETRYS_VALUE_NAME	"setting/bbm_piece_max_retrys"
#define P2P_SETTING_ITEM_GAMEID_VALUE_NAME			"setting/gameid"

namespace p2sp4 
{
	bool EngineHandler::init(const CFilePath& pathDLL, const P2SPStartEngineParam& params)
	{
		_hmodule = ::LoadLibraryW(pathDLL.value());
		if(!_hmodule) {
			return false;
		}

		_P2SP_StartEngine		= (__P2SP_StartEngine)::GetProcAddress(_hmodule, "P2SP_StartEngine");
		_P2SP_StopEngine		= (__P2SP_StopEngine)::GetProcAddress(_hmodule, "P2SP_StopEngine");
		_P2SP_ResumeAllTasks	= (__P2SP_ResumeAllTasks)::GetProcAddress(_hmodule, "P2SP_ResumeAllTasks");
		_P2SP_AddTask			= (__P2SP_AddTask)::GetProcAddress(_hmodule, "P2SP_AddTask");
		_P2SP_StartTask			= (__P2SP_StartTask)::GetProcAddress(_hmodule, "P2SP_StartTask");
		_P2SP_StopTask			= (__P2SP_StopTask)::GetProcAddress(_hmodule, "P2SP_StopTask");
		_P2SP_DeleteTask		= (__P2SP_DeleteTask)::GetProcAddress(_hmodule, "P2SP_DeleteTask");
		_P2SP_UserAction		= (__P2SP_UserAction)::GetProcAddress(_hmodule, "P2SP_UserAction");

		_P2SP_StopTask			= (__P2SP_StopTask)::GetProcAddress(_hmodule, "P2SP_StopTask");
		_P2SP_DeleteTask		= (__P2SP_DeleteTask)::GetProcAddress(_hmodule, "P2SP_DeleteTask");
		_P2SP_UserAction		= (__P2SP_UserAction)::GetProcAddress(_hmodule, "P2SP_UserAction");

		_engineSetPriorityDownloads = (__engineSetPriorityDownloads)::GetProcAddress(_hmodule, "engineSetPriorityDownloads");
		_engineSetPriorityBrowse = (__engineSetPriorityBrowse)::GetProcAddress(_hmodule, "engineSetPriorityBrowse");
		_engineRemoveFilter = (__engineRemoveFilter)::GetProcAddress(_hmodule, "engineRemoveFilter");

		ATLVERIFY(_P2SP_StartEngine && _P2SP_StopEngine && _P2SP_ResumeAllTasks 
			&& _P2SP_AddTask && _P2SP_StartTask && _P2SP_StopTask && _P2SP_DeleteTask && _P2SP_UserAction); 

		ATLVERIFY(this->P2SP_StartEngine(params, this));
		_wait_load_resume_task = false;
		this->P2SP_UserAction(0x4567, 0, 0);
        this->P2SP_ResumeAllTasks();
		
		while(!_wait_load_resume_task) {
			Sleep(100);
		}
		//this->P2SP_UserAction(eUserAction_LoadResumeTask, 0, 0);

		return true;
	}

	//--------------------------------------------------------------------------------------------------------
	bool EngineHandler::uninit()
	{
		this->P2SP_StopEngine();
		return !!::FreeLibrary(_hmodule);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnUserAction(IP2SPEngine* engine, UINT action, WPARAM wparam, LPARAM lparam)
	{
		switch(action)
		{
        case 0x4567:
            {
                size_t tasks = engine->TaskCount();
                for(size_t i = 0; i < tasks; ++i) {
                    IP2SPTask* task = engine->Task(i);
                    SinglePacket* t = Root::getSingleton()._GetSinglePacket(task->FileMD5());
                    if(t != NULL) {
                        t->tid = task->Id();
                        t->wsSavePath = UTF8ToWide(task->SavePath());
                        t->wsFilename = UTF8ToWide(task->FileName());
                        t->llFilesize = task->FileSize();
                        t->state = task->State();
                        t->progress.complete_size = task->FileCompleteSize();
                        t->progress.seconds = 0;
                        Root::getSingletonPtr()->_AddSinglePacketInfo(task->FileMD5(), task->State());
					} else {
						this->P2SP_DeleteTask(task->FileMD5());
					}
                }
				_wait_load_resume_task = true;
                
            }
            break;

		case P2P_SETTING_ITEM_MAX_DOWNLOAD_NUM:
			{
				engine->MaxDownloadingTask((size_t)wparam);
			}	
			break;

		case P2P_SETTING_ITEM_MAX_DOWNLOAD_SPEED:
			{
				engine->MaxDownloadSpeed((size_t)wparam);
			}
			break;

		case P2P_SETTING_ITEM_MAX_UPLOAD_SPEED:
			{
				engine->MaxUploadSpeed((size_t)wparam);
			}
			break;

		case P2P_SETTING_ITEM_FILE_SAVE_PATH:
			{
				std::string* save_path_utf8 = (std::string*)wparam;
				engine->TaskSaveDir(save_path_utf8->c_str());
				delete save_path_utf8;
			}
			break;

		//case P2P_SETTING_ITEM_MAX_MD5_RETRYS:
		//	{
		//		int md5_checksum_retrys = (size_t)wparam;
		//		if(0) {
		//			typedef VOID (*_SetPairValueInt)(const char*, int);
		//			_SetPairValueInt fpSetPairValueInt = (_SetPairValueInt)::GetProcAddress(_hmodule, "P2SP_SetPairValueInt");
		//			if(fpSetPairValueInt) {
		//				fpSetPairValueInt(P2P_SETTING_ITEM_MAX_MD5_RETRYS_VALUE_NAME, md5_checksum_retrys);
		//			}
		//		}
		//	}
		default:
			break;
		}
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnResumeTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err)
	{
		switch(err)
		{
		case P2SPE_OK:
			{
				IP2SPTask* task = engine->Task(file_md5);
				SinglePacket* t = Root::getSingleton()._GetSinglePacket(file_md5);
				if(t == NULL) {
					Root::getSingletonPtr()->logMessage("%s:don't find packet %s\n", __FUNCTION__, file_md5);				
					break;
				}
				ATLASSERT(t->md5 == file_md5);
				t->tid = task->Id();
				t->wsSavePath = UTF8ToWide(task->SavePath());
				t->wsFilename = UTF8ToWide(task->FileName());
				t->llFilesize = task->FileSize();
				t->state = task->State();
				t->progress.complete_size = task->FileCompleteSize();
				t->progress.seconds = 0;
			}
			break;

		case P2SPE_PARSE_TASK_SEED:
		case P2SPE_DOWNLOAD_TASK_SEED:
		case P2SPE_CHECK_TASK_SEED:
			{
				Root::getSingletonPtr()->logMessage("%s:fail %d\n", __FUNCTION__, err);				
			}
			break;
		}
		Root::getSingletonPtr()->_UpdateSinglePacket(file_md5, err);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnAddTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err)
	{
        uint32 _err = err;
		switch(err)
		{
		case P2SPE_TASK_ALREADY_EXISTS:
		case P2SPE_OK:
			{
				IP2SPTask* task = engine->Task(file_md5);
				SinglePacket* t = Root::getSingleton()._GetSinglePacket(file_md5);
				if(t == NULL) {
					Root::getSingletonPtr()->logMessage("%s:don't find packet %s\n", __FUNCTION__, file_md5);				
					break;
				}
				ATLASSERT(t->md5 == file_md5);
				t->tid = task->Id();
				t->wsSavePath = UTF8ToWide(task->SavePath());
				t->wsFilename = UTF8ToWide(task->FileName());
				t->llFilesize = task->FileSize();
				t->state = task->State();
				t->progress.complete_size = task->FileCompleteSize();
				t->progress.seconds = 0;
			}
            _err = P2SPE_OK;
			break;  

		case P2SPE_DOWNLOAD_TASK_SEED:
		case P2SPE_PARSE_TASK_SEED:
		case P2SPE_CHECK_TASK_SEED:
			{
				Root::getSingletonPtr()->logMessage("%s:fail %d\n", __FUNCTION__, _err);				
			}
			break;
		}
	
		Root::getSingletonPtr()->_UpdateSinglePacket(file_md5, _err);
		Root::getSingletonPtr()->_ProcessTaskCacheCmd(file_md5, Root::TaskUserAction_ADD, _err);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnStartTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err)
	{
		P2SP_ERROR _err = err;
		switch(err)
		{
		case P2SPE_STARTTASK_QUEUING:
		case P2SPE_OK:
			{
				IP2SPTask* task = engine->Task(file_md5);
				SinglePacket* t = Root::getSingleton()._GetSinglePacket(file_md5);
				if(t == NULL) {
					Root::getSingletonPtr()->logMessage("%s:don't find packet %s\n", __FUNCTION__, file_md5);				
					break;
				}
				ATLASSERT(t->md5 == file_md5);
				t->state = task->State();

				t->speed.http = (uint32)engine->TaskHttpDownloadSpeed(task->FileMD5());
				t->speed.p2p = (uint32)engine->TaskP2PDownloadSpeed(task->FileMD5());
				t->speed.total = (uint32)engine->TaskDownloadSpeed(task->FileMD5());
				t->speed.upload = (uint32)engine->TaskUploadSpeed(task->FileMD5());
			}
			_err = P2SPE_OK;
			break;

		case P2SPE_STARTTASK_INEXISTENT:
		case P2SPE_STARTTASK_DISKFULL:
		case P2SPE_STARTTASK_CREATE_FILE_ERROR:
			{
				Root::getSingletonPtr()->logMessage("%s:fail %d\n", __FUNCTION__, err);				
			}
			break;
		}
		Root::getSingletonPtr()->_UpdateSinglePacket(file_md5, _err);
		Root::getSingletonPtr()->_ProcessTaskCacheCmd(file_md5, Root::TaskUserAction_START, _err);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnStopTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err)
	{
		switch(err)
		{
		case P2SPE_OK:
			{
				IP2SPTask* task = engine->Task(file_md5);
				SinglePacket* t = Root::getSingleton()._GetSinglePacket(file_md5);
				if(t == NULL) {
					Root::getSingletonPtr()->logMessage("%s:don't find packet %s\n", __FUNCTION__, file_md5);				
					break;
				}
				ATLASSERT(t->md5 == file_md5);
				t->state = task->State();
			}
			break;

		case P2SPE_STARTTASK_INEXISTENT:
		default:
			{
				IP2SPTask* task = engine->Task(file_md5);
				SinglePacket* t = Root::getSingleton()._GetSinglePacket(file_md5);
				if(t == NULL) {
					Root::getSingletonPtr()->logMessage("%s:don't find packet %s\n", __FUNCTION__, file_md5);				
					break;
				}
				ATLASSERT(t->md5 == file_md5);
				t->state = task->State();
			}

			break;
		}
		Root::getSingletonPtr()->_UpdateSinglePacket(file_md5, err);
		Root::getSingletonPtr()->_ProcessTaskCacheCmd(file_md5, Root::TaskUserAction_STOP, err);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnDeleteTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err)
	{
		SinglePacket* t = Root::getSingletonPtr()->_GetSinglePacket(file_md5);
		if(!t && "此时应该是未下载任务已经在服务器端被删除") {
			return;
		}

		switch(err)
		{
		case P2SPE_OK:
			break;

		default:
			{
				CFilePath path(t->wsSavePath.c_str());
				path.Append(t->wsFilename.c_str());
				if(path.IsExists()) {
					::DeleteFileW(path.value());
				}
			}
			break;
		}

		P2SP_TASK_STATE state = t->state;
		t->complete.clear();
		t->progress.clear();
		t->speed.clear();
        t->wsSavePath.clear();
        t->wsFilename.clear();
		t->state = P2SPTS_READY;

		Root::getSingletonPtr()->_ProcessTaskCacheCmd(file_md5, Root::TaskUserAction_DELETE, err);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnHttpDownloadTrace(IP2SPEngine* engine, IP2SPTaskHTTPClient* client, P2SP_TRACE_TYPE type, const char* message)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnHttpDownloadError(IP2SPEngine* engine, IP2SPTaskHTTPClient* client, P2SP_ERROR err)
	{
		std::string file_md5 = client->FileMD5();
		
		int error = err;
		if(error == P2SPE_HTTPDOWNLOAD_TRY_TIMES_OVERRUN) {
			Root::getSingletonPtr()->_UpdateSinglePacket(file_md5, err);
			
			std::stringstream sss;
			sss << "EngineHandler::OnHttpDownloadError: " << err << "..................";
			::OutputDebugStringA(sss.str().c_str());
		} else 
		{
			std::stringstream sss;
			sss << "EngineHandler::OnHttpDownloadError: " << err << "------------------";
			::OutputDebugStringA(sss.str().c_str());
		}
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnP2PDownloadTrace(const char* file_md5, const char* message)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnP2PDownloadTrace(const char* message)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnTaskProgress(IP2SPEngine* engine, IP2SPTask* task)
	{
		SinglePacket* t = Root::getSingleton()._GetSinglePacket(task->FileMD5());
		assert(t);

		t->progress.complete_size = (uint64)task->FileCompleteSize();
		t->progress.http_download = (uint64)engine->TaskTotalHttpDownload(task->FileMD5());
		t->progress.http_saved = (uint64)engine->TaskTotalHttpSaved(task->FileMD5());
		t->progress.p2p_download = (uint64)engine->TaskTotalP2PDownload(task->FileMD5());
		t->progress.p2p_saved = (uint64)engine->TaskTotalP2PSaved(task->FileMD5());
		//t->progress.seconds ++;

		return Root::getSingletonPtr()->_UpdateSinglePacket(t->md5, P2SPE_OK);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnTaskBlockComplete(IP2SPEngine* engine, IP2SPTask* task, size_t block_index)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnTaskComplete(IP2SPEngine* engine, IP2SPTask* task)
	{
		SinglePacket* t = Root::getSingleton()._GetSinglePacket(task->FileMD5());
		assert(t);

		t->state = task->State();
		t->complete.time = (uint64)time(NULL);
		t->speed.clear();

		Root::getSingletonPtr()->_UpdateSinglePacket(t->md5, P2SPE_OK);
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnBeginTaskSpeed(IP2SPEngine* engine)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnTaskSpeed(IP2SPEngine* engine, IP2SPTask* task)
	{
		SinglePacket* t = Root::getSingleton()._GetSinglePacket(task->FileMD5());
		assert(t);

		t->speed.http = (uint32)engine->TaskHttpDownloadSpeed(task->FileMD5());
		t->speed.p2p = (uint32)engine->TaskP2PDownloadSpeed(task->FileMD5());
		t->speed.total = (uint32)engine->TaskDownloadSpeed(task->FileMD5());
		t->speed.upload = (uint32)engine->TaskUploadSpeed(task->FileMD5());
		t->progress.seconds += 1;

		Root::getSingletonPtr()->_UpdateSinglePacket(t->md5, P2SPE_OK);
		
		if(t->state == P2SPTS_DOWNLOADING)
		{
			Root::getSingletonPtr()->_CheckTaskSpeedTimeOut(t->md5, P2SPE_OK);
		}
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnEndTaskSpeed(IP2SPEngine* engine)
	{
	}

	//--------------------------------------------------------------------------------------------------------
	void EngineHandler::OnTaskUpdateState(IP2SPEngine* engine, IP2SPTask* task)
	{
		SinglePacket* t = Root::getSingleton()._GetSinglePacket(task->FileMD5());
		assert(t);

		t->state = task->State();
		Root::getSingletonPtr()->_UpdateSinglePacket(t->md5, P2SPE_OK);
	}

}