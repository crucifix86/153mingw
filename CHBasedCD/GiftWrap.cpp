      // GiftWrap.cpp: implementation of the CGiftWrap class.
//
//////////////////////////////////////////////////////////////////////

#include "GiftWrap.h"
#include "HalfSpace.h"

//#include <fstream>


#define ABS(x) (((x)<0)?(-(x)):(x))

#define CH_FLT_EPSILON 2.0e-7F			// ����ֵ��С�ĸ�����
#define CH_VALID_TOLERANCE  0.03f

#define CH_IDENTICAL_POS_ERROR	0.01f	// if two position's x/y/z components are different under this error, we regard them as the same one!

namespace CHBasedCD
{

int RemoveCloseVertices(A3DVECTOR3* Vertices, int iVertexNum, float fDistThresh)
{

	if(!Vertices || iVertexNum < 2) return iVertexNum;


	int iCurUniqueVertex = 1;
	
	for(int i=1; i<iVertexNum; i++)
	{
		bool bSameVert = false;

		for(int j =0; j<iCurUniqueVertex; j++)
		{
			if((Vertices[j]-Vertices[i]).Magnitude() < fDistThresh)
			{
				// yes, i is same with j...
				bSameVert = true;
				break;
			}
		}

		if(!bSameVert)
		{
			Vertices[iCurUniqueVertex++] = Vertices[i];
		}
		

	}


	return iCurUniqueVertex;
}	


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGiftWrap::CGiftWrap()
{
	m_fInitHSDistThresh=0.01f;			
	m_nHullType=HULL_3D;				//ȱʡ�ģ�͹��Ϊ3D��
}

CGiftWrap::~CGiftWrap()
{

}

CGiftWrap::CGiftWrap(A3DVECTOR3* pVertexes,int vNum)
{
	//���ö�����Ϣ
	SetVertexes(pVertexes,vNum);

	m_bExceptionOccur=false;
	
	m_fInitHSDistThresh=0.01f;
	m_nHullType=HULL_3D;				//ȱʡ�ģ�͹��Ϊ3D��
}

void CGiftWrap::ComputeConvexHull()
{
	for (m_fIdenticalPosErr = CH_IDENTICAL_POS_ERROR; m_fIdenticalPosErr >= 1e-5f; m_fIdenticalPosErr *= 0.1f)
	{
		InternalComputeConvexHull();
		if (!ExceptionOccur())
			break;
	}
}

void CGiftWrap::InternalComputeConvexHull()
{
	//------------------------------------------------------------------------
	// Revised by wenfeng, Aug.17, 2011
	// review convex-hull generation algorithm after 7 years
	// the major challenge lies in co-planar vertices, try to handle this case.

	CHalfSpace hsFitPlane;
	A3DVECTOR3 vFitNormal(0.0f);
	bool bFit = BestFitPlane(m_pVertexes, m_nVNum, hsFitPlane, m_fIdenticalPosErr);
	if(bFit)
	{
		// tune the vertices's position to make the convex-hull generation more accurately...
		for(int i=0; i<m_nVNum; i++)
		{
			hsFitPlane.ProjectPos2Plane(m_pVertexes[i]);
		}
	}

	vFitNormal = hsFitPlane.GetNormal();
	if(vFitNormal.IsZero())
		vFitNormal.Set(0.0f, 1.0f, 0.0f);

	//////////////////////////////////////////////////////////////////////////
	// Noted by wenfeng, 05-01-31
	// �������͹���Ĺ�����һ���̶���������CHalfSpace��ľ�����ֵ�����
	// ���ｫ�跨�ҵ�һ�����Եõ���ȷ����ľ�����ֵ��
	// һ����Ҫ���������ܷ�����͹�������ֵ�����ǵ����Ĺ�ϵ������������ֵԽ���ԽС
	// ��һ�����Եõ������ʵ���ϣ���ֵ��һ��΢С���Ŷ��������ܵ����쳣���ֻ����͹��
	// ��Ŀǰ��û���ҳ�����֮��ı�Ȼ��ϵ�͹��ɡ�
	// ��ˣ�Ŀǰ����ֵ�趨�������˿������ȵĳ��Է�����˵�����£�
	// ���ȳ���һ������ֵhsDistThreshBase��Ȼ����ֵ��ΪhsDistThreshBase��0.1��
	// ֱ����ֵ<CH_FLT_EPSILON��
	// ����������е���ֵ�Բ�������Ҫ�����hsDistThreshBase-0.1*hsDistThreshBase��
	// Ȼ����������ѭ����
	// �ɿ�������ķ�������������ķ���
	//////////////////////////////////////////////////////////////////////////
	

	float hsDistThreshBase=m_fInitHSDistThresh*10.0f;	//ÿ�εݼ�ѭ���Ļ������ȷŴ�10��
	float hsDistThreshBaseStep=m_fInitHSDistThresh;		//�ݼ�����
	float hsDistThresh;			//��ǰ����ֵ
	
	//��֤û���쳣�����͹����
	//˫��ѭ��
	do{
	
		hsDistThresh=hsDistThreshBase;
		do{
			
			m_nHullType=HULL_3D;						//���¼���ʱ����ʼ��Ϊ3D͹��

			hsDistThresh*=0.1f;								//��С��ֵ10��

			CHalfSpace::SetDistThresh(hsDistThresh);	//������ֵ
			
			//�ȵ��ø��ຯ���Ӷ������ղ���
			CConvexHullAlgorithm::ComputeConvexHull();
			ResetSameVertices();

			Edge e=SearchFirstEdge(vFitNormal);

			//by yx, August 29, 2011
			if (!ValidateCHPlane(CHalfSpace(m_pVertexes[e.start], m_pVertexes[e.end], m_pVertexes[e.start] + vFitNormal)))
			{
				A3DVECTOR3 vAxis[3] = { A3DVECTOR3(1, 0, 0), A3DVECTOR3(0, 1, 0), A3DVECTOR3(0, 0, 1) };
				for (int i = 0; i < 3; i++)
				{
					if (fabsf(DotProduct(vFitNormal, vAxis[i])) > 0.9f)
						continue;
					
					A3DVECTOR3 vN = CrossProduct(vFitNormal, vAxis[i]);
					Edge e1 = SearchFirstEdge(vN);
					if (ValidateCHPlane(CHalfSpace(m_pVertexes[e1.start], m_pVertexes[e1.end], m_pVertexes[e1.start] + vN)))
					{
						e = e1;
						break;
					}
				}
			}

			m_eFirst=e;
			m_EdgeStack.Push(e);
			m_EdgeStack.Push(e);			//ע�⣬����Ӧ��push���Σ��Ӷ���֤��while�����еĵ���������һ���߱�������ջ�У�
			
			while(!m_EdgeStack.IsEmpty() && !m_bExceptionOccur && m_nHullType==HULL_3D)
			{
				e=m_EdgeStack.Pop();
				DealE(e);
			}
			
		}while( ExceptionOccur()  && hsDistThresh>CH_FLT_EPSILON);
		
		hsDistThreshBase-=hsDistThreshBaseStep;		//���ѭ���ٽ���һ���Ŷ�

	}while( ExceptionOccur() && hsDistThreshBase>0.0f);

	if(hsDistThreshBase<=0.0f)
	{
		//˵���Ѿ���������ֵ��Χ����������쳣����
		//��˵��ȷʵ��ģ�͵�͹�Ǽ�������쳣��
		m_bExceptionOccur=true;
	}

	//hsDistThresh*=0.01f;	
	//CHalfSpace::SetDistThresh(hsDistThresh);  //�ָ���ȱʡ����ֵ
	
	CHalfSpace::SetDistThresh();  //�ָ���ȱʡ����ֵ
}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// ��SearchV()�����ĸĽ���ʹ����Դ�����㹲��������
/////////////////////////////////////////////////////////
bool CGiftWrap::DealE(Edge e)
{
	int i;
	for(i=0;i==(int)e.start || i==(int)e.end || IsFaceInCH(Face(e.start,e.end,i)) || m_pbVInvalid[i];i++);

	//����e�͸õ���ɵİ�ռ䣬��ʱ��hs�ض���Ч����Ϊ����֤�˲���������
	CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[i]);

