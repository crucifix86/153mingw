#include "EC_HttpGet.h"
#include "EC_Md5hash.h"
#include "EC_Signature.h"
#include "Base64.h"
#include "Updater.h"
#include "Singleton.h"
#include <Direct.h>

#define EXPORTFUNC __declspec(dllexport)

typedef int(* callback_t)(int status, PATCH::Downloader* worker);
typedef BOOL (CALLBACK*FuncType)(void *,LPCTSTR);
BOOL CALLBACK UpdateFunc(FuncType InterFaceFunc,LPCTSTR lName,LPCTSTR lParam,void *vp);

FILE* OpenFile(LPCTSTR name, LPCTSTR param);
void UDeleteFile(LPCTSTR src);
void ThreadLock();
void ThreadUnLock();
bool CheckLocal();
bool CheckPackVersion(bool bFirstGet);
void SetUpdateState(int state);
int UpdateFromPack(const char* packname,int numpacks,int packnum);
BOOL CalFileMd5(LPCTSTR lName,char *md5);
int UpdateCallback(int status,PATCH::Downloader* worker);
void Command(void *pList,LPCTSTR lCommand);
void DealWithUpdateError(int iError);
void PackListError();
void MD5ListError();
void PackDownloadError();
void WriteElementLog(LPCTSTR c,BOOL reWrite=FALSE);
bool CheckPack();
void DownloadFileByHttp(CString src, CString des, callback_t callback, unsigned int start, int iTryTimes = 5);

#ifdef RUSSIA_PATCH
void RunJadeLoader();
#endif

typedef enum {
	STATE_OK,
	NEED_INSTALL,
	NEED_DOWNLOAD,
}ARC_STATE_NORTHAMERICA;
extern ARC_STATE_NORTHAMERICA Arc_State_NorthAmerica;

extern CString fnLocalUpdatePack;
extern bool bUpdated;
extern bool bAutoClose;
extern bool bSepFileUpdate;
extern bool bCleanPack;
extern CString strTable[200];
extern CString strParamToClient;
extern CString UpdateServerName;
extern BOOL bSilentUpdate;

class SepFileUpdater : public Updater, public Singleton<SepFileUpdater>
{
public:
	SepFileUpdater(){}
	virtual ~SepFileUpdater(){}
	
	virtual bool Update();
	virtual int GetVersion(bool bFirstGet);
};
