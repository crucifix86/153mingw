#pragma once
#include "newskin.h"
class CControlWindow : public CDialog
{

public:
	CControlWindow(CWnd* pParentWnd = NULL);

	virtual ~CControlWindow();

	enum { IDD = IDD_CONTROL_WINDOW };

	void ShowDialog();

	void PostNcDestroy();

	void SetOleWnd(CWnd* pWnd){m_pOleWnd = pWnd;};

	DECLARE_MESSAGE_MAP()
	
private:
	COLORREF m_clrTransparent;
	CBrush m_brush;
	CWnd* m_pOleWnd;

protected:
	virtual void OnOK();
	virtual void OnCancel();
	void EndDialog(int nResult);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
};