	AArray<int,int>* pCoPlanarVertexes=new AArray<int,int>;	//����һ�����й������б�
	AArray<int,int>& CoPlanarVertexes=*pCoPlanarVertexes;
	CoPlanarVertexes.Add(i);			//���ӵ�ǰ��һ��Ԫ��
	//��ʼ��ѭ����ע��ѭ����ʼ����������һ��Ԫ�ؿ�ʼ��
	for(i++;i<m_nVNum;i++)
	{
		//����e�˵㣬û�п�������Ҳ��ڰ�ռ��ڲ���
		if(i!=(int)e.start && i!=(int)e.end && !m_pbVInvalid[i] && !IsFaceInCH(Face(e.start,e.end,i)) && ! hs.Inside(m_pVertexes[i]))	
		{
			if(hs.OnPlane(m_pVertexes[i]))
			{
				//�ڰ�ռ�ı߽��ϣ������ӵ�������б�
				CoPlanarVertexes.Add(i);			//���ӵ�ǰ��
				
			}
			else
			{

				//˵���õ㲻���ڲ�����õ�Ϊ�µ�Ŀ��㣬���ع���ռ�
				hs.Set(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[i]);

				//ͬʱ��չ����б��������õ������������б�
				CoPlanarVertexes.RemoveAll();
				CoPlanarVertexes.Add(i);			//���ӵ�ǰ��

				//ע�⣬����͹�ǵ����ʣ����ﲻ���ͷ��ʼ����

			}
		}

	}
	
