
#ifndef __http_request_h__
#define __http_request_h__

#include <Windows.h>
#include <ObjIdl.h>

#define WM_USER_DISPLAYSTATUS   (WM_USER + 1982) // 发送给通知窗口的消息
// wparam:0 lparam:DOWNLOADPROGRESS*(NULL)
struct DOWNLOADPROGRESS
{
    ULONG progress;
    ULONG progress_max;
};
// 下载Url链接到临时路径
// [in]  notify_wnd: 通知窗口, 传递进度信息(DOWNLOADPROGRESS), 可以为空
// [in]  stop_flag: 停止下载标志, 可以为空
// [in]  url: 指定下载文件的Url路径
// [out] file_path: 下载文件的存放路径
bool HttpDownloadToFile(HWND notify_wnd, bool* stop_flag, const char* url,
                        const wchar_t* file_path);

bool HttpDownloadToStream(HWND notify_wnd, bool* stop_flag, const char* url,
                          IStream** stream);

bool HttpUploadFile(const char* url, const wchar_t* file_path,
                    const char* file_part_name, int* timeout);

#endif //__http_request_h__