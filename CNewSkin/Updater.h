#ifndef NEWSKIN_UPDATER_H_
#define NEWSKIN_UPDATER_H_

#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
	TypeName(const TypeName&);\
	void operator=(const TypeName&)

class Updater
{
public:
	Updater(){}
	virtual ~Updater(){}

	virtual bool Update() = 0;
	virtual int GetVersion(bool bFirstGet) = 0;

	static void SetDownloadParam(int iCurrent, int iTotal, int iUpdateInterval);
	static void ShowDownloadSpeed(UINT64 ComplateSize);
	
	enum VersionExplain{
		FAILED_DOWNLOADING_VERSION_FILE = -1,
		VERSION_EQUAL_WITH_SERVER = 0,
		SERVER_MAINTENANCE = 1,
		LOCAL_VERSION_ERROR = 2,
		NEED_UPDATE = 3,
		SERVER_VERSION_LOW = 4,
		VERSION_STOP_UPDATE = 5,
		PACKVERSION_GETTING_FAILED = 6,
	};

	enum PackUpdateError{
		PACK_OPEN_FALIED = 1,
		PACK_UPDATELIST_ERROR = 2,
		PACK_UPDATELIST_VERIFY_FAILED = 3,
		PACK_VERSION_TOO_HIGH = 4,
		PACK_DISK_NOT_ENOUGH_SPACE = 5,
		PACK_FILE_CORRUPTED = 6,
		PACK_VERSION_ERROR = 7,
		PACK_CLIENT_NOT_FIT = 8,
	};

	enum UpdateState{
		STATE_SEP_UPDATE,// 散文件更新和用户手动使用离线更新
		STATE_PACK_UPDATE,// 自动下载更新包更新
	};

	enum PackStatus{
		PACK_UPDATE_FAILED = 0,
		PACK_NOT_EXISTS = 0,
		PACK_ALREADY_EXISTS = 1,
	};

private:
	DISALLOW_COPY_AND_ASSIGN(Updater);
	static int			m_iCurrentDownload;     // 当前下载的文件
	static int			m_iTotalDownload;	   // 一共需要下载的文件
	static int			m_iUpdateInterval;	   // 更新间隔
	static int			m_iLastTick;	
	static UINT64		m_u64LastFileSize;
};




#endif //NEWSKIN_UPDATER_H_