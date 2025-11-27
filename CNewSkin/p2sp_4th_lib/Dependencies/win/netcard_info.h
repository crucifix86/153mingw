
#ifndef __netcard_info_h__
#define __netcard_info_h__

#include <string>
#include <Windows.h>
#include <IPHlpApi.h>

// 网卡信息,包括有线网卡、无线网卡、虚拟网卡
class CNetCardInfo
{
public:
    CNetCardInfo();
    virtual ~CNetCardInfo();

    inline DWORD GetError() { return error_; }
    inline const IP_ADAPTER_INFO* GetAdapterInfo() { return adapter_info_; }

    static __int64 ToInt64(const IP_ADAPTER_INFO* adapter_info);

private:
    CNetCardInfo(const CNetCardInfo&);
    void operator=(const CNetCardInfo&);

    DWORD GetInfo();

    IP_ADAPTER_INFO* adapter_info_;
    DWORD error_;
};

#endif //__netcard_info_h__