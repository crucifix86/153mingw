//////////////////////////////////////////////////////////////////////
// Matrix.cpp
//
// ����������� CMatrix ��ʵ���ļ�
//
// from source code of <��ѧ�빤����ֵ�㷨 by Zhou changfa>
//////////////////////////////////////////////////////////////////////

#include "Matrix.h"
#include <assert.h>
#include <cstring>

namespace CHBasedCD
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// �������캯��
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix()
{
	m_nNumColumns = 1;
	m_nNumRows = 1;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);
}

//////////////////////////////////////////////////////////////////////
// ָ�����й��캯��
//
// ������
// 1. int nRows - ָ���ľ�������
// 2. int nCols - ָ���ľ�������
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nRows, int nCols)
{
	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);
}

//////////////////////////////////////////////////////////////////////
// ָ��ֵ���캯��
//
// ������
// 1. int nRows - ָ���ľ�������
// 2. int nCols - ָ���ľ�������
// 3. double value[] - һά���飬����ΪnRows*nCols���洢�����Ԫ�ص�ֵ
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nRows, int nCols, double value[])
{
	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);

	SetData(value);
}

//////////////////////////////////////////////////////////////////////
// �����캯��
//
// ������
// 1. int nSize - ����������
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nSize)
{
	m_nNumRows = nSize;
	m_nNumColumns = nSize;
	m_pData = NULL;
	bool bSuccess = Init(nSize, nSize);
	assert (bSuccess);
}

//////////////////////////////////////////////////////////////////////
// �����캯��
//
// ������
// 1. int nSize - ����������
// 2. double value[] - һά���飬����ΪnRows*nRows���洢�����Ԫ�ص�ֵ
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(int nSize, double value[])
{
	m_nNumRows = nSize;
	m_nNumColumns = nSize;
	m_pData = NULL;
	bool bSuccess = Init(nSize, nSize);
	assert (bSuccess);

	SetData(value);
}

//////////////////////////////////////////////////////////////////////
// �������캯��
//
// ������
// 1. const CMatrix& other - Դ����
//////////////////////////////////////////////////////////////////////
CMatrix::CMatrix(const CMatrix& other)
{
	m_nNumColumns = other.GetNumColumns();
	m_nNumRows = other.GetNumRows();
	m_pData = NULL;
	bool bSuccess = Init(m_nNumRows, m_nNumColumns);
	assert(bSuccess);

	// copy the pointer
	memcpy(m_pData, other.m_pData, sizeof(double)*m_nNumColumns*m_nNumRows);
}

//////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////
CMatrix::~CMatrix()
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// ��ʼ������
//
// ������
// 1. int nRows - ָ���ľ�������
// 2. int nCols - ָ���ľ�������
//
// ����ֵ��bool �ͣ���ʼ���Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CMatrix::Init(int nRows, int nCols)
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}

	m_nNumRows = nRows;
	m_nNumColumns = nCols;
	int nSize = nCols*nRows;
	if (nSize < 0)
		return false;

	// �����ڴ�
	m_pData = new double[nSize];
	
	if (m_pData == NULL)
		return false;					// �ڴ����ʧ��
	//if (IsBadReadPtr(m_pData, sizeof(double) * nSize))
	//	return false;

	// ����Ԫ��ֵ��0
	memset(m_pData, 0, sizeof(double) * nSize);

	return true;
}

//////////////////////////////////////////////////////////////////////
// �������ʼ��Ϊ��λ����
//
// ������
// 1. int nSize - ����������
//
// ����ֵ��bool �ͣ���ʼ���Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CMatrix::MakeUnitMatrix(int nSize)
{
	if (! Init(nSize, nSize))
		return false;

	for (int i=0; i<nSize; ++i)
		for (int j=0; j<nSize; ++j)
			if (i == j)
				SetElement(i, j, 1);

	return true;
}