	//���������ѭ����CoPlanarVertexes�оʹ洢���ɵ�ǰ��e�ҵ���һ��͹���ϵ����е�

	//�������ж��Ƿ���2D��͹��
	m_nHullType=HULL_2D;
	for(i=0;i<m_nVNum;i++)			//�������ж���
	{
		if(i!=(int)e.start && i!=(int)e.end && !m_pbVInvalid[i])	//��e�Ķ˵㣬������Ч
		{
			if(!IsVInVSets(i,pCoPlanarVertexes))	// �����ǰ���㲻�ٹ��漯�У�
			{
				// Added by wenfeng, 05-5-9
				// ����һ���жϣ��Ӷ�ʹ�ý��������
				CHalfSpace hs(m_pVertexes[e.start], m_pVertexes[e.end], m_pVertexes[CoPlanarVertexes[0]]);
				if(hs.OnPlane(m_pVertexes[i]))
					CoPlanarVertexes.Add(i);
				else
				{
					m_nHullType=HULL_3D;
					break;
				}
			}
		}
	}


	//���濪ʼ���д��������ｫԭ����ComputeConvexHull����ѭ���еĴ����������
	if(CoPlanarVertexes.GetSize()==1)		
	{
		//��򵥵������û�й����
		
		int v3=CoPlanarVertexes[0];
		//����������ѹջ
		Edge e1(e.start,v3),e2(v3,e.end);
		SelectivePushStack(e1);	//m_EdgeStack.CheckPush(e1);
		SelectivePushStack(e2);	//m_EdgeStack.CheckPush(e2);

		//����������Ƭ�������������
		m_Faces.Add(Face(e.start,e.end,v3));
		//ͬʱ����������Ƭ����������Ϊ͹�ǵ�ExtremeVertex!
		m_pbExtremeVertex[e.start]=true;
		m_pbExtremeVertex[e.end]=true;
		m_pbExtremeVertex[v3]=true;

		if(m_nHullType==HULL_2D)			// ͹��Ϊһ�������ε����
		{
			CoPlanarVertexes.Add(e.start);
			CoPlanarVertexes.Add(e.end);
			m_Planes.Add(pCoPlanarVertexes);
		}
		else
			delete pCoPlanarVertexes;				//û�ж�㹲��

	}
	else		//��㹲��������
	{
		//����e.start,e.end��CoPlanarVertexes[0]��ɵ�ƽ������͹��
		DealCoPlanarV(e,CoPlanarVertexes);

	}


