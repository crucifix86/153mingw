/*
* FILE: PropertyInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2011/4/15
*
* HISTORY: 
*
* Copyright (c) 2011 Archosaur Studio, All Rights Reserved.
*/

#ifndef _PropertyInterface_H_
#define _PropertyInterface_H_

#include "A3DVector.h"
#include "AString.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

namespace AudioEngine
{
	class EventSystem;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class APropertyInterface
//	˵��������������PropDlg�ؼ�����һЩ��չ����
//	�����ҪPropDlg�ؼ�֧��������Ƶ�����Project��Event���ѡ����(AProperty�е�WAY_AUDIOEVENT)
//	��Ҫ�ӱ���������ʵ��GetEventSystem�ӿڣ�����ָ��ͨ��SetPropertyInterface��������
//	
///////////////////////////////////////////////////////////////////////////

class APropertyInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	virtual ~APropertyInterface(void) {}

public:		//	Attributes

public:		//	Operations

	static APropertyInterface* GetPropertyInterface();
	static void SetPropertyInterface(APropertyInterface* pInterface);

	virtual AudioEngine::EventSystem* GetEventSystem() const = 0;
	//	Camera position is used
	virtual A3DVECTOR3 GetCamPos() const = 0;

protected:	//	Attributes

	static APropertyInterface* s_Interface;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	class ADefaultPIImpl
//
///////////////////////////////////////////////////////////////////////////

class ADefaultPIImpl : public APropertyInterface
{
public:
	ADefaultPIImpl(AudioEngine::EventSystem* pEventSystem)
		: m_pEventSystem(pEventSystem)
	{

	}

	virtual AudioEngine::EventSystem* GetEventSystem() const { return m_pEventSystem; }

private:
	AudioEngine::EventSystem* m_pEventSystem;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_PropertyInterface_H_