//////////////////////////////////////////////////////////////////////
// ���þ����Ԫ�ص�ֵ
//
// ������
// 1. double value[] - һά���飬����Ϊm_nNumColumns*m_nNumRows���洢
//                     �����Ԫ�ص�ֵ
//
// ����ֵ����
//////////////////////////////////////////////////////////////////////
void CMatrix::SetData(double value[])
{
	// empty the memory
	memset(m_pData, 0, sizeof(double) * m_nNumColumns*m_nNumRows);
	// copy data
	memcpy(m_pData, value, sizeof(double)*m_nNumColumns*m_nNumRows);
}

//////////////////////////////////////////////////////////////////////
// ����ָ��Ԫ�ص�ֵ
//
// ������
// 1. int nRows - ָ���ľ�������
// 2. int nCols - ָ���ľ�������
// 3. double value - ָ��Ԫ�ص�ֵ
//
// ����ֵ��bool �ͣ�˵�������Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CMatrix::SetElement(int nRow, int nCol, double value)
{
	if (nCol < 0 || nCol >= m_nNumColumns || nRow < 0 || nRow >= m_nNumRows)
		return false;						// array bounds error
	if (m_pData == NULL)
		return false;							// bad pointer error
	
	m_pData[nCol + nRow * m_nNumColumns] = value;

	return true;
}

//////////////////////////////////////////////////////////////////////
// ����ָ��Ԫ�ص�ֵ
//
// ������
// 1. int nRows - ָ���ľ�������
// 2. int nCols - ָ���ľ�������
//
// ����ֵ��double �ͣ�ָ��Ԫ�ص�ֵ
//////////////////////////////////////////////////////////////////////
double CMatrix::GetElement(int nRow, int nCol) const
{
	assert(nCol >= 0 && nCol < m_nNumColumns && nRow >= 0 && nRow < m_nNumRows); // array bounds error
	assert(m_pData);							// bad pointer error
	return m_pData[nCol + nRow * m_nNumColumns] ;		//�ɼ��������ȴ洢����ͬ�е�Ԫ������
}