	return true;
}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// �ú��������������ڱ�e���ҵ���Ŀ����Ƕ�㹲������
// ע�⣬�����е�CoPlanarVertexes�����ĵ���Ӧ>1
/////////////////////////////////////////////////////////
void CGiftWrap::DealCoPlanarV(Edge e, AArray<int,int> & CoPlanarVertexes)
{
	/*
	//�÷�����ʱ����ʹ�ã�wenfeng 04-12-02
	
	 //Revised by He wenfeng, 2004-11-29
	//��������������㣺��������v1,v2,v3��Ķ���λ�ý���������
	//ʹ����ͶӰ��v1,v2,v3���ɵ�ƽ���ϣ�
	
	CHalfSpace Plane(m_pVertexes[e.start],m_pVertexes[e.end],m_pVertexes[CoPlanarVertexes[0]]);
	for(int j=1;j<CoPlanarVertexes.GetSize();j++)
	{
		A3DVECTOR3 vDiff=m_pVertexes[CoPlanarVertexes[j]]-m_pVertexes[e.start];
		vDiff=DotProduct(vDiff,Plane.GetNormal())*Plane.GetNormal();
		m_pVertexes[CoPlanarVertexes[j]]-=vDiff;
	}
	//*/
	
	AArray<int,int> *parrCoPlanarVertexes=&CoPlanarVertexes;

	//������ָñߵ�������Ч�������ֱ�ӷ���
	if(m_pbVInvalid[e.start]||m_pbVInvalid[e.end])
	{
		//�쳣�˳�ʱ��һ��Ҫ�ͷ��ڴ�
		delete parrCoPlanarVertexes;
		return;
	}

	//�����ʼ��halfspace
	//ע�⹹��ķ�������halfspace�ķָ����e.end��CoPlanarVertexes[0]��ͬʱƽ���ڷ���
	CHalfSpace hs;
	
	AArray<int,int> ExtremeVertexes;				//ƽ�������е��͹�ǵ㼯�������洢���

	AArray<int,int> CoLinearVertexes;				//���ߵ��������һ���㼯��ʾ
	//CoLinearVertexes.Add(CoPlanarVertexes[0]);

	//��e.start��e.endҲ����CoPlanarVertexes��
	CoPlanarVertexes.Add(e.start);
	CoPlanarVertexes.Add(e.end);
	


	
	//���µ��㷨�У�ÿ����e�Ѿ������쵽�����������ˣ����ÿ���
	//���ܽ�������������
	//���޸���2004-8-25

	int vidNotCL=CoPlanarVertexes[0];		
	//���������������CoPlanarVertexes[0]һ�������e���ߵ�
	//����vidNotCL���������ƽ��ķ���
	A3DVECTOR3 normal=CrossProduct(m_pVertexes[e.end]-m_pVertexes[e.start],m_pVertexes[vidNotCL]-m_pVertexes[e.start]);
	normal.Normalize();

	Edge curE(e.end,vidNotCL);						//��ǰ�ı�

	//΢����04-10-19����
	//��while���ܻ������ѭ������˱�������ж���������
	int vSize=CoPlanarVertexes.GetSize();

	//main loop
	while(curE.start!=e.start)				//������ҵ�e.start��һ��ʱ�������ҵ������е㴦��͹���ϵĵ�
	{
		//��ʼ��hs
		hs.Set(m_pVertexes[curE.start],m_pVertexes[curE.end],m_pVertexes[curE.start]+normal);
		
		CoLinearVertexes.RemoveAll();
		CoLinearVertexes.Add(curE.end);		//��ǰ�����������ߵ㼯

		//Ѱ�����㵱ǰcurE��Ŀ���
		int i;
		for(i=0;i<CoPlanarVertexes.GetSize();i++)
		{
			int id=CoPlanarVertexes[i];
			if(id!=(int)curE.start &&
				id!=(int)curE.end &&
				!m_pbVInvalid[id] &&			//����Ϊ��Ч�㣡
				!hs.Inside(m_pVertexes[id]))
			{
				//�ж��Ƿ��棬ʵ����Ϊ�Ƿ���
				if(hs.OnPlane(m_pVertexes[CoPlanarVertexes[i]]))
				{
					//������e���ߵ��������������
					//if(CoPlanarVertexes[i]!=e.start && CoPlanarVertexes[i]!=e.end)
						//&& DotProduct(m_pVertexes[curE.end]-m_pVertexes[curE.start],m_pVertexes[CoPlanarVertexes[i]]-))
					CoLinearVertexes.Add(CoPlanarVertexes[i]);
				}
				else
				{
					//���ĵ�ǰ��
					curE.end=CoPlanarVertexes[i];
					//���¼���halfspace
					hs.Set(m_pVertexes[curE.start],m_pVertexes[curE.end],m_pVertexes[curE.start]+normal);

					//��գ������¼��㹲�ߵ㼯
					CoLinearVertexes.RemoveAll();
					CoLinearVertexes.Add(CoPlanarVertexes[i]);

				}
			}
		}
		//ѭ����ɣ���ʱӦ���Ѿ��ҵ�һ�����һ��㼯
		if(CoLinearVertexes.GetSize()==1)
		{
			//һ��������
			//������͹�Ǳ߽�㼯
			ExtremeVertexes.Add(CoLinearVertexes[0]);
			//�ҵ��ĵ��Ϊ��ǰ�ߵ����
			curE.start=CoLinearVertexes[0];

			//��һ������ΪcurE.end
			int i;
			for(i=0;i<CoPlanarVertexes.GetSize() && CoPlanarVertexes[i]==(int)curE.start;i++);	//���ܵ���start!
			curE.end=CoPlanarVertexes[i];

			vSize--;
			if(vSize<0)
			{
				this->m_bExceptionOccur=true;		//�����쳣�ˣ��˳���
				
				//�쳣�˳�ʱ��һ��Ҫ�ͷ��ڴ�
				delete parrCoPlanarVertexes;

				return;
			}

		}
		else
		{
			//04-10-30
			//����ֻ����Զ�㣬��������û��Ҫ��һ�����������
			
			//����㼯�����
			//���㵽curE.startŷ�Ͼ����Զ�����򣬴ӽ���Զ
			SortByDist(curE.start,CoLinearVertexes);



			//�������Ϊ��Զ��
			curE.start=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
			//������Զ�㣡
			ExtremeVertexes.Add(curE.start);

			//��һ������ΪcurE.end
			for(i=0;i<CoPlanarVertexes.GetSize() && (CoPlanarVertexes[i]==(int)curE.start || m_pbVInvalid[CoPlanarVertexes[i]]);i++);	//���ܵ���start!
			assert(i<CoPlanarVertexes.GetSize());
			curE.end=CoPlanarVertexes[i];			

			vSize--;
			if(vSize<0)
			{
				this->m_bExceptionOccur=true;		//�����쳣�ˣ��˳���
				//�쳣�˳�ʱ��һ��Ҫ�ͷ��ڴ�
				delete parrCoPlanarVertexes;

				return;
			}

		}

	} // end of main loop

	//ע�⣬����ǰ���ѭ��ExtremeVertexes�����һ��Ԫ��Ӧ�þ���e.start
	//assert(ExtremeVertexes.GetSize()>1);
	if(ExtremeVertexes.GetSize()<=1)
	{
		m_bExceptionOccur=true;		//˵���쳣�������˳���
		//�쳣�˳�ʱ��һ��Ҫ�ͷ��ڴ�
		delete parrCoPlanarVertexes;
		return;
	}

	//��ʱ���Ѿ��õ���һ�鰴˳��Ķ��㣬������߶�ջ�����б���������
	Edge ep(ExtremeVertexes[0],e.end);
	SelectivePushStack(ep);	//m_EdgeStack.CheckPush(ep);
	m_Faces.Add(Face(e.start,e.end,ExtremeVertexes[0],true)); //���潫���ʷ�
	
	int i;
	for(i=0;i<ExtremeVertexes.GetSize()-2;i++)
	{
		//���ӱߵ���ջ��һ��Ҫע��˳��
		ep.Set(ExtremeVertexes[i+1],ExtremeVertexes[i]);		//Ӧ�÷�������
		SelectivePushStack(ep);	//m_EdgeStack.CheckPush(ep);
		m_Faces.Add(Face(e.start,ExtremeVertexes[i],ExtremeVertexes[i+1],true));
		
	}
	//���һ����
	//ע�⣺�ԸñߵĴ���Ӧ��ֱ��ʹ��m_EdgeStack.CheckPush
	//��Ϊ����ǰ�Ѿ������˰����ñߵ������棬����
	//���ʹ��SelectivePushStack�������Ϊ�ñ��Ѿ���
	//���ӵ��������б��У��Ӷ�©���������ߣ�
	//�����ᵼ������͹��ȱ����һ���棡
	ep.Set(ExtremeVertexes[i+1],ExtremeVertexes[i]);
	m_EdgeStack.CheckPush(ep);

	//�����й���㶼��Ϊ��Ч
	for(i=0;i<CoPlanarVertexes.GetSize();i++)
		m_pbVInvalid[CoPlanarVertexes[i]]=true;

	//��������CoPlanarVertexes��ʹ��������߽��
	CoPlanarVertexes.RemoveAll();	
	//��e.end���뵽ExtremeVertexes��
	ExtremeVertexes.Add(e.end);
	//Ȼ�����б߽����Ϊ��Ч
	for(i=0;i<ExtremeVertexes.GetSize();i++)
	{
		m_pbVInvalid[ExtremeVertexes[i]]=false;
		m_pbExtremeVertex[ExtremeVertexes[i]]=true;
		CoPlanarVertexes.Add(ExtremeVertexes[i]);
	}
	
	//��ʱ�ٽ�CoPlanarVertexes���뵽m_Planes��
	//����������е㹹�ɵ�ƽ��������m_Planes
	m_Planes.Add(&CoPlanarVertexes);


}

