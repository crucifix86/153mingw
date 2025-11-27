// Filename	: DlgPetEvolutionAnim.h
// Creator	: Wang Yongdong
// Date		: 2012/12/04

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include <A3DGeometry.h>
#include "EC_Counter.h"

class A3DGFXEx;
class CECModel;

class CDlgPetEvoAnim: public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
		
public:
	void DoDataExchange(bool bSave);

	CDlgPetEvoAnim();
	virtual ~CDlgPetEvoAnim();
	
	void SetPet(int tid, int evo_id);	
	
	void Reset();
	
	void OnCommand_CANCEL(const char * szCommand);
	void ResizeWindow(A3DRECT *prc);

protected:

	PAUIIMAGEPICTURE m_pImg_Pet;
	
	int	m_idPet;
	int m_idEvo; //������ģ��
	CECModel* m_pModel;
	CECModel* m_pNewModel;
	CECModel* m_pCurModel;

	A3DAABB		m_aabb;			// ����ص�������aabb
	A3DAABB		m_newaabb;		// ��ģ�͵�aabb
	
	CECCounter m_counter; // �رն��������ʱ
	CECCounter m_lagCounter; // �ȴ�2��
	CECCounter m_lastCounter;
	CECCounter m_incubatorCounter;

	float  	m_fScale;

	bool	m_bStandFlag; // վ��
	bool	m_bLastState;

protected:
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

	CECModel * LoadPetModel(int idPet);

	void UpdateRenderCallback();
	void ComputeAABB(CECModel* pModel, A3DAABB& aabb);

};