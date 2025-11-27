
#include "netcard_info.h"

#include "string_printf.h"

#include <atlbase.h>

#pragma comment(lib, "Iphlpapi")

CNetCardInfo::CNetCardInfo()
{
    adapter_info_ = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    error_ = GetInfo();
}

CNetCardInfo::~CNetCardInfo()
{
    if(adapter_info_)
    {
        free(adapter_info_);
        adapter_info_ = NULL;
    }
    error_ = 0;
}

DWORD CNetCardInfo::GetInfo()
{
    ULONG out_buf_len = sizeof(IP_ADAPTER_INFO);
    DWORD error = ::GetAdaptersInfo(adapter_info_, &out_buf_len);

    if(error == ERROR_BUFFER_OVERFLOW)
    {
        free(adapter_info_);
        adapter_info_ = (IP_ADAPTER_INFO*)malloc(out_buf_len);
        error = ::GetAdaptersInfo(adapter_info_, &out_buf_len);
    }

    return error;
}

__int64 CNetCardInfo::ToInt64(const IP_ADAPTER_INFO* adapter_info)
{
    ATLASSERT(adapter_info);
    __int64 int_64 = adapter_info->Address[0];
    int_64 = int_64 << 8;
    int_64 += adapter_info->Address[1];
    int_64 = int_64 << 8;
    int_64 += adapter_info->Address[2];
    int_64 = int_64 << 8;
    int_64 += adapter_info->Address[3];
    int_64 = int_64 << 8;
    int_64 += adapter_info->Address[4];
    int_64 = int_64 << 8;
    int_64 += adapter_info->Address[5];

    return int_64;
}