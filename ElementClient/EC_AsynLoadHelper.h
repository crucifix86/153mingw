// Filename	: EC_AsynLoadHelper.h
// Creator	: Xu Wenbin
// Date		: 2014/11/28

#pragma once

#include <vector.h>

//	���������ڶ��߳�ģ�ͼ��ع��̼�¼�����ƣ������ڼ����ظ����ص�

//	���ر�ʶ������Ωһ��λ�������ڲ�ѯ�жϡ�ɾ������;
class CECAsynLoadID{
	int				m_loadType;		//	��������
	unsigned int	m_loadCounter;	//	���ؼ���
	unsigned long	m_requestTime;	//	�������ʱ��
public:
	CECAsynLoadID()
		: m_loadType(-1)
		, m_loadCounter(0)
		, m_requestTime(0)
	{
	}
	CECAsynLoadID(int type, int counter, unsigned long requestTime)
		: m_loadType(type)
		, m_loadCounter(counter)
		, m_requestTime(requestTime)
	{}
	int  GetLoadType()const{
		return m_loadType;
	}
	int  GetLoadCounter()const{
		return m_loadCounter;
	}
	unsigned long GetRequestTime()const{
		return m_requestTime;
	}
	bool operator == (const CECAsynLoadID &rhs)const{
		return GetLoadType() == rhs.GetLoadType()
			&& GetLoadCounter() == rhs.GetLoadCounter()
			&& GetRequestTime() == rhs.GetRequestTime();
	}
	bool IsValid()const{
		return m_requestTime > 0;
	}
};

//	���ر�ʶ�������������̣߳�ͬһ���������Ĳű�֤Ωһ��
class CECAsynLoadIDFactory{
	unsigned int	m_nextLoadCounter;
	
	//	����
	CECAsynLoadIDFactory(const CECAsynLoadIDFactory &);
	CECAsynLoadIDFactory & operator = (const CECAsynLoadIDFactory &);
public:
	CECAsynLoadIDFactory();
	CECAsynLoadID Generate(int loadType);
};

//	���ز�������
class CECAsynLoadContent{
public:
	virtual ~CECAsynLoadContent()=0;
	virtual CECAsynLoadContent * Clone()const=0;
	virtual bool operator == (const CECAsynLoadContent &)const=0;
	virtual float GetSortWeight()const=0;
	virtual void UpdateSortWeight()=0;
	virtual bool ThreadLoad()=0;
};

// Pure virtual destructor definition (required for linkage)
inline CECAsynLoadContent::~CECAsynLoadContent() {}

//	�ɹ���¼�ļ�������
class CECAsynLoadCommand{
	CECAsynLoadID			m_id;
	CECAsynLoadContent	*	m_pContent;

	//	����
	CECAsynLoadCommand(const CECAsynLoadCommand &);
	CECAsynLoadCommand & operator = (const CECAsynLoadCommand &);
public:
	CECAsynLoadCommand(const CECAsynLoadID &id, CECAsynLoadContent *pContent)
		: m_id(id)
		, m_pContent(pContent)
	{}
	~CECAsynLoadCommand();
	const CECAsynLoadID & GetID()const{
		return m_id;
	}
	CECAsynLoadContent * GetContent()const{
		return m_pContent;
	}
	void UpdateSortWeight();
	bool ThreadLoad();
};

//	class CECAsynLoadCommandMatcher
class CECAsynLoadCommandMatcher{
public:
	virtual ~CECAsynLoadCommandMatcher(){}
	virtual bool IsMatch(const CECAsynLoadCommand *pCommand)const=0;
};

//	�������
class CECAsynLoadCommandArray{
public:
	typedef abase::vector<CECAsynLoadCommand *> CommandArray;
	typedef CommandArray::iterator	Iterator;

private:
	CommandArray	m_commandArray;

public:
	CECAsynLoadCommandArray();
	CECAsynLoadCommandArray(const CommandArray &);
	void Assign(const CommandArray &commandArray);
	
	void Append(CECAsynLoadCommand *pCommand);
	void Remove(Iterator it);
	int  RemoveIf(CECAsynLoadCommandMatcher *pMatcher);
	void Clear();

	void UpdateSortWeight();
	CECAsynLoadCommandArray FetchNearest(int count);
	CECAsynLoadCommandArray FetchAll();
	
	int Count()const;
	bool IsEmpty()const;
	CECAsynLoadCommand *GetAt(int index);

	Iterator Begin();
	Iterator End();
	Iterator Find(const CECAsynLoadID &id);								//	���ڸ��ݼ��ؽ��ɾ�����ؼ�¼
	Iterator Find(int loadType, const CECAsynLoadContent *pContent);	//	���ڲ����Ƿ��ڼ���ָ������
};

//	���ض���
class CECAsynLoadCenter{
	CECAsynLoadCommandArray	m_commandArray;
	CECAsynLoadIDFactory	m_idFactory;

	//	����
	CECAsynLoadCenter(const CECAsynLoadCenter &);
	CECAsynLoadCenter & operator = (const CECAsynLoadCenter &);

public:
	CECAsynLoadCenter();
	~CECAsynLoadCenter();

	bool Append(int loadType, CECAsynLoadContent *pContent, CECAsynLoadID &id);
	void Clear();

	CECAsynLoadCommandArray & GetCommandArray(){
		return m_commandArray;
	}
};