//////////////////////////////////////////////////////////////////////
// ��ȡ���������
//
// ��������
//
// ����ֵ��int �ͣ����������
//////////////////////////////////////////////////////////////////////
int	CMatrix::GetNumColumns() const
{
	return m_nNumColumns;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ���������
//
// ��������
//
// ����ֵ��int �ͣ����������
//////////////////////////////////////////////////////////////////////
int	CMatrix::GetNumRows() const
{
	return m_nNumRows;
}

//////////////////////////////////////////////////////////////////////
// ��ȡ���������
//
// ��������
//
// ����ֵ��double��ָ�룬ָ������Ԫ�ص����ݻ�����
//////////////////////////////////////////////////////////////////////
double* CMatrix::GetData() const
{
	return m_pData;
}

//////////////////////////////////////////////////////////////////////
// ��ȡָ���е�����
//
// ������
// 1. int nRows - ָ���ľ�������
// 2.  double* pVector - ָ�������и�Ԫ�صĻ�����
//
// ����ֵ��int �ͣ�������Ԫ�صĸ����������������
//////////////////////////////////////////////////////////////////////
int CMatrix::GetRowVector(int nRow, double* pVector) const
{
	if (pVector == NULL)
		delete pVector;

	pVector = new double[m_nNumColumns];
	assert(pVector != NULL);

	for (int j=0; j<m_nNumColumns; ++j)
		pVector[j] = GetElement(nRow, j);

	return m_nNumColumns;
}

//////////////////////////////////////////////////////////////////////
// ��ȡָ���е�����
//
// ������
// 1. int nCols - ָ���ľ�������
// 2.  double* pVector - ָ�������и�Ԫ�صĻ�����
//
// ����ֵ��int �ͣ�������Ԫ�صĸ����������������
//////////////////////////////////////////////////////////////////////
int CMatrix::GetColVector(int nCol, double* pVector) const
{
	if (pVector == NULL)
		delete pVector;

	pVector = new double[m_nNumRows];
	assert(pVector != NULL);

	for (int i=0; i<m_nNumRows; ++i)
		pVector[i] = GetElement(i, nCol);

	return m_nNumRows;
}

//////////////////////////////////////////////////////////////////////
// ���������=��������ֵ
//
// ������
// 1. const CMatrix& other - ���ڸ�����ֵ��Դ����
//
// ����ֵ��CMatrix�͵����ã������õľ�����other���
//////////////////////////////////////////////////////////////////////
CMatrix& CMatrix::operator=(const CMatrix& other)
{
	if (&other != this)
	{
		bool bSuccess = Init(other.GetNumRows(), other.GetNumColumns());
		assert(bSuccess);

		// copy the pointer
		memcpy(m_pData, other.m_pData, sizeof(double)*m_nNumColumns*m_nNumRows);
	}

	// finally return a reference to ourselves
	return *this ;
}

//////////////////////////////////////////////////////////////////////
// ���������==���жϾ����Ƿ����
//
// ������
// 1. const CMatrix& other - ���ڱȽϵľ���
//
// ����ֵ��bool �ͣ��������������Ϊtrue������Ϊfalse
//////////////////////////////////////////////////////////////////////
bool CMatrix::operator==(const CMatrix& other) const
{
	// ���ȼ���������Ƿ����
	if (m_nNumColumns != other.GetNumColumns() || m_nNumRows != other.GetNumRows())
		return false;

	for (int i=0; i<m_nNumRows; ++i)
	{
		for (int j=0; j<m_nNumColumns; ++j)
		{
			if (GetElement(i, j) != other.GetElement(i, j))
				return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// ���������!=���жϾ����Ƿ����
//
// ������
// 1. const CMatrix& other - ���ڱȽϵľ���
//
// ����ֵ��bool �ͣ����������������Ϊtrue������Ϊfalse
//////////////////////////////////////////////////////////////////////
bool CMatrix::operator!=(const CMatrix& other) const
{
	return !(*this == other);
}

//////////////////////////////////////////////////////////////////////
// ���������+��ʵ�־���ļӷ�
//
// ������
// 1. const CMatrix& other - ��ָ��������ӵľ���
//
// ����ֵ��CMatrix�ͣ�ָ��������other���֮��
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator+(const CMatrix& other) const
{
	// ���ȼ���������Ƿ����
	assert (m_nNumColumns == other.GetNumColumns() && m_nNumRows == other.GetNumRows());

	// ����������
	CMatrix	result(*this) ;		// ��������
	// ����ӷ�
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) + other.GetElement(i, j)) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// ���������-��ʵ�־���ļ���
//
// ������
// 1. const CMatrix& other - ��ָ����������ľ���
//
// ����ֵ��CMatrix�ͣ�ָ��������other���֮��
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator-(const CMatrix& other) const
{
	// ���ȼ���������Ƿ����
	assert (m_nNumColumns == other.GetNumColumns() && m_nNumRows == other.GetNumRows());

	// ����Ŀ�����
	CMatrix	result(*this) ;		// copy ourselves
	// ���м�������
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) - other.GetElement(i, j)) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// ���������*��ʵ�־��������
//
// ������
// 1. double value - ��ָ��������˵�ʵ��
//
// ����ֵ��CMatrix�ͣ�ָ��������value���֮��
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator*(double value) const
{
	// ����Ŀ�����
	CMatrix	result(*this) ;		// copy ourselves
	// ��������
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j <  m_nNumColumns; ++j)
			result.SetElement(i, j, result.GetElement(i, j) * value) ;
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// ���������*��ʵ�־���ĳ˷�
//
// ������
// 1. const CMatrix& other - ��ָ��������˵ľ���
//
// ����ֵ��CMatrix�ͣ�ָ��������other���֮��
//////////////////////////////////////////////////////////////////////
CMatrix	CMatrix::operator*(const CMatrix& other) const
{
	// ���ȼ���������Ƿ����Ҫ��
	assert (m_nNumColumns == other.GetNumRows());

	// construct the object we are going to return
	CMatrix	result(m_nNumRows, other.GetNumColumns()) ;

	// ����˷�����
	//
	// [A][B][C]   [G][H]     [A*G + B*I + C*K][A*H + B*J + C*L]
	// [D][E][F] * [I][J] =   [D*G + E*I + F*K][D*H + E*J + F*L]
	//             [K][L]
	//
	double	value ;
	for (int i = 0 ; i < result.GetNumRows() ; ++i)
	{
		for (int j = 0 ; j < other.GetNumColumns() ; ++j)
		{
			value = 0.0 ;
			for (int k = 0 ; k < m_nNumColumns ; ++k)
			{
				value += GetElement(i, k) * other.GetElement(k, j) ;
			}

			result.SetElement(i, j, value) ;
		}
	}

	return result ;
}

//////////////////////////////////////////////////////////////////////
// ������ĳ˷�
//
// ������
// 1. const CMatrix& AR - ��߸������ʵ������
// 2. const CMatrix& AI - ��߸�������鲿����
// 3. const CMatrix& BR - �ұ߸������ʵ������
// 4. const CMatrix& BI - �ұ߸�������鲿����
// 5. CMatrix& CR - �˻��������ʵ������
// 6. CMatrix& CI - �˻���������鲿����
//
// ����ֵ��bool�ͣ�������˷��Ƿ�ɹ�
//////////////////////////////////////////////////////////////////////
bool CMatrix::CMul(const CMatrix& AR, const CMatrix& AI, const CMatrix& BR, const CMatrix& BI, CMatrix& CR, CMatrix& CI) const
{
	// ���ȼ���������Ƿ����Ҫ��
	if (AR.GetNumColumns() != AI.GetNumColumns() ||
		AR.GetNumRows() != AI.GetNumRows() ||
		BR.GetNumColumns() != BI.GetNumColumns() ||
		BR.GetNumRows() != BI.GetNumRows() ||
		AR.GetNumColumns() != BR.GetNumRows())
		return false;

	// ����˻�����ʵ��������鲿����
	CMatrix mtxCR(AR.GetNumRows(), BR.GetNumColumns()), mtxCI(AR.GetNumRows(), BR.GetNumColumns());
	// ���������
    for (int i=0; i<AR.GetNumRows(); ++i)
	{
	    for (int j=0; j<BR.GetNumColumns(); ++j)
		{
			double vr = 0;
			double vi = 0;
            for (int k =0; k<AR.GetNumColumns(); ++k)
			{
                double p = AR.GetElement(i, k) * BR.GetElement(k, j);
                double q = AI.GetElement(i, k) * BI.GetElement(k, j);
                double s = (AR.GetElement(i, k) + AI.GetElement(i, k)) * (BR.GetElement(k, j) + BI.GetElement(k, j));
                vr += p - q;
                vi += s - p - q;
			}
            mtxCR.SetElement(i, j, vr);
            mtxCI.SetElement(i, j, vi);
        }
	}

	CR = mtxCR;
	CI = mtxCI;

	return true;
}

//////////////////////////////////////////////////////////////////////
// �����ת��
//
// ��������
//
// ����ֵ��CMatrix�ͣ�ָ������ת�þ���
//////////////////////////////////////////////////////////////////////
CMatrix CMatrix::Transpose() const
{
	// ����Ŀ�����
	CMatrix	Trans(m_nNumColumns, m_nNumRows);

	// ת�ø�Ԫ��
	for (int i = 0 ; i < m_nNumRows ; ++i)
	{
		for (int j = 0 ; j < m_nNumColumns ; ++j)
			Trans.SetElement(j, i, GetElement(i, j)) ;
	}

	return Trans;
}

}	// end namespace