/////////////////////////////////////////////////////////
// added by wenfeng,2004.8.20
// ���յ�v����Զ������vList�еĵ㣬��������
/////////////////////////////////////////////////////////
void CGiftWrap::SortByDist(int v, AArray<int,int> & vList)
{
	if(vList.GetSize()<2) return;

	//�ȼ������
	float* pDist=new float[vList.GetSize()];
	int i;
	for(i=0;i<vList.GetSize();i++)
		pDist[i]=(m_pVertexes[vList[i]]-m_pVertexes[v]).SquaredMagnitude();

	int minDistID;
	int tmp;			//��������
	//���ݾ�������
	for(i=0;i<vList.GetSize();i++)
	{
		minDistID=i;
		for(int j=i+1;j<vList.GetSize();j++)
			if(pDist[j]<pDist[minDistID])
				minDistID=j;
		//��ѭ��ɨ��һ�飬�ҵ���i��GetSize()�о�����С�ĵ�

		//��ʼ����
		tmp=vList[i];
		vList[i]=vList[minDistID];
		vList[minDistID]=tmp;
		
		//ͬʱ����������
		pDist[minDistID]=pDist[i];

	}

	delete [] pDist;
}

//�жϿռ��������Ƿ���
bool CGiftWrap::IsTriVsCoLinear(int v1, int v2, int v3)
{
	A3DVECTOR3 vd1=m_pVertexes[v2]-m_pVertexes[v1],vd2=m_pVertexes[v3]-m_pVertexes[v1];
	vd1.Normalize();
	vd2.Normalize();

	if(1- ABS(DotProduct(vd1,vd2)) < 0.00006f)
		return true;
	else return false;

	
}

