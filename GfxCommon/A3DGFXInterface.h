/*
 * FILE: A3DGFXInterface.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: zhangyachuan, 2011/2/24
 *
 * HISTORY:
 *
 * Copyright (c) 2011 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DGFXInterface_H_
#define _A3DGFXInterface_H_

#include <AString.h>
#include <ABaseDef.h>
#include "GfxCommonTypes.h"
#include "A3DGFXExMan.h"

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
	class EventInstance;
	class EventSystem;
}

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DGFXInterface
//
///////////////////////////////////////////////////////////////////////////

class A3DGFXInterface
{
public:		//	Types

	enum AttackOrientation
	{
		ATKORIENTATION_DEFAULT = 0, 
		ATKORIENTATION_UP,
		ATKORIENTATION_DOWN,
		ATKORIENTATION_LEFT,
		ATKORIENTATION_RIGHT,
		ATKORIENTATION_NUM,
	};

	struct PARAM_PLAYSKILLGFX
	{
		AString			strAtkFile;
		unsigned char	SerialID;
		clientid_t		nCasterID;
		clientid_t		nCastTargetID;
		A3DVECTOR3		vFixedPoint;
		int				nDivisions;
		TargetDataVec*	pTargets;
		DWORD			dwUserData;
		int				nAttIndex;
		int				nAttackOrientation;		//	0:Ĭ�� 1:�� 2:�� 3:�� 4:��
	};

public:		//	Constructors and Destructor

	A3DGFXInterface() {}
	virtual ~A3DGFXInterface() {}

public:		//	Attributes

public:		//	Operations

	//	���������������ǰ��AfxPlaySkillGfx�ص�����
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param) = 0;

	//	���ڴ������ͷ�AudioEngine�е�EventInstance����
	//	���º�������angelica.cfg�ļ��е�[a3d]���е�faudio_enable = 1�����û���fmod��AudioEngine��ʱ�ᱻ����
	//	���ڲ���AMedia������Ƶ���ŵ�����Ŀ�����Բ���ʵ��
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath) const { return NULL; }
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const {}
	virtual void SetEventSystem(AudioEngine::EventSystem* pEventSystem) {}
	//	����ͳ��GFXϵͳ�е�ǰ���Event Instance����
	virtual LONG GetCurActEventInstanceNum() const { return 0; }

protected:	//	Attributes

protected:	//	Operations

private:

	A3DGFXInterface(const A3DGFXInterface&);
	A3DGFXInterface& operator = (const A3DGFXInterface&);

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DGFXInterfaceDefImpl
//
///////////////////////////////////////////////////////////////////////////

class A3DGFXInterfaceDefImpl : public A3DGFXInterface
{
public:
	A3DGFXInterfaceDefImpl() {}
protected:
	//	ֱ�ӵ�����ǰ��AfxPlaySkillGfx
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param);
};


//	This is used in Angelica 2.2
class A3DGFXInterfaceDefImpl2 : public A3DGFXInterfaceDefImpl
{
public:
	A3DGFXInterfaceDefImpl2(AudioEngine::EventSystem* pEventSystem);

protected:
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath) const;
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const;
	virtual LONG GetCurActEventInstanceNum() const { return m_iCurActEventInstanceNum; }
	virtual void SetEventSystem(AudioEngine::EventSystem* pEventSystem) { m_pEventSystem = pEventSystem; }
private:
	AudioEngine::EventSystem* m_pEventSystem;
	mutable LONG m_iCurActEventInstanceNum;
};

#ifdef GFX_EDITOR

///////////////////////////////////////////////////////////////////////////
//
//	Class EditorGFXInterfaceDefImpl
//
///////////////////////////////////////////////////////////////////////////

class EditorGFXInterfaceDefImpl : public A3DGFXInterface
{
public:
	EditorGFXInterfaceDefImpl() {}
protected:
	
	virtual bool PlaySkillGfx(const PARAM_PLAYSKILLGFX* param);
};

#endif

///////////////////////////////////////////////////////////////////////////
//
//	inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DGFXInterface_H_

