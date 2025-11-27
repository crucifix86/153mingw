/*
 * FILE: AVariant.h
 *
 * DESCRIPTION: ʵ������������AVariant
 *
 * CREATED BY: ����ӯ, 2002/8/7
 *
 * HISTORY: By JiangLi��ʹ��ģ�幹�캯���Լ�ģ������ת��������Ӧ���ǲ����еġ�
 *               ������¶�������ز��֡�
 *
 * MODIFY: By ������ȥ����
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */


#ifndef _AVARIANT_H__
#define _AVARIANT_H__

#include <AString.h>
#include <A3DTypes.h>
#include <A3DMacros.h>
#include <A3DFontMan.h>
#include "AFile.h"

#define DEFINEOPERATOR(type) 	operator type() {return *(type *)(&m_avData);}

class ASet;
class AObject;
class AClassInfo;
class AScriptCodeData;
class AVariant  
{
public:

	enum AVARTYPE
	{
		AVT_INVALIDTYPE,	// �Ƿ����ͣ�����Ĭ�Ϲ���,��ʱ�κ����ݳ�Ա��������
		AVT_BOOL,			// ������				m_avData.bV;
		AVT_CHAR,			// �ַ�					m_avData.cV;	
		AVT_UCHAR,			// �޷����ַ�			m_avData.ucV;	
		AVT_SHORT,			// ������				m_avData.sV;	
		AVT_USHORT,			// �޷��Ŷ�����			m_avData.usV;	
		AVT_INT,			// ����					m_avData.iV;	
		AVT_UINT,			// �޷�������			m_avData.uiV;	
		AVT_LONG,			// ������				m_avData.lV;	
		AVT_ULONG,			// �޷��ų�����			m_avData.ulV;	
		AVT_INT64,			// 64Bit����			m_avData.i64V;	
		AVT_UINT64,			// �޷���64Bit����		m_avData.ui64V;
		AVT_FLOAT,			// ������				m_avData.fV;	
		AVT_DOUBLE,			// ˫���ȸ�����			m_avData.dV;	
		AVT_POBJECT,		// ����ָ��				m_avData.paV;
		AVT_PSCRIPT,		//�ű��������				m_avData.pascdV
		AVT_A3DVECTOR3,		// ����					m_avData.a3dvV;
		AVT_A3DMATRIX4,		// ����					m_avData.pa3dmV;
		AVT_STRING,			// �ַ���				m_avData.pstrV;
		AVT_PABINARY,		// �����ƿ�				m_avData.binV;
		AVT_PACLASSINFO,	// �മ�л���Ϣָ��		m_avData.paciV;
		AVT_PASET,			// ����ָ��				m_avData.pasetV;
		AVT_FONT,			// ����					m_avData.font;
	};


public:

	AVariant();
	AVariant(const AVariant & v);
	AVariant(bool bV);
	AVariant(char cV);
	AVariant(unsigned char ucV);
	AVariant(short sV);
	AVariant(unsigned short usV);
	AVariant(int iV);
	AVariant(unsigned int uiV);
	AVariant(long lV);
	AVariant(unsigned long ulV);
	AVariant(__int64 i64V);
	AVariant(unsigned __int64 ui64V);
	AVariant(float fV);
	AVariant(double dV);
	AVariant(AObject & object);
	AVariant(AScriptCodeData & object);
	AVariant(A3DVECTOR3 & avec3);
	AVariant(A3DMATRIX4 &amat4);
	AVariant(AString & str);
//	AVariant(const ABinary & binV);
	AVariant(AClassInfo * paciV);
//	AVariant(ASet * pSet);
	AVariant(A3DFontMan::FONTTYPE font);

	int GetType() const {return m_iType;}

	DEFINEOPERATOR(bool)
	DEFINEOPERATOR(char)
	DEFINEOPERATOR(unsigned char)
	DEFINEOPERATOR(short)
	DEFINEOPERATOR(unsigned short)
	DEFINEOPERATOR(int)
	DEFINEOPERATOR(unsigned int)
	DEFINEOPERATOR(long)
	DEFINEOPERATOR(unsigned long)
	DEFINEOPERATOR(__int64)
	DEFINEOPERATOR(unsigned __int64)

