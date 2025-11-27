// File		: EC_Observer.h
// Creator	: Xu Wenbin
// Date		: 2014/4/10

#ifndef _ELEMENTCLIENT_EC_OBSERVER_H_
#define _ELEMENTCLIENT_EC_OBSERVER_H_

#include <vector.h>
#include <algorithm>
#include <ABaseDef.h>

//	class CECObservableChange
//	�� CECObservable �ı�ľ������ݷ�װ
class CECObservableChange
{
public:
	virtual ~CECObservableChange(){}
};

//	class CECObserver
//	�۲��߻��ࣺ������� Model
template <typename Model>
class CECObserver
{
public:
	virtual ~CECObserver(){}
	virtual void OnRegistered(const Model *p){}
	virtual void OnModelChange(const Model *p, const CECObservableChange *q){}
	virtual void OnUnregister(const Model *p){}
};

//	class CECObservable
//	�ɹ۲��߻��ࣺ������� Model
//	�ṩ�۲���ע�ἰ֪ͨ����
template <typename Model>
class CECObservable
{
public:
	typedef CECObserver<Model>			Observer;

private:
	struct ObserverImpl
	{
		Observer *	m_observer;					//	�۲���ָ��
		int			m_observeTimes;				//	�۲������Ϊ����ʱ����Ϊ1���յ��˴����� OnModelChange ��Ϣ�����Զ�ȡ��ע�ᣬΪ����ʱΪ���ã�����Ϊ0ֵ��
		bool		m_autoDeleteOnUnregister;	//	ȡ��ע��ʱ�Ƿ��Զ� delete

		ObserverImpl(Observer *observer, int observeTimes, bool autoDeleteOnUnregister)
			: m_observer(observer)
			, m_observeTimes(observeTimes)
			, m_autoDeleteOnUnregister(autoDeleteOnUnregister){
			if (m_observeTimes == 0){
				ASSERT(false);
				m_observeTimes = 1;		//	�������۲����Ϊ0
			}
		}

		bool operator == (const ObserverImpl &rhs)const{
			return this->m_observer == rhs.m_observer;
		}
		bool operator == (const Observer * pObserver)const{
			return this->m_observer == pObserver;
		}

		bool WillNotObserve()const{
			//	�Ƿ�����۲�Ŀ��
			return m_observeTimes == 0;
		}

		bool ObserveOnce(){
			//	����һ�ι۲���Ϣʱ���ã������ٹ۲�ʱ���� true
			if (m_observeTimes > 0){
				-- m_observeTimes;
			}
			return WillNotObserve();
		}
	};
	typedef abase::vector<ObserverImpl>	Observers;
	Observers m_observers;

	const Model * AsModel()const{
		return dynamic_cast<const Model *>(this);
	}

public:
	virtual ~CECObservable(){}
	
	bool IsObserverRegistered(const Observer *pObserver)const{
		return std::find(m_observers.begin(), m_observers.end(), pObserver) != m_observers.end();
	}
	bool RegisterObserver(Observer *pObserver, int observeTimes=-1, bool autoDeleteOnUnregister=false){
		bool bRegistered(false);
		if (!IsObserverRegistered(pObserver)){
			m_observers.push_back(ObserverImpl(pObserver, observeTimes, autoDeleteOnUnregister));
			pObserver->OnRegistered(AsModel());
			bRegistered = true;
		}
		return bRegistered;
	}
	bool UnregisterObserver(Observer *pObserver){
		typename Observers::iterator it = std::find(m_observers.begin(), m_observers.end(), pObserver);
		return UnregisterObserverImpl(it);
	}
	void NotifyObservers(const CECObservableChange *pChange){
		const Model *pModel = AsModel();
		typename Observers::iterator it = m_observers.begin();
		while (it != m_observers.end()){
			ObserverImpl &observerImpl = *it;
			observerImpl.m_observer->OnModelChange(pModel, pChange);
			if (observerImpl.ObserveOnce()){
				UnregisterObserverImpl(it);
			}else{
				it ++;
			}
		}
	}
private:
	bool UnregisterObserverImpl(typename Observers::iterator &it){
		bool bUnRegistered(false);
		if (it != m_observers.end()){
			ObserverImpl &observerImpl = *it;
			observerImpl.m_observer->OnUnregister(AsModel());
			if (observerImpl.m_autoDeleteOnUnregister){
				delete observerImpl.m_observer;
			}
			it = m_observers.erase(it);
			bUnRegistered = true;
		}
		return bUnRegistered;
	}
};