//�жϱ�e�͵�v֮��Ĺ�ϵ
//����ֵ������£�
//0:v��e�⣡
//1:v��ֱ��e�ϣ������߶�e��࣬��e.start��
//2:v��ֱ��e�ϣ������߶�e�Ҳ࣬��e.end��
//3:v���߶�e��
//-1:v��e.start�غ�
//-2:v��e.end�غ�
int CGiftWrap::EVRelation(const Edge& e, int v)
{
	if(v==(int)e.start) return -1;
	if(v==(int)e.end) return -2;

	if(!IsTriVsCoLinear(e.start,e.end,v)) return 0;

	A3DVECTOR3 de=m_pVertexes[e.end]-m_pVertexes[e.start];		//�߶εķ���
	A3DVECTOR3 vd1=m_pVertexes[v]-m_pVertexes[e.start];
	A3DVECTOR3 vd2=m_pVertexes[v]-m_pVertexes[e.end];

	if(DotProduct(vd1,vd2)<0.0f) return 3;		//���߶�e��

	if(DotProduct(vd1,de)>0.0f) return 2;

	return 1;

}

//Ѱ�ҵ�һ���ߣ�
Edge CGiftWrap::SearchFirstEdge(const A3DVECTOR3& vRefNormal)
{
	Edge e;
	//��Ѱ��yȡֵ��С��һ���
	AArray<int,int> LowestVertexes;
	float yMin=m_pVertexes[0].y;
	LowestVertexes.Add(0);
	int i;
	for(i=1;i<m_nVNum;i++)
	{
		if(yMin-m_pVertexes[i].y>m_fIdenticalPosErr)	// ˵����yMin��С
		{
			yMin=m_pVertexes[i].y;
			LowestVertexes.RemoveAll();
			LowestVertexes.Add(i);
		}
		else if( ABS(yMin-m_pVertexes[i].y)<m_fIdenticalPosErr)
		{
			//����
			LowestVertexes.Add(i);
		}

	}

	//ѭ����ɺ�LowestVertexes��������yֵ��С��һ��㼯
	if(LowestVertexes.GetSize()==1) //����һ����
	{
		e.start=LowestVertexes[0];

		//Ѱ�ҵ�2���㣬����Ϊ��XOYƽ����Ѱ�ң�
		for(i=0;i==(int)e.start;i++);
		
		// ����vRefNormal�����ƽ��...
		CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[i], m_pVertexes[e.start] + vRefNormal);

		AArray<int,int> CoPlanarVertexes;
		CoPlanarVertexes.Add(i);			//���ӵ�ǰ��һ��Ԫ��		
		for(i++;i<m_nVNum;i++)
			if(i!=(int)e.start && !hs.Inside(m_pVertexes[i]))
			{
                if(hs.OnPlane(m_pVertexes[i]))
					CoPlanarVertexes.Add(i);			//���ӵ�ǰ��
				else
				{
					//˵���õ㲻���ڲ�����õ�Ϊ�µ�Ŀ��㣬���ع���ռ�
					hs.Set(m_pVertexes[e.start],m_pVertexes[i], m_pVertexes[e.start] + vRefNormal);

					//ͬʱ��չ����б��������õ������������б�
					CoPlanarVertexes.RemoveAll();
					CoPlanarVertexes.Add(i);			//���ӵ�ǰ��

				}
            }
		
		if(CoPlanarVertexes.GetSize()==1)
		{
			//��һ���㣬��õ㼴Ϊe.end
			e.end=CoPlanarVertexes[0];
			return e;
		}
		else
		{
			//�ж���һ���㹲������
			A3DVECTOR3 vd3=hs.GetNormal();
			//�����ʼ��halfspace
			hs.Set(m_pVertexes[e.start],m_pVertexes[CoPlanarVertexes[0]],m_pVertexes[e.start]+vd3);
			AArray<int,int> CoLinearVertexes;
			CoLinearVertexes.Add(CoPlanarVertexes[0]);
			for(i=1;i<CoPlanarVertexes.GetSize();i++)
				if(!hs.Inside(m_pVertexes[CoPlanarVertexes[i]]))
				{
                    if(hs.OnPlane(m_pVertexes[CoPlanarVertexes[i]]))
						CoLinearVertexes.Add(CoPlanarVertexes[i]);
					else
					{

						//���¼���halfspace
						hs.Set(m_pVertexes[e.start],m_pVertexes[CoPlanarVertexes[i]],m_pVertexes[e.start]+vd3);

						//��գ������¼��㹲�ߵ㼯
						CoLinearVertexes.RemoveAll();
						CoLinearVertexes.Add(CoPlanarVertexes[i]);
					}
                }
			//ѭ������
			if(CoLinearVertexes.GetSize()==1)
			{
				//��һ�����ߵ㣬��õ㼴Ϊ��һ�˵�
				e.end = CoLinearVertexes[0];
				return e;
			}
			else
			{
				//�Թ��ߵ㼯���о��������
				SortByDist(e.start,CoLinearVertexes);

				//����Զ��Ϊe����һ���˵㣬����������Ϊ��Ч
				for(int i=0;i<CoLinearVertexes.GetSize()-1;i++)
					m_pbVInvalid[CoLinearVertexes[i]]=true;

				e.end=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
				return e;
			}
		}


	}
	else
	{
		//yȡ����Сֵ����һ����������
		//��ֻ����XOZƽ���Ͽ��Ǽ���
		if(LowestVertexes.GetSize()==2)		//����������㣬��ֱ�ӷ���
		{
			e.start=LowestVertexes[0];
			e.end=LowestVertexes[1];
			return e;
		}
		//��zֵȡ��С�ĵ��㼯
		AArray<int,int> ZMinVertexes;
		float ZMin=m_pVertexes[LowestVertexes[0]].z;
		ZMinVertexes.Add(LowestVertexes[0]);
		for(i=1;i<LowestVertexes.GetSize();i++)
		{
			if(ZMin-m_pVertexes[LowestVertexes[i]].z>m_fIdenticalPosErr)	// ˵����yMin��С
			{
				ZMin=m_pVertexes[LowestVertexes[i]].z;
				ZMinVertexes.RemoveAll();
				ZMinVertexes.Add(LowestVertexes[i]);
			}
			else if( ABS(ZMin-m_pVertexes[LowestVertexes[i]].z)<m_fIdenticalPosErr)
			{
				//����
				ZMinVertexes.Add(LowestVertexes[i]);
			}

		}
		
		if(ZMinVertexes.GetSize()==1)		
		{
			//��õ�ͱ���Ϊ��һ��
			e.start=ZMinVertexes[0];
			AArray<int,int> CoLinearVertexes;
			
			//��LowestVertexes�п�ʼѰ����һ��
			for(i=0;LowestVertexes[i]==(int)e.start;i++);
			CoLinearVertexes.Add(LowestVertexes[i]);
			A3DVECTOR3 vd3=A3DVECTOR3(0.0f,-1.0f,0.0f);
			CHalfSpace hs(m_pVertexes[e.start],m_pVertexes[LowestVertexes[i]],m_pVertexes[e.start]+vd3);

			for(i++;i<LowestVertexes.GetSize();i++)
				if(LowestVertexes[i]!=(int)e.start && !hs.Inside(m_pVertexes[LowestVertexes[i]]))
				{
                    if(hs.OnPlane(m_pVertexes[LowestVertexes[i]]))
						CoLinearVertexes.Add(LowestVertexes[i]);
					else
					{

						//���¼���halfspace
						hs.Set(m_pVertexes[e.start],m_pVertexes[LowestVertexes[i]],m_pVertexes[e.start]+vd3);

						//��գ������¼��㹲�ߵ㼯
						CoLinearVertexes.RemoveAll();
						CoLinearVertexes.Add(LowestVertexes[i]);
					}
                }
			
			//ѭ������
			if(CoLinearVertexes.GetSize()==1)
			{
				//��һ�����ߵ㣬��õ㼴Ϊ��һ�˵�
				e.end = CoLinearVertexes[0];
				return e;
			}
			else
			{
				//�Թ��ߵ㼯���о��������
				SortByDist(e.start,CoLinearVertexes);

				//����Զ��Ϊe����һ���˵㣬����������Ϊ��Ч
				for(i=0;i<CoLinearVertexes.GetSize()-1;i++)
					m_pbVInvalid[CoLinearVertexes[i]]=true;

				e.end=CoLinearVertexes[CoLinearVertexes.GetSize()-1];
				return e;
			}			
		}
		else
		{
			//�ж���㶼ȡ����z��С
			//�ҳ�x��С���������
			int xminID=ZMinVertexes[0],xmaxID=ZMinVertexes[0];
			float XMin=m_pVertexes[ZMinVertexes[0]].x;
			float XMax=m_pVertexes[ZMinVertexes[0]].x;
			for(i=1;i<ZMinVertexes.GetSize();i++)
			{
				if(XMin-m_pVertexes[ZMinVertexes[i]].x>m_fIdenticalPosErr)	// ˵����xMin��С
				{
					XMin=m_pVertexes[ZMinVertexes[i]].x;
					xminID=ZMinVertexes[i];
				}
				if(m_pVertexes[ZMinVertexes[i]].x-XMax>m_fIdenticalPosErr)	// ˵����xMin��С
				{
					XMax=m_pVertexes[ZMinVertexes[i]].x;
					xmaxID=ZMinVertexes[i];
				}
			}

			//��ZMinVertexes�����е�Ϊ��Ч��
			for(i=0;i<ZMinVertexes.GetSize();i++)
				m_pbVInvalid[ZMinVertexes[i]]=true;
			
			m_pbVInvalid[xmaxID]=false;
			m_pbVInvalid[xminID]=false;
			m_pbExtremeVertex[xmaxID]=true;
			m_pbExtremeVertex[xminID]=true;

			e.start=xminID;
			e.end=xmaxID;
			return e;
		}

	}

}


