/**
 * @file      cc_sdk.h
   @brief     The header file for SDK APIs.
   @author    shenhui@pwrd.com
   @data      2013/03/20
   @version   02.00.00
 */

/* 
 *  Copyright(C) 2013. Perfect World Entertainment Inc
 *
 *  This PROPRIETARY SOURCE CODE is the valuable property of PWE and 
 *  its licensors and is protected by copyright and other intellectual 
 *  property laws and treaties.  We and our licensors own all rights, 
 *  title and interest in and to the SOURCE CODE, including all copyright 
 *  and other intellectual property rights in the SOURCE CODE.
 */

/*
 * $LastChangedBy: shenhui@pwrd.com
 * $LastChangedDate: 
 * $Revision: 02.00.00
 * $Id:
 * $notes:.
 */

#pragma once

// Include C++ standard library headers BEFORE defining SAL macros
// to avoid conflicts with parameter names in std:: templates
#ifdef __cplusplus
#include <tuple>
#endif

#include <windows.h>
#include <tchar.h>
#include "ArcAsia_Datatype.h"
#include "ArcAsia_Error.h"
#define ARC_ASIA_SDK_STATICLIB

namespace ARC_ASIA_SDK
{

#ifdef _cplusplus
extern "C"
{
#endif

#ifndef ARC_ASIA_SDK_STATICLIB
#ifdef CC_SDK_EXPORTS
#define CC_SDK_API extern "C" __declspec(dllexport)
#else
#define CC_SDK_API extern "C" __declspec(dllimport)
#endif
#else
#define CC_SDK_API
#endif

// SAL annotation macros - define as empty for non-MSVC compilers
#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

/****************************************************************************/
/*                       Interface declaration                              */
/****************************************************************************/

/**
* @brief: launch application from game
* @param[in] pszGameAbbr: the abbreviation of game name
* @param[in] nLanguage: language info of game client.
*
* @return: If succeed, return S_OK, otherwise return error code.
* @notes: abbr is unique for each game and provided by PWE.
*/
CC_SDK_API HRESULT CC_LaunchClient(wchar_t* pszGameAbbr, LANGUAGE_VERSION nLanguage) ;

/**
* @brief: initialize SDK.
* @param[in] nPartnerId: useless now, reserve for future.
* @param[in] nAppId: game unique id, allocated by PWE.
*
* @return: If succeed, return a handle of SDK, otherwise return NULL 
* @notes: none.
*/
CC_SDK_API CC_HANDLE CC_Init(unsigned int nPartnerId, unsigned int nAppId) ;

/**
* @brief: exit SDK.
* @param[in] hSDK: handle returned by CC_Init
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_UnInit(const CC_HANDLE hSDK) ;

/**
* @brief: get a token when user are trying to log into game.
* @param[in] hSDK: handle returned by CC_Init.
* @param[in] pszAccount: this is account name used to log into game, this parameter is null-terminated.
* @param[out]pszToken: this is the token return back to caller.
*            Token is a 16 characters null-terminated string at present, so caller need allocate at least 17Bytes buffer.
* @param[in] nTokenLen: length of pszToken in byte, should be larger than 32 bytes.
* @param[in] dwTimeout:  timeout in milliseconds.
* @return: If succeed, return S_OK, otherwise return error code
* @notes: This API should be called in the real game client executable, not the launcher or patcher.
*/
//CC_SDK_API HRESULT CC_GetToken(const CC_HANDLE hSDK, wchar_t* pszAccount, wchar_t* pszToken, unsigned int nTokenLen, unsigned long dwTimeout = 60*1000) ;


/**
* @brief: ��Ϸ��ͼ�Զ���¼ʱ���Arc��ȡ��¼��Ϣ�����ٴλ�ȡtoken.
* @param[in] hSDK: handle returned by CC_Init
* @param[out] pszAccount: Arc���ص���ʵ���û��ʺŻ��û���
* @param[in] nAccountLen: pszAccount��ָ��Ļ���������ַ���, ��СΪ128
* @param[out] pszToken: Arc���ص�һ��һ�ܵ�token
* @param[in] nTokenLen: pszToken��ָ��Ļ���������ַ��� ��СΪ64
* @param[out] pszAgent: Arc���ص�Agent����
* @param[in] nAgentLen: pszAgent��ָ��Ļ���������ַ��� ��СΪ16
* @param[in] dwTimeout:  timeout in milliseconds.
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/

CC_SDK_API HRESULT CC_GetTokenEx(__in const CC_HANDLE hSDK, __in wchar_t* pszAccount, __in unsigned int nAccountLen,\
							     __out wchar_t* pszToken, __in unsigned int nTokenLen, \
							     __out wchar_t* pszAgent, __in unsigned int nAgentLen,
							     __in unsigned long dwTimeout = 60*1000);


/**
* @brief: ��ȡArc������Ϸʱָ���ķ�������Ϣ.
* @param[in] hSDK: handle returned by CC_Init
* @param[out] pszServer: ������Ϸʱָ������Ϸ��������Ϊ��ʱArcδָ���ض��ķ�����
* @param[in] nServerLen: pszServer��ָ��Ļ���������ַ���, ��СΪ64
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_GetServerInfo(__in const CC_HANDLE hSDK, __out wchar_t* pszServer, __in unsigned int nServerLen);

/**
* @brief: create a browser tab in game browser.
* @param[in] hSDK: handle returned by CC_Init
* @param[in] pszUrl: url to browser in game browser.
*
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_GotoUrlInOverlay(__in const CC_HANDLE hSDK, __in wchar_t* pszUrl) ;


/**
* @brief: Arc�����Σ���Ϸ�ڳ�ʼ��Sdk�ɹ�֮����ĳ��ԭ��ʹ����Ϸ��¼���˺���Arc���ݵ��˺Ų�һ�£���¼���ɹ����û������л��˺ţ������ص�¼��), ��Ϸ֪ͨArc�˺����л���
* @param[in] hSDK: handle returned by CC_Init
* @param[in] pszNewAccount: ��Ϸ�µ�¼���˺�
* @param[in] pszTicket: ���˺Ź�����ticket
* @return: If succeed, return S_OK, otherwise return error code
* @notes: if return FALSE, error code can be found in CC_Error.h.
*/
CC_SDK_API HRESULT CC_UserChanged(__in const CC_HANDLE hSDK, __in wchar_t* pszNewAccount, __in wchar_t* pszTicket);

#ifdef _cplusplus
}
#endif

}//end of namespace