/*
 * FILE: EC_Manager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once
#include "cxxpool.h"

#include "EC_MsgDef.h"

#include <vector>
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Manager index
enum
{
	MAN_PLAYER = 0,		//	Player manager
	MAN_NPC,			//	NPC manager
	MAN_MATTER,			//	Matter manager
	MAN_ORNAMENT,		//	Ornament manager
	MAN_SKILLGFX,		//	Skill Gfx Manager
	MAN_ATTACKS,		//	Attack Manager
	MAN_DECAL,			//	Decal manager
	NUM_MANAGER,
};

//	Manager mask
enum
{
	MANMASK_PLAYER		= 0x0001,
	MANMASK_NPC			= 0x0002,
	MANMASK_MATTER		= 0x0004,
	MANMASK_ORNAMENT	= 0x0008,
	MANMASK_SKILLGFX	= 0x0010,
	MANMASK_ATTACKS		= 0x0020,
	MANMASK_DECAL		= 0x0040,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECViewport;
class CECGameRun;
struct ECRAYTRACE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

#define PARALLEL true

extern unsigned int g_cpucores;
extern cxxpool::thread_pool* g_pTickThreadPool;
template <typename T>
class CECParallelTickManager
{
	std::vector<std::future<void>> futures;
	std::vector<T> elems;
	bool parallel;
public:
	CECParallelTickManager() : parallel(PARALLEL)
	{
	}
	void DoSequential(bool value)
	{
		parallel = !value;
	}
	void AddElem(T value, DWORD dwDeltaTime)
	{
		elems.push_back(value);
	}
	void Tick(DWORD dwDeltaTime)
	{
		if (parallel)
		{
			int thread = g_cpucores;
			int numBatches = 0;
			int numElemsPerThread = 0;
			do
			{
				numElemsPerThread = elems.size() / (thread);
				if (elems.size() % (thread) != 0)
					numElemsPerThread++;
				numElemsPerThread = max(2, numElemsPerThread);
				numBatches = elems.size() / numElemsPerThread;
				if (elems.size() % numElemsPerThread != 0)
					numBatches += 1;
				thread--;
			} while (numBatches > (int)g_cpucores);
			for (int i = 0; i < numBatches; i++)
			{
				futures.emplace_back(g_pTickThreadPool->push([this, i, numElemsPerThread, dwDeltaTime]
					{
						int startIndex = i * numElemsPerThread;
						int endIndex = min((int)elems.size(), startIndex + numElemsPerThread);
						for (int j = startIndex; j < endIndex; j++)
						{
							elems[j]->Tick(dwDeltaTime);
						}
					})
				);
			}
			cxxpool::wait(futures.begin(), futures.end());
		}
		else
		{
			for (int i = 0, max = elems.size(); i < max; i++)
			{
				T ptr = elems[i];
				ptr->Tick(dwDeltaTime);
			}
		}
	}
};
template <typename T>
class CECParallelTickOnlyAnimationManager
{
	std::vector<std::future<void>> futures;
	std::vector<T> elems;
	bool parallel;
public:
	CECParallelTickOnlyAnimationManager() : parallel(PARALLEL)
	{
	}
	void DoSequential(bool value)
	{
		parallel = !value;
	}
	void AddElem(T value, DWORD dwDeltaTime)
	{
		elems.push_back(value);
	}
	void TickAnimation(DWORD dwDeltaTime)
	{
		if (parallel)
		{
			int thread = g_cpucores;
			int numBatches = 0;
			int numElemsPerThread = 0;
			do
			{
				numElemsPerThread = elems.size() / (thread);
				if (elems.size() % (thread) != 0)
					numElemsPerThread++;
				numElemsPerThread = max(2, numElemsPerThread);
				numBatches = elems.size() / numElemsPerThread;
				if (elems.size() % numElemsPerThread != 0)
					numBatches += 1;
				thread--;
			} while (numBatches > (int)g_cpucores);
			for (int i = 0; i < numBatches; i++)
			{
				futures.emplace_back(g_pTickThreadPool->push([this, i, numElemsPerThread, dwDeltaTime]
					{
						int startIndex = i * numElemsPerThread;
						int endIndex = min((int)elems.size(), startIndex + numElemsPerThread);
						for (int j = startIndex; j < endIndex; j++)
						{
							elems[j]->TickOnlyAnimation(dwDeltaTime);
						}
					})
				);
			}
			cxxpool::wait(futures.begin(), futures.end());
		}
		else
		{
			for (int i = 0, max = elems.size(); i < max; i++)
			{
				T ptr = elems[i];
				ptr->TickOnlyAnimation(dwDeltaTime);
			}
		}
	}
};
template <typename T>
class CECParallelSkillGFXTickAnimationManager
{
	std::vector<std::future<void>> futures;
	std::vector<T> elems;
	std::vector<bool> elemsBools;
	bool parallel;
public:
	CECParallelSkillGFXTickAnimationManager() : parallel(PARALLEL)
	{
	}
	void DoSequential(bool value)
	{
		parallel = !value;
	}
	void AddElem(T value, DWORD dwDeltaTime, bool useDeltaTime)
	{
		elems.push_back(value);
		elemsBools.push_back(useDeltaTime);
	}
	void TickAnimation(DWORD dwDeltaTime)
	{
		if (parallel)
		{
			int thread = g_cpucores;
			int numBatches = 0;
			int numElemsPerThread = 0;
			do
			{
				numElemsPerThread = elems.size() / (thread);
				if (elems.size() % (thread) != 0)
					numElemsPerThread++;
				numElemsPerThread = max(2, numElemsPerThread);
				numBatches = elems.size() / numElemsPerThread;
				if (elems.size() % numElemsPerThread != 0)
					numBatches += 1;
				thread--;
			} while (numBatches > (int) g_cpucores);
			for (int i = 0; i < numBatches; i++)
			{
				futures.emplace_back(g_pTickThreadPool->push([this, i, numElemsPerThread, dwDeltaTime]
					{
						int startIndex = i * numElemsPerThread;
						int endIndex = min((int)elems.size(), startIndex + numElemsPerThread);
						for (int j = startIndex; j < endIndex; j++)
						{
							elems[j]->TickAnimation( elemsBools[j] ? dwDeltaTime : 0);
						}
					})
				);
			}
			cxxpool::wait(futures.begin(), futures.end());
		}
		else
		{
			for (int i = 0, max = elems.size(); i < max; i++)
			{
				T ptr = elems[i];
				ptr->TickAnimation(dwDeltaTime);
			}
		}
	}
};
template <typename T>
class CECParallelECMGFXTickAnimationManager
{
	std::vector<std::future<void>> futures;
	std::vector<T> elems;
	bool parallel;
public:
	CECParallelECMGFXTickAnimationManager() : parallel(PARALLEL)
	{
	}
	void DoSequential(bool value)
	{
		parallel = !value;
	}
	void AddElem(T value, DWORD dwDeltaTime)
	{
		elems.push_back(value);
	}
	void AddAll(std::vector<T> elemsToAdd)
	{
		elems.insert(elems.end(), elemsToAdd.begin(), elemsToAdd.end());
	}
	void TickAnimation(DWORD dwDeltaTime)
	{
		if (parallel)
		{
			int thread = g_cpucores;
			int numBatches = 0;
			int numElemsPerThread = 0;
			do
			{
				numElemsPerThread = elems.size() / (thread);
				if (elems.size() % (thread) != 0)
					numElemsPerThread++;
				numElemsPerThread = max(2, numElemsPerThread);
				numBatches = elems.size() / numElemsPerThread;
				if (elems.size() % numElemsPerThread != 0)
					numBatches += 1;
				thread--;
			} while (numBatches > (int)g_cpucores);
			for (int i = 0; i < numBatches; i++)
			{
				futures.emplace_back(g_pTickThreadPool->push([this, i, numElemsPerThread, dwDeltaTime]
					{
						int startIndex = i * numElemsPerThread;
						int endIndex = min((int)elems.size(), startIndex + numElemsPerThread);
						for (int j = startIndex; j < endIndex; j++)
						{
							elems[j]->TickAnimation(dwDeltaTime);
						}
					})
				);
			}
			cxxpool::wait(futures.begin(), futures.end());
		}
		else
		{
			for (int i = 0, max = elems.size(); i < max; i++)
			{
				T ptr = elems[i];
				ptr->TickAnimation(dwDeltaTime);
			}
		}
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECManager
//	
///////////////////////////////////////////////////////////////////////////

class CECManager
{
public:		//	Types

public:		//	Constructor and Destructor

	CECManager(CECGameRun* pGameRun);
	virtual ~CECManager() {}

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release() {}

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }
	//	Render routine
	virtual bool Render(CECViewport* pViewport) { return true; }
	//	RenderForReflected routine
	virtual bool RenderForReflect(CECViewport * pViewport) { return true; }
	//	RenderForRefract routine
	virtual bool RenderForRefract(CECViewport * pViewport) { return true; }
	//	Tick animation
	virtual bool TickAnimation() { return true; }

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg) { return true; }

	//	On entering game world
	virtual bool OnEnterGameWorld() { return true; }
	//	On leaving game world
	virtual bool OnLeaveGameWorld() { return true; }

	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo) { return false; }

	//	Get tick time (ms) of this frame
	DWORD GetTickTime() { return m_dwCurTickTime; }
	//	Get manager ID
	int GetManagerID() { return m_iManagerID; }

protected:	//	Attributes

	int			m_iManagerID;		//	Manager's ID
	CECGameRun* m_pGameRun;			//	Game run object
	DWORD		m_dwCurTickTime;	//	Logic tick time

protected:	//	Operations

	//	Begin recroding tick time
	void BeginTickTime();
	//	End recroding tick time
	void EndTickTime();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