	// ��float��ת�����������
	operator float()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return (float)m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return m_avData.fV;
		return (float)(*(__int64 *)(&m_avData));
	}

	// ��double��ת�����������
	operator double()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		if(m_iType == AVT_DOUBLE)
			return m_avData.dV;
		if(m_iType == AVT_FLOAT)
			return (double)m_avData.fV;
		return (double)(*(__int64 *)(&m_avData));
	}

	operator AString()
	{
		return m_string;
	}

	AVariant& operator= (const AVariant& src)
	{
		m_string = src.m_string;
		memcpy(&m_avData, &src.m_avData, sizeof(src.m_avData));
		m_iType = src.m_iType;
		return *this;
	}

	DEFINEOPERATOR(AObject *)
	DEFINEOPERATOR(A3DVECTOR3)
	DEFINEOPERATOR(A3DMATRIX4)
	DEFINEOPERATOR(AClassInfo *)
	DEFINEOPERATOR(AScriptCodeData*)
	DEFINEOPERATOR(A3DFontMan::FONTTYPE)
	operator AScriptCodeData&()
	{
		assert(m_iType != AVT_INVALIDTYPE);
		return *(AScriptCodeData*)(&m_avData);
	}
//	DEFINEOPERATOR(ASet *)
	
	void Save(AFile* pFile);
	void Load(AFile* pFile);

private:

	AString m_string;
	union AVARIANT
	{
		bool				bV;			// ������
		char				cV;			// �ַ�
		unsigned char			ucV;		// �޷����ַ�
		short				sV;			// ������
		unsigned short			usV;		// �޷��Ŷ�����
		int				iV;			// ����
		unsigned int			uiV;		// �޷�������
		long				lV;			// ������
		unsigned long			ulV;		// �޷��ų�����
		__int64				i64V;		// 64Bit����
		unsigned __int64		ui64V;		// �޷���64Bit����
		float				fV;			// ������
		double				dV;			// ˫���ȸ�����
		AObject *			paV;		// ����ָ��
		char				a3dvV[sizeof(A3DVECTOR3)];	// ����������
		char				a3dmV[sizeof(A3DMATRIX4)];	// ����������
//		AString *			pstrV;		// �ַ��� 
//		const ABinary *			pbinV;		// �����ƿ����ָ��
		AClassInfo *			paciV;		// ���л���Ϣָ��
		ASet *				pasetV;		// ����ָ��
		AScriptCodeData*		pascdV;		//�ű���ָ��
		A3DFontMan::FONTTYPE font;
	} m_avData;
	int m_iType;	// ��������
};

#define		CODE_PAGE			CP_ACP

class CSafeString
{
	LPSTR m_szBuf;
	LPWSTR m_wszBuf;

public:
	CSafeString(LPCSTR lpsz, int nLen) : m_wszBuf(NULL)
	{
		assert(lpsz);
		if (nLen < 0) nLen = strlen(lpsz);
		m_szBuf = new char[nLen+1];
		strncpy(m_szBuf, lpsz, nLen);
		m_szBuf[nLen] = '\0';
	}

	CSafeString(LPCSTR lpsz) : m_wszBuf(NULL)
	{
		assert(lpsz);
		int n = strlen(lpsz);
		m_szBuf = new char[n+1];
		strcpy(m_szBuf, lpsz);
	}
	
	CSafeString(LPCWSTR lpwsz, int nLen) : m_szBuf(NULL)
	{
		assert(lpwsz);
		if (nLen < 0) nLen = wcslen(lpwsz);
		m_wszBuf = new wchar_t[nLen+1];
		wcsncpy(m_wszBuf, lpwsz, nLen);
		m_wszBuf[nLen] = L'\0';
	}

	CSafeString(LPCWSTR lpwsz) : m_szBuf(NULL)
	{
		assert(lpwsz);
		int n = wcslen(lpwsz);
		m_wszBuf = new wchar_t[n+1];
		wcscpy(m_wszBuf, lpwsz);
	}

	operator LPCSTR() { return GetAnsi();}
	operator LPCWSTR() { return GetUnicode(); }
	
	CSafeString& operator= (const CSafeString& str)
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;

		m_szBuf = NULL;
		m_wszBuf = NULL;

		if (str.m_szBuf)
		{
			m_szBuf = new char[strlen(str.m_szBuf)+1];
			strcpy(m_szBuf, str.m_szBuf);
		}

		if (str.m_wszBuf)
		{
			m_wszBuf = new wchar_t[wcslen(str.m_wszBuf)+1];
			wcscpy(m_wszBuf, str.m_wszBuf);
		}
		return *this;
	}

	LPCSTR GetAnsi()
	{
		if (m_szBuf) return m_szBuf;
		assert(m_wszBuf);
		
		int nCount = WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			NULL,
			0,
			NULL,
			NULL);
	
		m_szBuf = new char[nCount];

		WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			m_szBuf,
			nCount,
			NULL,
			NULL);
		return m_szBuf;
	}
	
	LPCWSTR GetUnicode()
	{
		if (m_wszBuf) return m_wszBuf;
		assert(m_szBuf);
		
		int nCount = MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			NULL,
			0);

		m_wszBuf = new wchar_t[nCount];

		MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			m_wszBuf,
			nCount);

		return m_wszBuf;
	}

	virtual ~CSafeString() 
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;
	}
};

#endif
