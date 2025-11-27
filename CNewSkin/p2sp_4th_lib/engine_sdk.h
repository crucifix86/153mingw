
#ifndef __p2sp_engine_sdk_h__
#define __p2sp_engine_sdk_h__

#include <Windows.h>

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64	uint64;
typedef unsigned __int64	TASKID;

#ifdef P2SPENGINESDK_EXPORT
#define P2SPENGINESDK_PUBLIC    __declspec(dllexport)
#else
#define P2SPENGINESDK_PUBLIC    __declspec(dllimport)
#endif

// 错误码
enum P2SP_ERROR
{
    P2SPE_OK,

    P2SPE_TASK_ALREADY_EXISTS,
    P2SPE_DOWNLOAD_TASK_SEED,
    P2SPE_PARSE_TASK_SEED,
    P2SPE_CHECK_TASK_SEED,

    P2SPE_STARTTASK_INEXISTENT,
    P2SPE_STARTTASK_DISKFULL,
    P2SPE_STARTTASK_QUEUING,

    P2SPE_HTTPDOWNLOAD_PARSEURL,
    P2SPE_HTTPDOWNLOAD_RESOLVEHOST,
    P2SPE_HTTPDOWNLOAD_CONNECTSERVER,
    P2SPE_HTTPDOWNLOAD_REQUEST,
    P2SPE_HTTPDOWNLOAD_READSTATUSLINE,
    P2SPE_HTTPDOWNLOAD_UNSUPPORTEPARTIAL,
    P2SPE_HTTPDOWNLOAD_READHEADERS,
    P2SPE_HTTPDOWNLOAD_READDATA,
    
	P2SPE_HTTPDOWNLOAD_TRY_TIMES_OVERRUN,	//verrun
	P2SPE_STARTTASK_CREATE_FILE_ERROR,

    P2SPE__COUNT,
};

// 任务状态
enum P2SP_TASK_STATE
{
	P2SPTS_TASK_UNKOWN = -1,

    P2SPTS_LOAD_RESUME, // 断点续传待恢复状态, 恢复成功状态切换到P2SPTS_READY
    P2SPTS_READY,       // 任务已准备好, 可以开始下载(新任务状态从这开始)
    P2SPTS_DOWNLOADING, // 正在下载
    P2SPTS_QUEUING,     // 正在排队等待下载
    P2SPTS_STOP,        // 停止状态
    P2SPTS_COMPLETE,    // 完成状态

	// 增加任务状态为硬盘空间分配
	P2SPTS_ALLOCATE_FREESPACE,

	// 此状态多任务专用，表示添加添加过程中
	P2SPTS_MTASK_ADDING, //过渡状态，不能操作
	P2SPTS_MTASK_ADDED,	//添加成功, 过渡状态不能操作
	
	// 正在停止
	P2SPTS_MTASK_STOPING,

	// 正在删除任务
	P2SPTS_MTASK_DELETEING,
};

// 输出信息类型
enum P2SP_TRACE_TYPE
{
    P2SPTT_HTTP_RESOLVING,
    P2SPTT_HTTP_CONNECTION,
    P2SPTT_HTTP_REQUEST,
    P2SPTT_HTTP_READSTATUSLINE,
    P2SPTT_HTTP_READHEADERS,

    P2SPTT_HTTP_CONNECTED,
    P2SPTT_HTTP_STARTREADDATA,
    P2SPTT_HTTP_REQUESTNEXTRANGE,
    P2SPTT_HTTP_REQUESTNONE,

    P2SPTT_HTTP_DROPTHISRANGE,
};

// 所有的接口只允许在IP2SPEngineCallback的回调接口中使用
class IP2SPTaskHTTPClient
{
public:
    virtual size_t Index() = 0;
    virtual const char* FileMD5() = 0;
};
class IP2SPTask
{
public:
    virtual TASKID Id() = 0;
    virtual P2SP_TASK_STATE State() = 0;
    virtual const char* FileMD5() = 0;
    virtual const char* FileName() = 0;
    virtual int64 FileSize() = 0;
    virtual int64 FileCompleteSize() = 0;
    virtual size_t Blocks() = 0;
    virtual void BlocksStatus(/*[in]*/bool* complete_status, size_t len) = 0;
    virtual const char* SavePath() = 0;
};
class IP2SPEngine
{
public:
    virtual size_t TaskCount() = 0;
    virtual size_t DownloadingTaskCount() = 0;
    virtual IP2SPTask* Task(size_t index) = 0;
    virtual IP2SPTask* Task(const char* file_md5) = 0;
    virtual size_t TaskHttpClients(const char* file_md5) = 0;
    virtual int64 TaskTotalHttpDownload(const char* file_md5) = 0;
    virtual int64 TaskTotalP2PDownload(const char* file_md5) = 0;
    virtual int64 TaskTotalHttpSaved(const char* file_md5) = 0;
    virtual int64 TaskTotalP2PSaved(const char* file_md5) = 0;
    virtual size_t TaskDownloadSpeed(const char* file_md5) = 0;
    virtual size_t TaskHttpDownloadSpeed(const char* file_md5) = 0;
    virtual size_t TaskP2PDownloadSpeed(const char* file_md5) = 0;
    virtual size_t TaskUploadSpeed(const char* file_md5) = 0;
    virtual size_t MaxDownloadSpeed() = 0;
    virtual size_t MaxDownloadSpeed(size_t speed) = 0;
    virtual size_t MaxUploadSpeed() = 0;
    virtual size_t MaxUploadSpeed(size_t speed) = 0;
    virtual size_t MaxDownloadingTask() = 0;
    virtual size_t MaxDownloadingTask(size_t max_task) = 0;
    virtual const char* TaskSaveDir() = 0;
    virtual const char* TaskSaveDir(const char* dir) = 0;
};

