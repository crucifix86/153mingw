#pragma once

class SkinAnimItem;
// CAnimWindow dialog

class CAnimWindow : public CDialog
{
	DECLARE_DYNAMIC(CAnimWindow)

public:
	CAnimWindow(SkinAnimItem* pItem,CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimWindow();

// Dialog Data
	enum { IDD = IDD_ANIMATE_WINDOW };

public:
	SkinAnimItem* m_pAnimItem;
	COLORREF m_clrTransparent;
public:
	SkinAnimItem* GetSkinItem() { return m_pAnimItem;}
	void ShowDialog();
	void UpdateView();
	void Tick() ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
};
