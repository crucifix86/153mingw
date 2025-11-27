//////////////////////////////////////////////////////////////////////
//	Created by He wenfeng
//  2005-01-10
//  A class derived from the class CConvexHullData 
//  It is used for Collision Detection and Path finding in 2 2D environment.
//  It uses the form of CConvexHullData, while, in fact it only represents a 2D obstruct
//  To note is that it is not  convex when it describes the Terrain obstruct or water area!
//////////////////////////////////////////////////////////////////////

#ifndef _2DOBSTRUCT_H_
#define _2DOBSTRUCT_H_

#include "ConvexHullData.h"

#include <vector.h>
using namespace abase;

//extern ALog g_Log;

namespace CHBasedCD
{

class C2DObstruct : public CConvexHullData  
{
	//������������
	enum 
	{
		CLOCKWISE=1,
		ANTICLOCKWISE
	};

public:

	C2DObstruct();
	virtual ~C2DObstruct();
	
	//�����ݽ��б任
	virtual void Transform(const A3DMATRIX4& mtxTrans);

	//////////////////////////////////////////////////////////////////////////
	// �ļ������������װ��
	// �����˳�Աm_arrInflateVecs������븲�ǻ���ķ�����
	//////////////////////////////////////////////////////////////////////////
	//��Ӧһ���ļ�FILE����
	virtual bool LoadFromStream(FILE* InFile);
	virtual bool SaveToStream(FILE* OutFile) const;

	/************************************************************************
	// Removed by wenfeng, 05-3-28
	// ���������г���std::fstream�Ŀ⣬����������ҪVC��֧�֣�

	//��Ӧһ���ļ���
	virtual bool LoadFromStream(std::ifstream& InFile);
	virtual bool SaveToStream(std::ofstream& OutFile) const;
	/************************************************************************/

	// �ж�ĳһ�����Ƿ�Ϊ���㣡
	bool IsVertexConcave(int vid);

	//����һ��㼯��ʼ�����㼯������ά������ʽ������Ч�Ľ�Ϊx,z����
	void Init(const vector<A3DVECTOR3>& Vertices, float fHeight=1.0f);

	///////////////////////////////////////////////////
	// ������ײ����ƶ�����
	///////////////////////////////////////////////////

	//�����ײ��Ӧ����Ϣ������������һ�м̵�
	void GetCRInfo(CFace* pCDFace,const A3DVECTOR3& Dir,int& RotateDir,int& RelayVID,int CurRotateDir=0);

	
	//���ݷ����ҳ���һ������id
	inline int GetNextVID(int CurVID,int RotateDir);

	//�������Ʒ����ҵ���vid�ڽӵ�Face
	//vidӦ���ǵ����ϵ�id������Ӧ��Ϊż��
	inline CFace* GetFaceByVID(int vid, int RotateDir);

	A3DVECTOR3 GetInflateVector(int vid) {  return m_arrInflateVecs[vid]; }
	
	//�Զ�����һ������
	A3DVECTOR3 GetInflateVertex(int vid, float fInflateSize)
	{
		return (m_arrVertices[vid]+fInflateSize*m_arrInflateVecs[vid>>1]);
	}

protected:
	//���ǻ���ķ���
	virtual int ComputeBufSize() const;					//���㱣�浽AFile�Ļ�������С
	virtual bool WriteToBuf(char* buf) const;		  //��ȫ������д�뵽������buf�У���ǰbuf�����Ѿ�����
	virtual bool ReadFromBuf(char* buf);				//��buf�ж�������

protected:
	
	//���������б�����Ӧ��ÿһ��2D���㡣������ײ�ƶ������жԶ��������������
	//���Ӧ����v����������ΪvInflate,����ڰ뾶Ϊr�����壬�ö��㽫��������v+r*vInflate
	AArray<A3DVECTOR3,const A3DVECTOR3&> m_arrInflateVecs;		

};

inline int C2DObstruct::GetNextVID(int CurVID,int RotateDir)
{
	if(RotateDir==ANTICLOCKWISE) 
	{
		return (CurVID+2)%GetVertexNum();
	}
	else if(RotateDir==CLOCKWISE )
	{
		return (CurVID+GetVertexNum()-2)%GetVertexNum();
	}
	return -1;			//�������������һ����Чֵ
}

inline CFace* C2DObstruct::GetFaceByVID(int vid, int RotateDir)
{
	int fid;
	if(RotateDir==CLOCKWISE )
		fid=GetNextVID(vid,RotateDir);
	else
		fid=vid;
	fid=(fid>>1)+2;			//����һ��Ҫע�����ȼ� ������>>!

	if(fid<0 || fid>GetFaceNum()-1)
		return NULL;
	
	//ע�⣡Ӧ����ʹ��&m_arrFaces[i]
	return m_arrFaces[fid];
	
}

}	// end namespace

#endif // _2DOBSTRUCT_H_
