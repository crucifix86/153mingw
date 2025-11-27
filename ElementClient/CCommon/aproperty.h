    /*
 * FILE: AProperty.h
 *
 * DESCRIPTION: 
 *	ʵ�����Զ����������ͽӿڶ���
 *		ARange;		��ʾ��һ����ֵ��Χ�Ľӿ�
 *		ASet;		��ʾ��һ����ֵ�����Ľӿ�
 *		AProperty;	��ʾ�����ԵĽӿ�
 *		ARangeTemplate;	��ֵ��Χ��ģ��ʵ��
 *		ASetTemplate;	��ֵ������ģ��ʵ��
 *		ASetElement;	��ֵ�������Ԫ��
 *		APropertyTemplate;	���Ե�ģ��ʵ��
 *
 * CREATED BY: ����ӯ, 2002/8/7
 *
 * MODIFY BY�����������˺��ٵ��޸ģ����������ط���һЩ��Ų�����������������ע�� 
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Epie-Game, All Rights Reserved.
 */

#pragma once

#pragma warning ( disable : 4786 )
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <AMemory.h>
#include <vector.h>
#include "AVariant.h"


// Ϊʵ�ִ���򻯶������
#define EXTERN_DEFINESET(type, name) extern ASetTemplate<type> name;
#define	DEFINE_SETBEGIN(type, name) ASetTemplate<type> name(
#define DEFINE_SETELEMENT(type, name, value) ASetTemplate<type>::ASetElement<type>(name, value),
#define DEFINE_SETEND(type) ASetTemplate<type>::ASetElement<type>());
#define DEFINE_SETFILE(type,name,filename) ASetTemplate<type> name(filename);

// �༭���Դ����Ե��Ƽ���ʽ
enum 
{
	WAY_DEFAULT,		// ȱʡ��ʽ���ɱ༭���Զ����AVariant�����Ͳ�����
	WAY_BOOLEAN,		// ������������ʽ
	WAY_INTEGER,		// ���ʹ�����ʽ
	WAY_FLOAT,		// ���㴦����ʽ
	WAY_STRING,		// �ִ�������ʽ
	WAY_FILENAME,		// �ļ���
	WAY_COLOR,		// ��ɫֵ
	WAY_VECTOR,		// ����ֵ
	WAY_BINARY,		// �����ƿ�
	WAY_OBJECT,		// ����
	WAY_IDOBJECT,
	WAY_NAME,		// ���������:)����Ϊʹ��SetName���õ������д����ƣ�������ͨ�õķ�����ֵ
	WAY_SCRIPT,		// �ű�����
	WAY_UNITID,		// ����ID
	WAY_PATHID,		// ·��ID
	WAY_WAYPOINTID,		// ·��ID
	WAY_STRID,		// (�ĵ���)�ַ�����ԴID
	WAY_SFXID,		// (�ĵ���)��Ч��ԴID
	WAY_TRIGGEROBJECT,	// ������
	WAY_MEMO,		// ��ע
	WAY_FONT,		// ����
	WAY_CUSTOM,		// �û��Զ��巽ʽ
	// ������ʽ˵��
	WAY_READONLY	= (1 << 31),// ����ֻ������
};

/////ARange�ӿ�
class ARange
{
public:

	virtual AVariant GetMaxValue() = 0;
	virtual AVariant GetMinValue() = 0;
	virtual bool IsInRange(AVariant value) = 0;
};

/////ASet�ӿ�
class ASet
{
	friend int InitPropertiesList();
	friend int ReadSetData(const char * filename, ASet *pSet);

	virtual int AddData(const char * name,const char * data){return -1;}
	virtual int ReadData() { return -1;};
public:

	virtual int GetCount() = 0;
	virtual AString GetNameByIndex(int index) = 0;
	virtual AVariant GetValueByIndex(int index) = 0;
	virtual AVariant GetValueByName(AString szName) = 0;
	virtual AString GetNameByValue(AVariant value) = 0;
	virtual int FindValue(AVariant value) = 0;
	virtual int FindName(AString szName) = 0;

};

int AddInitElement(ASet * pSet);
int InitPropertiesList();
int ReadSetData(const char * filename, ASet *pSet);