// 所有的回调函数都在引擎主线程中执行
// 禁止在回调函数中直接调用 "引擎使用入口" 部分的API, 会导致死锁
// 最好不要在回调函数中做计算量大的操作, 这样会堵塞引擎主线程
class IP2SPEngineCallback
{
public:
    virtual void OnResumeTask(IP2SPEngine* engine,
        const char* file_md5, P2SP_ERROR err) = 0;
    virtual void OnAddTask(IP2SPEngine* engine,
        const char* file_md5, P2SP_ERROR err) = 0;
    virtual void OnStartTask(IP2SPEngine* engine,
        const char* file_md5, P2SP_ERROR err) = 0;
    virtual void OnStopTask(IP2SPEngine* engine,
        const char* file_md5, P2SP_ERROR err) = 0;
    virtual void OnDeleteTask(IP2SPEngine* engine,
        const char* file_md5, P2SP_ERROR err) = 0;
    virtual void OnUserAction(IP2SPEngine* engine,
        UINT action, WPARAM wparam, LPARAM lparam) = 0;
    
    virtual void OnHttpDownloadTrace(IP2SPEngine* engine,
        IP2SPTaskHTTPClient* client, P2SP_TRACE_TYPE type,
        const char* message) = 0;
    virtual void OnHttpDownloadError(IP2SPEngine* engine,
        IP2SPTaskHTTPClient* client, P2SP_ERROR err) = 0;
    virtual void OnP2PDownloadTrace(const char* file_md5,
        const char* message) = 0;
    virtual void OnP2PDownloadTrace(const char* message) = 0;

    virtual void OnTaskProgress(IP2SPEngine* engine,
        IP2SPTask* task) = 0;
    virtual void OnTaskBlockComplete(IP2SPEngine* engine,
        IP2SPTask* task, size_t block_index) = 0;
    virtual void OnTaskComplete(IP2SPEngine* engine,
        IP2SPTask* task) = 0;
    virtual void OnBeginTaskSpeed(IP2SPEngine* engine) = 0;
    virtual void OnTaskSpeed(IP2SPEngine* engine,
        IP2SPTask* task) = 0;
    virtual void OnEndTaskSpeed(IP2SPEngine* engine) = 0;

	// 通知任务状态改变
	virtual void OnTaskUpdateState(IP2SPEngine* engine, IP2SPTask* task) = 0;
};

enum P2SPServerArea
{
    P2SPSA_DUPLEX,  // 双线
    P2SPSA_TELECOM, // 电信
    P2SPSA_UNICOM,  // 新联通
};
struct P2SPServerParam
{
    unsigned int    id;
    unsigned long   ip;
    unsigned short  port;
    P2SPServerArea  isp;
};

//////////////////////////////////////////////////////////////////////////
#define MAGIC_GAME_ID   0          // 下载客户端标识, !!!不要分发给游戏!!!
//////////////////////////////////////////////////////////////////////////
struct P2SPStartEngineParam
{
    int game_id;                    // 启动引擎的game标识 MAGIC_GAME_ID为下载客户端
    const char* seed_url_prefix;    // 种子文件url前缀
    ushort listen_port;             // P2P侦听端口
    const char* save_path_utf8;     // 下载保存路径
    size_t max_download_speed;      // 最大下载速率(B) 0:不限速
    size_t max_upload_speed;        // 最大上传速率(B) 0:不限速
    size_t start_p2p_throttler;     // 任务启动p2p下载的阀值
    size_t max_downloading_task;    // 最大同时下载任务数
    size_t task_http_connections;   // 单任务的HTTP连接数

    const char* log_file_url;       // 日志文件上传地址
    const char* log_file_part_name; // 日志文件上传form中的part name
    const char* minidump_file_url;      // 崩溃文件上传地址
    const char* minidump_file_part_name;// 崩溃文件上传form中的part name

    P2SPServerParam* cis_list;
    int cis_list_len;
    P2SPServerParam* tunnel_list;
    int tunnel_list_len;
};

// 引擎使用入口

extern "C" {
	// 启动引擎
	P2SPENGINESDK_PUBLIC bool P2SP_StartEngine(const P2SPStartEngineParam& param,
											   IP2SPEngineCallback* callback);
	// 停止引擎
	P2SPENGINESDK_PUBLIC void P2SP_StopEngine();
	// 恢复上次未完成的任务
	P2SPENGINESDK_PUBLIC bool P2SP_ResumeAllTasks();
	// 添加新任务, 对于正在添加的任务或者已经存在的任务不会重复添加
	// task_save_path_utf8为空表示使用全局设置的保存路径
	P2SPENGINESDK_PUBLIC bool P2SP_AddTask(const char* file_md5,
										   const char* task_save_path_utf8);
	// 开始下载指定任务
	P2SPENGINESDK_PUBLIC bool P2SP_StartTask(const char* file_md5);
	// 停止下载指定任务
	P2SPENGINESDK_PUBLIC bool P2SP_StopTask(const char* file_md5);
	// 删除指定任务(删除下载文件)
	P2SPENGINESDK_PUBLIC bool P2SP_DeleteTask(const char* file_md5);
	// 用户回调动作
	P2SPENGINESDK_PUBLIC bool P2SP_UserAction(UINT action, WPARAM wparam, LPARAM lparam);

	// 错误码信息 (信息为中文宽字符编码 建议真正的工程中不要使用该函数 可采取资源字符形式显示)
	// 仅限开发者调试信息输出使用!!!
	P2SPENGINESDK_PUBLIC const wchar_t* P2SP_GetErrorMessage(P2SP_ERROR err);
};
#endif //__p2sp_engine_sdk_h__