// some interface for outer step-by-step call
bool CGiftWrap::IsOver() const
{
	return m_EdgeStack.IsEmpty();
}



void CGiftWrap::GoOneStep()
{
	Edge e;
	e=m_EdgeStack.Pop();
	DealE(e);	
}

void CGiftWrap::Start()
{
		//�ȵ��ø��ຯ���Ӷ������ղ���
	CConvexHullAlgorithm::ComputeConvexHull();
	Edge e=SearchFirstEdge();
	m_EdgeStack.Push(e);
	m_EdgeStack.Push(e);			//ע�⣬����Ӧ��push���Σ��Ӷ���֤��while�����еĵ���������һ���߱�������ջ�У�

	m_eFirst=e;
}

void CGiftWrap::Reset()
{
	CConvexHullAlgorithm::Reset();
	m_EdgeStack.Clear();
	m_bExceptionOccur=false;
}

void CGiftWrap::ResetSameVertices()
{
	CConvexHullAlgorithm::Reset();
	m_EdgeStack.Clear();
	m_bExceptionOccur=false;

	//��ʼ��͹�Ǳ߽�㼯״̬
	m_pbVInvalid=new bool[m_nVNum];
	m_pbExtremeVertex=new bool[m_nVNum];

	for(int i=0;i<m_nVNum;i++)
	{
		m_pbVInvalid[i]=false;
		m_pbExtremeVertex[i]=false;
	}

	//*m_pnIDsNum=0;		//��ͷ��ʼ������
}

