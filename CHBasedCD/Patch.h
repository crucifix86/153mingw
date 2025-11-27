/*
 * FILE: Patch.h
 *
 * DESCRIPTION: a class defines a planar patch of a convex polytope
 *				In fact,it is a polygon or a triangle.
 *
 * CREATED BY: He wenfeng, 2004/9/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_PATCH_H_
#define _PATCH_H_


#include "HalfSpace.h"
#include <AArray.h>
#include <A3DVector.h>

namespace CHBasedCD
{



class CPatch;
class CConvexPolytope;


/////////////////////////////////////////////////////////////////
//  A struct describes the vertex and its' corresponding patches
//		 of a patch in the polytopes
//	Created by: He wenfeng, 2004-9-20
/////////////////////////////////////////////////////////////////
struct VPNeighbor
{
//public members:
	int vid;						//������polytope��ȫ��id
	CPatch* pNeighborPatch;			//��Ӧ�� �ö�������һ�����㹹�ɵıߵ��ڽ�ƽ��Ƭ

	VPNeighbor() {pNeighborPatch=NULL;}
};

class CPatch : public CHalfSpace  
{
public:
	CPatch(CConvexPolytope* pCP);
	virtual ~CPatch();
	//overload
	const CPatch& operator=(const CPatch& patch);		//��д��ֵ����
	CPatch(const CPatch& patch);						//��д���ƹ��캯��

	void DecreVDegree();
	void IncreVDegree();	

	bool InNeighbors(CPatch* pPatch);

	bool Removed();							//���汻�Ӷ�������ɾ��
	void UpdateRemovedError();				//���¸�ƽ����Ƭ��ɾ����
	void Neighbor(CPatch* pPatch);			//�ж�pPatch�Ƿ��ǵ�ǰpatch���ڽ�patch������ǣ����뵽��Ӧ��λ��
	

//set && get operations
	//��ö���ĸ���
	int GetVNum() { return m_arrNeighbors.GetSize();}		//��ö��������
	//��õ�vid������
	A3DVECTOR3 GetVertex(int vid);

	int GetNeighborCount() { return m_arrNeighbors.GetSize();}
	//����һ��������ʽ����
	AArray<VPNeighbor,VPNeighbor>& GetNeighbors(){	return m_arrNeighbors; }
	float GetRemovedError() { return m_fRemovedError;}

	void GetEdge(int id,A3DVECTOR3& v1,A3DVECTOR3& v2);
	
	int GetVID(int i) { return m_arrNeighbors[i].vid; }
	
protected:
	// �ж�PatchesIntersectV���Ƿ��Ѿ����ཻ��һ���������Ƭ��������PIntersectV��
	int HasPIntersectVExist(AArray<int,int>* pPIntersectV,const AArray<AArray<int,int>*,AArray<int,int>*>& PatchesIntersectV);
	bool IsVAdjacent(AArray<int,int>* pArr1, AArray<int,int>* pArr2);						
	bool InArray(int pid, AArray<int,int>* pArr);		//pid�Ƿ�����ڶ�̬����pArr��
	bool VInPatch(int vid);								//�ж϶���vid�Ƿ�Ϊ��ǰ��Ƭ��һ������

	bool Processed(int n[3], AArray<AArray<int,int>*,AArray<int,int>*>& CoSolutionList);
	bool Solve3NPIntersection(int n[3], A3DVECTOR3& vIntersection);
	
	//���ص�i�����򶥵����һ�������ȫ��id���Ӷ��뵱ǰ����㹹��һ���ڱߣ�
	int GetNextV(int i) { return ((i+1<m_arrNeighbors.GetSize())?m_arrNeighbors[i+1].vid:m_arrNeighbors[0].vid);}
	int GetNext(int i) {return ((i+1<m_arrNeighbors.GetSize())?i+1:0);}
	int GetPre(int i) {return (i-1<0?m_arrNeighbors.GetSize()-1:i-1);}


//Attributes
private:
	float m_fRemovedError;								//�Ƴ������������Ƭ�Ӷ��������Ƴ�������������
	AArray<VPNeighbor,VPNeighbor> m_arrNeighbors;		//���㼰���ڵ���Ƭ
	CConvexPolytope* m_pConvexPolytope;					//����Ƭ�����Ķ������ָ�룻
};

}	// end namespace

#endif // _PATCH_H_