/////AProperty�ӿ�
class AProperty
{
public:
	enum
	{
		HAS_DEFAULT_VALUE	= 1,
		DEFAULT_VALUE		= 2,
		DO_NOT_SAVE		= 4,

//��������ʱ�õ�����
		USE_DEFINE_DEFAULT	= 3
		
	};

	virtual bool IsUserVisable() = 0;
	virtual const char * GetName() = 0;
	virtual const char * GetPath() = 0;
	virtual ASet * GetPropertySet() = 0;
	virtual ARange * GetPropertyRange() = 0;
	virtual int GetWay() = 0;
	virtual void SetWay(int way) = 0;
	virtual int GetState() = 0;
	virtual void SetState(int state) = 0;
	virtual AVariant GetValue(void * pBase) = 0;
	virtual void SetValue(void * pBase, AVariant value) = 0;
	virtual void SetStringValue(void *pBase,const char * str) = 0;
	virtual void GetStringValue(void *pBase,AString &str) = 0;
	virtual ~AProperty() {};
};

/////ARangeTemplate��
template <class TYPE> class ARangeTemplate : public ARange
{
public:

	ARangeTemplate(TYPE min, TYPE max) :m_min(min), m_max(max) {}
	AVariant GetMaxValue() {return AVariant(m_max);}
	AVariant GetMinValue() {return AVariant(m_min);}

	bool IsInRange(AVariant value)
	{
		if(TYPE(value) < m_min && TYPE(value) > m_max)
		{
			return false;
		}

		return true;
	}

private:
	
	TYPE m_min;
	TYPE m_max;
};

/////ASetTemplate��
template <class TYPE> class ASetTemplate : public ASet
{

	virtual int ReadData()
	{
		if(!m_FileName.IsEmpty())
		{
			return ReadSetData(m_FileName,this);
		}
		else
		{
			assert(false);
			return -1;
		}
	}

	virtual int AddData(const char * name,const char * value)
	{
		TYPE data;
		StringToValue(data,value);
		ASetElement element(name,data);
		m_List.push_back(element);
		return 0;
	}
public:

	// Inner element class - uses outer template parameter TYPE
	class ASetElement
	{
	public:

		ASetElement()
		{
			m_szName.Empty();
		}

		ASetElement(AString szName, TYPE data)
		{
			m_szName	= szName;
			m_data		= data;
		}

		ASetElement(const ASetElement & e)
		{
			m_szName	= e.m_szName;
			m_data		= e.m_data;
		}

		AString GetName() {return m_szName;}
		TYPE GetData() {return m_data;}

	private:

		AString m_szName;
		TYPE		 m_data;
	};

protected:

	abase::vector<ASetElement > m_List;
	AString	m_FileName;

public:

	// ���캯��
	ASetTemplate(const char * filename):m_FileName(filename)
	{
		AddInitElement(this);		
	}

	// ���캯��
	ASetTemplate(ASetElement first, ...)
	{
		va_list vlist;
		ASetElement element = first;
		va_start(vlist, first);
		while(!element.GetName().IsEmpty())
		{
			m_List.push_back(element);
			element = va_arg(vlist, ASetElement);
		}
		va_end(vlist);
	}

	ASetTemplate() {}

	~ASetTemplate()
	{
	}

	void AddElement(AString szName, TYPE val)
	{
		m_List.push_back(ASetElement(szName, val));
	}

	void AddElement(ASetElement& ele)
	{
		m_List.push_back(ele);
	}

	//Ԫ�ظ���
	int GetCount()
	{
		return m_List.size();
	}

	// ����������ȡ����
	AString GetNameByIndex(int index)
	{
		return m_List[index].GetName();
	}

	// ����������ȡֵ
	AVariant GetValueByIndex(int index)
	{
		return AVariant(m_List[index].GetData());
	}

	// �������Ʋ�ֵ
	AVariant GetValueByName(AString szName)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(strcmp(szName, m_List[index].GetName()) == 0)
			{
				return AVariant(m_List[index].GetData());
			}
		}

		return AVariant();	//δ�ҵ�Ԫ�ص�����£�����һ��������������ΪAVT_INVALIDTYPE��
	}

	// ����ֵ������
	AString GetNameByValue(AVariant value)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetData() == (TYPE)value)
			{
				return m_List[index].GetName();
			}
		}

		return AString();
	}

	// ��ֵ����
	int FindValue(AVariant value)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetData() == (TYPE)value)
			{
				return index;
			}
		}

		return -1;
	}

	// ����������
	int FindName(AString szName)
	{
		for(int index = 0;index < m_List.size();index ++)
		{
			if(m_List[index].GetName() == szName)
			{
				return index;
			}
		}

		return -1;
	}
};