//ѡ����ѹջ
//�����e�������Ƿ��Ѿ����������ˣ���
//�Ƿ��Ѿ�������ѹ��ջ����������
//�����������������ѹջ��
//���򣬽�����������ѭ����
void CGiftWrap::SelectivePushStack(const Edge& e)
{
	if(m_EdgeStack.CheckPush(e))
	{
		//ѹջ�ɹ�������ѹջǰջ��û��e����
		//��ʱӦ�жϸñ��Ƿ��Ѿ���������
		if(IsEdgeInCH(e))
		{
			//��������£����㷨�������һ��������3��
			//�����ʵ�����ˣ�����������쳣��
			m_bExceptionOccur=true;		//˵���쳣������
			
			m_EdgeStack.Pop();		//new interface!
		}

	}
}

//�����㱣�浽�ļ�
bool CGiftWrap::SaveVerticesToFile(const char *szFileName)
{
	FILE* OutFile = fopen(szFileName, "wt");;

	if(!OutFile)
		return false;

	fprintf(OutFile, "%d\n", m_nVNum);		//��������
	for(int i=0;i<m_nVNum;i++)
		fprintf(OutFile, "%f %f %f\n", m_pVertexes[i].x, m_pVertexes[i].y, m_pVertexes[i].z);
	
	fclose(OutFile);
	return true;
}

//���ļ��ж�ȡ������Ϣ
bool LoadVerticesFromFile(const char *szFileName,int& nVNum,A3DVECTOR3* & pVertices )
{
	FILE* InFile = fopen(szFileName, "rt");;
	if(!InFile)
		return false;

	fscanf(InFile, "%d\n", nVNum);
	pVertices=new A3DVECTOR3[nVNum];
	for(int i=0;i<nVNum;i++)
		fscanf(InFile,"%f %f %f\n",&pVertices[i].x,&pVertices[i].y,&pVertices[i].z);
	
	fclose(InFile);
	return true;

}

bool CGiftWrap::ValidConvexHull()
{
	// ��鹲��Ķ����Ƿ����ֶ��㵽ƽ��ľ��볬����ֵ�����
	CHalfSpace hs;
	for(int i=0; i<m_Planes.GetSize(); i++ )
	{
		if (m_Planes[i]->GetSize() < 3)
			continue;

		hs.Set(m_pVertexes[m_Planes[i]->GetAt(0)], m_pVertexes[m_Planes[i]->GetAt(1)], m_pVertexes[m_Planes[i]->GetAt(2)]);

		for(int j=3; j<m_Planes[i]->GetSize(); j++)
		{
			if(fabs(hs.DistV2Plane(m_pVertexes[m_Planes[i]->GetAt(j)]) ) > CH_VALID_TOLERANCE )
				return false;
		}
	}

	return true;
}

//check if is validate convex hull plane, yx Aug. 29, 2011 
bool CGiftWrap::ValidateCHPlane(const CHalfSpace& hs)
{
	for (int i = 0; i < m_nVNum; i++)
	{
		if (!hs.Inside(m_pVertexes[i]) && !hs.OnPlane(m_pVertexes[i]))
			return false;
	}
	return true;
}

}	// end namespace