/* ������������

#include "EC_Observer.h"

class CECObserableExampleChange : public CECObservableChange
{
public:
	enum ChangeType{
		CT_DATA1,
		CT_DATA2,
	};
private:
	ChangeType	m_changeType;
public:
	CECObserableExampleChange(ChangeType changeType) : m_changeType(changeType){}
	ChangeType GetType()const{ return m_changeType; }
};

class CECObserableExample : public CECObservable<CECObserableExample>
{
	int	m_data1;
	int m_data2;
public:
	CECObserableExample(int data1, int data2) : m_data1(data1), m_data2(data2) {
	}
	int Data1()const{
		return m_data1;
	}
	int Data2()const{
		return m_data2;
	}
	void SetData1(int newData){
		m_data1 = newData;
		CECObserableExampleChange change(CECObserableExampleChange::CT_DATA1);
		NotifyObservers(&change);
	}
	void SetData2(int newData){
		m_data2 = newData;
		CECObserableExampleChange change(CECObserableExampleChange::CT_DATA2);
		NotifyObservers(&change);
	}
};

class CECObserableExampleObserver : public CECObserver<CECObserableExample>
{
	const char *m_name;
public:
	CECObserableExampleObserver(const char *name) : m_name(name){
	}
	const char *Name()const{
		return m_name;
	}
	virtual ~CECObserableExampleObserver(){
		a_LogOutput(1, "CECObserableExampleObserver(%s)::dtor()", Name());
	}
	virtual void OnRegistered(const CECObserableExample *p){
		a_LogOutput(1, "CECObserableExampleObserver(%s)::OnRegistered(data1=%d, data2=%d)", Name(), p->Data1(), p->Data2());
	}
	virtual void OnModelChange(const CECObserableExample *p, const CECObservableChange *pChange){
		const CECObserableExampleChange *q = dynamic_cast<const CECObserableExampleChange *>(pChange);
		if (q->GetType() == CECObserableExampleChange::CT_DATA1){
			a_LogOutput(1, "CECObserableExampleObserver(%s)::OnModelChange(data1=%d)", Name(), p->Data1());
		}else{
			a_LogOutput(1, "CECObserableExampleObserver(%s)::OnModelChange(data2=%d)", Name(), p->Data2());
		}
	}
	virtual void OnUnregister(const CECObserableExample *p){
		a_LogOutput(1, "CECObserableExampleObserver(%s)::OnUnregister(data1=%d, data2=%d)", Name(), p->Data1(), p->Data2());
	}
};

CECObserableExample example(-1, -10);
CECObserableExampleObserver *observer1 = new CECObserableExampleObserver("observer1");
CECObserableExampleObserver observer2("observer2");
CECObserableExampleObserver *observer3 = new CECObserableExampleObserver("observer3");
CECObserableExampleObserver observer4("observer4");
CECObserableExampleObserver *observer5 = new CECObserableExampleObserver("observer5");
CECObserableExampleObserver observer6("observer6");
example.RegisterObserver(observer1, 1, true);
example.RegisterObserver(&observer2, 1, false);
example.RegisterObserver(observer3, 2, true);
example.RegisterObserver(&observer4, 2, false);
example.RegisterObserver(observer5, -1, true);
example.RegisterObserver(&observer6, -1, false);
example.SetData1(10);
example.SetData2(20);
example.SetData1(30);
delete observer5;

*/

#endif	//	_ELEMENTCLIENT_EC_OBSERVER_H_