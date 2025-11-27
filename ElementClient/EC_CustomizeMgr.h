/*
 * FILE: EC_CustomizeMgr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_CUSTOMIZE_MGR_H_
#define _EC_CUSTOMIZE_MGR_H_

#ifdef _WIN32
#pragma once
#endif


#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A3DFont.h"
#include "A3DSurfaceMan.h"
#include "A3DSurface.h"
#include "A3DCamera.h"
#include "A3DFuncs.h"

#include "AIniFile.h"


class AUIManager;
class CECPlayer;
class AUIDialog;
class CECCustomizeBound;
class CECFace;
class AUIStillImageButton;
class CECCustomizePolicy;

// Forward declarations for customize dialog classes
class CDlgCustomize;
class CDlgCustomizeBase;
class CDlgCustomizePre;
class CDlgCustomizeStature;
class CDlgCustomizeEye;
class CDlgCustomizeEye2;
class CDlgCustomizeFace;
class CDlgCustomizeFace2;
class CDlgCustomizeHair;
class CDlgCustomizeNoseMouth;
class CDlgCustomizeNoseMouth2;
class CDlgCustomizePaint;
class CDlgFaceName;


#define FACE_TICKET_DEFAULT -1//ȱʡ���Ʒ�Χ

#define FACE_SCENE_STATURE 0
#define FACE_SCENE_HEAD 1

class CECCustomizeMgr
{
	friend class CDlgCustomizeBase;
	friend class CDlgCustomizeEye;
	friend class CDlgCustomizeEye2;
	friend class CDlgCustomizeFace;
	friend class CDlgCustomizeFace2;
	friend class CDlgCustomizeNoseMouth;
	friend class CDlgCustomizeNoseMouth2;
	friend class CDlgCustomizeHair;
	friend class CDlgCustomizePaint;
	friend class CDlgCustomizePre;
	friend class CDlgCustomizeStature;
	friend class CDlgCustomizeMoveCamera;
	friend class CDlgCustomize;
	friend class CDlgFaceName;
	friend class CDlgCustomizeEar;

public:
	

	struct CustomizeInitParam_t
	{
		AUIManager* pAUImanager;
		CECPlayer* pCurPlayer;
		A3DVECTOR3 vPos;
		A3DVECTOR3 vDir;
		int nFaceTicketID;
	};

	bool Test(const CustomizeInitParam_t &curInitParam)const;
	bool Init(const CustomizeInitParam_t cusInitParam, CECCustomizePolicy *policy);
	void RestorePlayer(CECPlayer *pPlayer)const;
	bool IsWorking()const;
	void Finish();
	void HideRelatedDialogs();

	CECPlayer * GetPlayer(){ return m_pCurPlayer; }
	void ConfirmPlayerCustomizeData();
	void RestoreCamera();

	bool GetDragRole() { return m_bDragRole; }
	bool GetDragCamera() { return m_bDragCamera; }
	void ResetDragRole(bool bflag);
	void ResetDragCamera(bool bflag);
	void InitDragRole(LPARAM lParam);
	void InitDragCamera(LPARAM lParam);
	void DragRole(LPARAM lParam);
	void DragCamera(LPARAM lParam);
	void MouseWheel(WPARAM wParam);
	void ResetCursor();
	void Tick();
	bool CustomizeDealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool AddRestoreDialog(AUIDialog* pDlg);
	bool RemoveRestoreDialog(AUIDialog* pDlg);

	CECCustomizeMgr();
	void Release();
	
	CECCustomizeBound* GetSliderBound(void){ return m_pSliderBound;}
	void ChangeScene(int nScene);

	A2DSprite * GetImageSelected();
	A2DSprite * GetImageMouseOn();
	
	void HideAllOldDlg();
	void ShowAllOldDlg();

protected:
	bool InitDefaultData(void);
	bool InitSwithButtons();
	bool InitDialogs();
	CECCustomizeBound * GetSliderBound(int nFaceTicketID)const;

	bool CalculateAdvaceOption(void);
	int MakeSureSlider(unsigned char &level, int nMax, int nMin);
	void SwitchDialog(const char * szCommand);

	void OnOK();
	void OnCancel();
	
protected:
	abase::vector<AUIDialog*> m_vecOldDlg;
	typedef abase::vector<AUIDialog*>::iterator OldDlgVecIter_t;

	CECCustomizePolicy * m_pCustomizePolicy;
	
	CECCustomizeBound *m_pSliderBound;
	AUIManager * m_pAUIManager;
	CECPlayer * m_pCurPlayer;
	A3DVECTOR3	m_vPlayerSavedPos;
	A3DVECTOR3	m_vPlayerSavedDir;
	A3DVECTOR3	m_vPlayerSavedUp;
	A3DVECTOR3 m_vNPCPos;
	A3DVECTOR3 m_vNPCDir;
	A3DVECTOR3 m_vPosDaltaM;
	A3DVECTOR3 m_vPosDaltaF;

	bool m_bUnsymmetrical;//�ǶԳ�

public:
	bool m_bLoadFacePillData;//�Ƿ������������ص�һЩ��������

protected:
	A2DSprite * m_pSpriteImageSelect;
	A2DSprite * m_pSpriteImageMouseOn;

	// Dialogs
	CDlgCustomizePre	* m_pDlgCustomizePre;
	CDlgCustomizeStature * m_pDlgCustomizeStature;
	CDlgCustomizeEye * m_pDlgCustomizeEye;
	CDlgCustomizeEye2 * m_pDlgCustomizeEye2;
	CDlgCustomizeFace2 * m_pDlgCustomizeFace2;
	CDlgCustomizeHair * m_pDlgCustomizeHair;
	CDlgCustomizeNoseMouth * m_pDlgCustomizeNoseMouth;
	CDlgCustomizeNoseMouth2 * m_pDlgCustomizeNoseMouth2;
	CDlgCustomizePaint * m_pDlgCustomizePaint;
	CDlgCustomizeBase * m_pCurDialog; // current dialog
	
	CDlgCustomize* m_pDlgCustomize;
	CDlgFaceName* m_pDlgFacename;
	//CDlgCustomizeEar* m_pDlgCustomizeEar;

	// role draging when create and customize
	bool		m_bDragRole;		// flag indicates whether drag the role to rotate
	
	// camera draging when create and customize
	bool        m_bDragCamera;      // flag indicates whether drag the camera to rotate
	
	APointI		m_ptDragStart;		// drag start position
	

	//----------------���ð�ť�ؼ�---------------------
	AUIStillImageButton* m_pBtnChoosePreCustomize;
	AUIStillImageButton* m_pBtnChooseFace;
	AUIStillImageButton* m_pBtnChooseEye;
	AUIStillImageButton* m_pBtnChooseNoseMouth;
	AUIStillImageButton* m_pBtnChooseHair;
	AUIStillImageButton* m_pBtnChooseFacePainting;

	AUIStillImageButton* m_pBtnCurChoose;//��ǰ��ť
	//AUIStillImageButton* m_pBtnChooseEar;
	
	//------------�����������Զ�����صĹ�������ʼλ��

	//3ͥ3
	int m_nDefaultScaleUp;
	int m_nDefaultScaleMiddle;
	int m_nDefaultScaleDown ;

	//��������2
	int m_nDefaultScaleFaceH;
	int m_nDefaultScaleFaceV;
	

	//�۾�14
	int m_nDefaultScaleEyeH;
	int m_nDefaultScaleEyeV;
	int m_nDefaultRotateEye;
	int m_nDefaultOffsetEyeH;
	int m_nDefaultOffsetEyeV;
	int m_nDefaultOffsetEyeZ;
	int m_nDefaultScaleEyeBall;	

	int m_nDefaultScaleEyeH2;
	int m_nDefaultScaleEyeV2;
	int m_nDefaultRotateEye2;
	int m_nDefaultOffsetEyeH2;
	int m_nDefaultOffsetEyeV2;
	int m_nDefaultOffsetEyeZ2;
	int m_nDefaultScaleEyeBall2;	


	//üë6
	int m_nDefaultScaleBrowH;
	int m_nDefaultScaleBrowV;
	int m_nDefaultRotateBrow;
	int m_nDefaultOffsetBrowH;
	int m_nDefaultOffsetBrowV;
	int m_nDefaultOffsetBrowZ;

	int m_nDefaultScaleBrowH2;
	int m_nDefaultScaleBrowV2;
	int m_nDefaultRotateBrow2;
	int m_nDefaultOffsetBrowH2;
	int m_nDefaultOffsetBrowV2;
	int m_nDefaultOffsetBrowZ2;
		
	//����5
	int m_nDefaultScaleNoseTipH;
	int m_nDefaultScaleNoseTipV;
	int m_nDefaultScaleNoseTipZ;
	int m_nDefaultOffsetNoseTipV;
	int m_nDefaultScaleBridgeTipH;
	int m_nDefaultOffsetBridegTipZ;
	
	
	//��6
	int m_nDefaultThickUpLip;
	int m_nDefaultThickDownLip;
	int m_nDefaultScaleMouthH;
	int m_nDefaultOffsetMouthV ;
	int m_nDefaultOffsetMouthZ ;
	int m_nDefaultOffsetCornerOfMouthSpecial;
	int m_nDefaultScaleMouthH2;
	int m_nDefaultOffsetCornerOfMouthSpecial2;

	//����2
	int m_nDefaultScaleEar;
	int m_nDefaultOffsetEarV;
	
	//��ͷ5
	int m_nDefaultOffsetForeheadH;
	int m_nDefaultOffsetForeheadV;
	int m_nDefaultOffsetForeheadZ;
	int m_nDefaultRotateForehead;
	int m_nDefaultScaleForehead;

	//ȧ��5
	int m_nDefaultOffsetYokeBoneH ;
	int m_nDefaultOffsetYokeBoneV;
	int m_nDefaultOffsetYokeBoneZ ;
	int m_nDefaultRotateYokeBone;
	int m_nDefaultScaleYokeBone;

	//����4
	int m_nDefaultOffsetCheekH ;
	int m_nDefaultOffsetCheekV;
	int m_nDefaultOffsetCheekZ;
	int m_nDefaultScaleCheek ;


	//�°�4
	int m_nDefaultOffsetChainV;
	int m_nDefaultOffsetChainZ;
	int m_nDefaultRotateChain;
	int m_nDefaultScaleChainH;


	//��6
	int m_nDefaultOffsetJawH;
	int m_nDefaultOffsetJawV;
	int m_nDefaultOffsetJawZ;
	int m_nDefaultScaleJawSpecial;
	int m_nDefaultScaleJawH;
	int m_nDefaultScaleJawV;

	//����
	int m_nDefaultHeadScale;
	int m_nDefaultUpScale;
	int m_nDefaultWaistScale;
	int m_nDefaultArmWidth;
	int m_nDefaultLegWidth;
	int m_nDefaultBreastScale;

	//MoveCamera �Ի��� ��������������ƶ�
	float m_fMoveZDist;
	float m_fMoveXDist;
	float m_fMoveYDist;

public:
	CDlgCustomizeFace * m_pDlgCustomizeFace;

};


#endif