void StringToValue(AVariant &var, const char * str);
void StringToValue(int &value, const char * str);
void StringToValue(float &value, const char * str);
void StringToValue(bool &value, const char * str);
void StringToValue(AString &value, const char * str);
void StringToValue(A3DVECTOR3 &value,const char * str); 
void StringToValue(A3DCOLOR &value,const char * str);
void StringToValue(AScriptCodeData &value,const char * str);

void ValueToString(const AVariant &var, AString & str);
void ValueToString(int value, AString & str);
void ValueToString(float value, AString & str);
void ValueToString(bool value, AString & str);
void ValueToString(const AString &value, AString & str);
void ValueToString(const A3DVECTOR3 &value, AString & str); 
void ValueToString(const A3DCOLOR &value, AString & str); 
void ValueToString(const AScriptCodeData &value, AString & str); 

template <class TYPE> class APropertyTemplate : public AProperty  
{
public:
	
	APropertyTemplate(
		const char * clsName,
		const char * szName,			// ������
		int iOffset,				// ������ƫ����
		ASet * pSet = NULL,			// ����
		ARange * pRange = NULL,			// ��Χ
		int way = WAY_DEFAULT,			// ���ԵĴ�����ʽ
		const char * szPath = NULL,		// ·�����ļ����ӵ���Ѱλ�ã�ΪNULLʱ������ִ�г���ʼ��·�����£�
		bool isVisable = true,			//�Ƿ�Ϊ�û��ɼ�
		int  iState = 0)			//���Ե�״̬(����Ĭ��ֵ)
	{
		assert(szName);
		m_szName	= szName;
		m_iOffset	= iOffset;
		m_pRange	= pRange;
		m_pSet		= pSet;
		m_iWay		= way;
		m_bUserVisable	= isVisable;
		m_State 	= iState;
		if(szPath) m_szPath = szPath;
		/*
		char buf[30];
		AString str;
		str += clsName;
		str += "\t����:";
		str += szName;
		str += "\tƫ��:";
		str += itoa(iOffset,buf,10);
		str += "\n";		
		OutputDebugString(str);
		*/
	}



	bool IsUserVisable() { return m_bUserVisable;}
	int GetState()
	{
		return m_State;
	}
	void SetState(int state)
	{
		m_State = state;
	}

	AVariant GetValue(void * pBase)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		return AVariant(*(TYPE*)(pData + m_iOffset));
	}

	void SetValue(void * pBase, AVariant value)
	{
		assert(pBase);
		char * pData = (char *)pBase;
		*(TYPE*)(pData + m_iOffset) = (TYPE)value;
	}

	void SetStringValue(void *pBase,const char * str)
	{
		::StringToValue(*(TYPE*)((char *)pBase + m_iOffset),str);
	}

	void GetStringValue(void *pBase,AString &str)
	{
		::ValueToString(*(TYPE*)((char *)pBase + m_iOffset),str);
	}

	const char * GetName()
	{
		return m_szName;
	}

	const char * GetPath()
	{
		return m_szPath;
	}

	ASet * GetPropertySet()
	{
		return m_pSet;
	}

	ARange * GetPropertyRange()
	{
		return m_pRange;
	}

	int GetWay()
	{
		return m_iWay;
	}

	void SetWay(int way)
	{
		m_iWay = way;
	}

private:

	AString			m_szName;		// ������
	AString			m_szPath;		// ·��(������Ϊ�ļ�����ʱ�������ļ�����Ѱ λ��)
	int			m_iOffset;		// ����ƫ����
	int			m_iWay;			// ���ԵĴ�����ʽ
	ARange *		m_pRange;		// ����ֵ�ķ�Χ
	ASet *			m_pSet;			// ����ֵ�ļ���
	bool			m_bUserVisable;
	int			m_State;
};


//��������Ҫ�ĺ�

// ��ʼ����̬���Լ��ϣ��������ڹ��캯���г��֣�
#define INITIALIZE_STATICPROPERIES(classname) 

// ���徲̬���Լ�����ʼ�㣬������������������
#define STATICPROPERIES_BEGIN(classname,base) \
	protected:	\
	virtual int PropertiesCount() \
	{ \
		int count = 0;\
		classname::Properties(&count, NULL, count);\
		return base::PropertiesCount() + count;\
	}\
	virtual AProperty * Properties(int index)\
	{\
		AProperty * pProp = base::Properties(index);\
		if(pProp) return pProp;\
		index -= base::PropertiesCount();\
		return classname::Properties(NULL, NULL, index);\
	}\
	virtual AProperty * Properties(const char *  name,int *index = NULL)\
	{\
		AProperty * pProp = base::Properties(name);\
		if(pProp) return pProp;\
		int idx;\
		return classname::Properties(NULL, name, index?*index:idx);\
	}\
	struct _staticproperies\
	{\
		AProperty * properties(int * pCount, const char * pName, int &index)\
		{\
			static AProperty * pProps[] = \
			{

// ����������Ŀ��������  ������  ���������������ͣ�����ָ�룬����ֵ���ϣ�����ֵ��Χ�����ԵĴ�����ʽ���ļ�������Ѱλ�ã��������ֵΪ�ļ����ӵĻ������Ƿ�Ϊ�û��ɼ�
#define STATICPROPERTY(classname, name, prop_type, prop_data, prop_set, prop_range, prop_way, prop_path,prop_visable,prop_state) \
			new APropertyTemplate<prop_type>(#classname,name, offsetof(classname, prop_data), prop_set, prop_range, prop_way, prop_path,prop_visable,prop_state),

// ���徲̬�����ս�㣬����������
#define STATICPROPERIES_END(classname) \
			};\
			static int count = sizeof(pProps) / sizeof(AProperty *);\
			if(pCount) {*pCount = count;return NULL;}\
			if(pName)\
			{\
				for(int a = 0;a < count;a ++)\
					if(strcmp(pProps[a]->GetName(), pName) == 0) {\
						index = a;\
						return pProps[a];\
					}\
				return NULL;\
			}\
			if(index >= 0 && index < count) return pProps[index];\
			return NULL;\
		}\
		~_staticproperies()\
		{\
			int count = 0;\
			properties(&count, NULL, count);\
			for(int a = 0;a < count;a ++)\
			{\
				AProperty * pProp = properties(NULL, NULL, a);\
				delete pProp;\
			}\
		}\
	};\
	friend struct _staticproperies;\
	static AProperty * Properties(int * pCount, const char * pName, int &index)\
	{\
		static classname::_staticproperies properies;\
		return properies.properties(pCount, pName, index);\
	}



/*
  ���͵����Զ��������֣�

  ������
  STATICPROPERTY("property", int, m_int_value, NULL, NULL, WAY_DEFAULT, NULL)

  �����͵�ָ������ģʽ
  STATICPROPERTY("property", A3DCOLOR, m_a3c_value, NULL, NULL, WAY_COLOR, NULL)

  �ļ�����
  STATICPROPERTY("property", AUString, m_str_skytexture, NULL, NULL, WAY_FILENAME, "texture\\sky")

  ���ڴ�����ʽ�Ķ��壬���£�
	enum 
	{
		WAY_DEFAULT,		// ȱʡ��ʽ���ɱ༭���Զ����AVariant�����Ͳ�����
		WAY_BOOLEAN,		// ������������ʽ
		WAY_INTEGER,		// ���ʹ�����ʽ
		WAY_FLOAT,			// ���㴦����ʽ
		WAY_STRING,			// �ִ�������ʽ
		WAY_FILENAME,		// �ļ���
		WAY_COLOR,			// ��ɫֵ
		WAY_VECTOR,			// ����ֵ
		WAY_BINARY,			// �����ƿ�
		WAY_OBJECT,			// ����
		WAY_UNITID,			// ����ID
		WAY_PATHID,			// ·��ID
		WAY_STRID,			// (�ĵ���)�ַ�����ԴID
		WAY_SFXID,			// (�ĵ���)��Ч��ԴID
		// ������ʽ˵��
		WAY_READONLY	= (1 << 31),// ����ֻ������
	};
*/
