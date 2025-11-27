

#ifndef __p2spEngineHandler_h__
#define __p2spEngineHandler_h__

#pragma once
#include "p2spInternalStruct.h"
#include "p2spStableHeadres.h"

namespace p2sp4 {
	class EngineHandler : public IP2SPEngineCallback
	{
	public:
		enum eUserAction {
			eUserAction_CheckResumeTask		= 3,
			eUserAction_LoadResumeTask		= 5,
		};

		EngineHandler() {
			_P2SP_StartEngine		= NULL;
			_P2SP_StopEngine		= NULL;
			_P2SP_ResumeAllTasks	= NULL;
			_P2SP_AddTask			= NULL;
			_P2SP_StartTask			= NULL;
			_P2SP_StopTask			= NULL;
			_P2SP_DeleteTask		= NULL;
			_P2SP_UserAction		= NULL;
			_hmodule				= NULL;

			_engineSetPriorityDownloads = NULL;
			_engineSetPriorityBrowse = NULL;
			_engineRemoveFilter = NULL;
		}
		~EngineHandler() { }

		bool init(const CFilePath& pathDLL, const P2SPStartEngineParam& param);
		bool uninit();

	public:
		// 启动引擎
		bool P2SP_StartEngine(const P2SPStartEngineParam& param, IP2SPEngineCallback* callback) 
		{
			return _P2SP_StartEngine(param, callback);
		}

		// 停止引擎
		void P2SP_StopEngine(void)
		{
			return _P2SP_StopEngine();
		}

		// 恢复上次未完成的任务
		bool P2SP_ResumeAllTasks(void)	
		{
			return _P2SP_ResumeAllTasks();
		}

		bool P2SP_AddTask(const char* file_md5, const char* task_save_path_utf8)
		{
			return _P2SP_AddTask(file_md5, task_save_path_utf8);
		}

		// 开始下载指定任务
		bool P2SP_StartTask(const char* file_md5) 
		{
			return _P2SP_StartTask(file_md5);
		}

		// 停止下载指定任务
		bool P2SP_StopTask(const char* file_md5) 
		{
			return _P2SP_StopTask(file_md5);
		}

		// 删除指定任务(删除下载文件)
		bool P2SP_DeleteTask(const char* file_md5) 
		{
			return _P2SP_DeleteTask(file_md5);
		}

		// 用户回调动作
		bool P2SP_UserAction(UINT action, WPARAM wparam, LPARAM lparam) 
		{
			return _P2SP_UserAction(action, wparam, lparam);
		}

		bool engineSetPriorityDownloads() {
			if(!_engineSetPriorityDownloads) return false;
			return _engineSetPriorityDownloads();
		}

		bool engineSetPriorityBrowse() { 
			if(!_engineSetPriorityBrowse) return false;
			return _engineSetPriorityBrowse();
		}
		
		bool engineRemoveFilter() {
			if(!_engineRemoveFilter) return false;
			return _engineRemoveFilter();
		}

	private:
		virtual void OnResumeTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err);
		virtual void OnAddTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err);
		virtual void OnStartTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err);
		virtual void OnStopTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err);
		virtual void OnDeleteTask(IP2SPEngine* engine, const char* file_md5, P2SP_ERROR err);
		virtual void OnUserAction(IP2SPEngine* engine, UINT action, WPARAM wparam, LPARAM lparam);
		virtual void OnHttpDownloadTrace(IP2SPEngine* engine, IP2SPTaskHTTPClient* client, P2SP_TRACE_TYPE type, const char* message);
		virtual void OnHttpDownloadError(IP2SPEngine* engine, IP2SPTaskHTTPClient* client, P2SP_ERROR err);
		virtual void OnP2PDownloadTrace(const char* file_md5, const char* message);
		virtual void OnP2PDownloadTrace(const char* message);
		virtual void OnTaskProgress(IP2SPEngine* engine, IP2SPTask* task);
		virtual void OnTaskBlockComplete(IP2SPEngine* engine, IP2SPTask* task, size_t block_index);
		virtual void OnTaskComplete(IP2SPEngine* engine, IP2SPTask* task);
		virtual void OnBeginTaskSpeed(IP2SPEngine* engine);
		virtual void OnTaskSpeed(IP2SPEngine* engine, IP2SPTask* task);
		virtual void OnEndTaskSpeed(IP2SPEngine* engine);
		virtual void OnTaskUpdateState(IP2SPEngine* engine, IP2SPTask* task) override;

	private:
		typedef bool (*__P2SP_StartEngine)(const P2SPStartEngineParam&, IP2SPEngineCallback*);// 启动引擎
		typedef void (*__P2SP_StopEngine)(void);// 停止引擎
		typedef bool (*__P2SP_ResumeAllTasks)(void);	// 恢复上次未完成的任务
		typedef bool (*__P2SP_AddTask)(const char* file_md5, const char* task_save_path_utf8);
		typedef bool (*__P2SP_StartTask)(const char* file_md5);		// 开始下载指定任务
		typedef bool (*__P2SP_StopTask)(const char* file_md5); // 停止下载指定任务
		typedef bool (*__P2SP_DeleteTask)(const char* file_md5); // 删除指定任务(删除下载文件)
		typedef bool (*__P2SP_UserAction)(UINT action, WPARAM wparam, LPARAM lparam); // 用户回调动作

		/// 设置优先下载
		typedef bool (*__engineSetPriorityDownloads)();
		/// 设置优先上网
		typedef bool (*__engineSetPriorityBrowse)();
		/// 删除SPI层LSP过滤，恢复到原状
		typedef bool (*__engineRemoveFilter)();

		__P2SP_StartEngine		_P2SP_StartEngine;
		__P2SP_StopEngine		_P2SP_StopEngine;
		__P2SP_ResumeAllTasks	_P2SP_ResumeAllTasks;
		__P2SP_AddTask			_P2SP_AddTask;
		__P2SP_StartTask		_P2SP_StartTask;
		__P2SP_StopTask			_P2SP_StopTask;
		__P2SP_DeleteTask		_P2SP_DeleteTask;
		__P2SP_UserAction		_P2SP_UserAction;
		__engineSetPriorityDownloads _engineSetPriorityDownloads;
		__engineSetPriorityBrowse _engineSetPriorityBrowse;
		__engineRemoveFilter _engineRemoveFilter;

		HMODULE _hmodule;
		volatile bool _wait_load_resume_task;
	};
}
#endif //__p2spEngineHandler_h__