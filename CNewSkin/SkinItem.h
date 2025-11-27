class	CSkinWindow;
class	XMLLabelAttribute;
struct	XMLLabel;
class	SkinWindowList;
class	SkinScrollBar;
class	SkinComboBox;
struct	SkinItemNode;
class	SkinItem;

#include <vector>

using namespace Gdiplus;

const int MAX_FRAME = 16;

int String2Int(CString cString);

/////////////////////////////////////////ItemType/////////////////////////////////////////////
enum ItemType
{
	SK_BUTTON,
	SK_BROWSER,
	SK_TEXTFIELD,
	SK_PASSWORDFIELD,
	SK_PROGRESSBAR,
	SK_SCROLLBAR,
	SK_SCROLLARROW,
	SK_LISTBOX,
	SK_COMBOBOX,
	SK_Slider,
	SK_ANIMITEM,
};

/////////////////////////////////////////SkinArt///////////////////////////////////////////////
class SkinArt
{
public:
	Bitmap*	pMap;
	Bitmap* pBG;
	Bitmap* pSelected;
	Bitmap* pClicked;
	Bitmap* pFocus;
	Bitmap* pFocusClicked;
	Bitmap* pHover;
	Bitmap* pDisable;
	Bitmap* pDisableAndSel;
	Bitmap* pUnfilled;
	Bitmap* pFilled;
	Bitmap* pExpandBG;
	Bitmap* pDisFocus;
//	Bitmap* pAnimFrame[MAX_FRAME];
	std::vector<Bitmap*> pAnimFrame;

	XMLLabel *m_xLabel;
public:
	SkinArt(XMLLabel *xLabel);
	~SkinArt();
	void Release();
	void LoadArt();
};

/////////////////////////////////////////SkinStyle////////////////////////////////////////////
class SkinStyle
{
public:
	Color		m_cDefault;
	Color		m_cDisabled;
	Color		m_cHover;
	Font*		m_pFont;
	XMLLabel	*m_xLabel;
	CString		m_cName;
public:
	SkinStyle(XMLLabel *xLabel);
	~SkinStyle()
	{
		if (m_pFont)
		{
			delete m_pFont;
		}
	}
	void LoadStyle();
};

/////////////////////////////////////////SkinOption////////////////////////////////////////////
class SkinOption
{
public:
	XMLLabel			*m_xLabel;
	CString				m_cName;
	SkinWindowList		*m_sWindowList;
	SkinItemNode		*m_sItemHead;
	XMLLabelAttribute	*m_xAttrib;
public:
	SkinOption(SkinWindowList *sWindowList,XMLLabel *xLabel);
	~SkinOption();
	void AddItem(SkinItem *sItem);
	void LoadOption(BOOL bDraw=FALSE);
	void SaveOption();
};
/////////////////////////////////////////SkinGroup////////////////////////////////////////////
class SkinGroup
{
public:
	XMLLabel		*m_xLabel;
	CString			m_cName;
	SkinWindowList	*m_sWindowList;
	SkinItemNode	*m_sItemHead;
public:
	SkinGroup(SkinWindowList *sWindowList,XMLLabel *xLabel);
	~SkinGroup();
	void AddItem(SkinItem *sItem);
	void SetSelected(SkinItem *sItem);
};
/////////////////////////////////////////SkinItem/////////////////////////////////////////////
class SkinItem
{
public:
	CSkinWindow	*m_cParentWnd;
	CString		m_cName;
	int			m_iTabID;
	Color		m_cMapColor;
	CRect		m_cPosition;
	RectF		m_cRectF;
	BOOL		m_bToggle;
	BOOL		m_bPlainText;
	BOOL		m_bIsSelected;
	BOOL		m_bIsEnable;
	BOOL		m_bIsHover;
	BOOL		m_bIsFocus;
	BOOL		m_bIsClilcked;

	XMLLabel	*m_xLabel;
	ItemType	m_iType;
	SkinStyle	*m_sStyle;
	CString		m_cAlign;
	CString		m_cVAlign;
	SkinArt		*m_sArt;
	SkinComboBox*m_sUpperComboBox;
	SkinGroup	*m_sGroup;
public:
	SkinItem(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	virtual ~SkinItem()
	{
		Release();
	}
	virtual void Release()
	{
		if (m_sArt)
		{
			delete m_sArt;
			m_sArt = NULL;
		}
	}
//	BOOL Command(CString lCommand);
//	BOOL Command(XMLLabelAttribute *xAttirb);
	virtual BOOL Draw();
};

struct SkinItemNode
{
	SkinItemNode* next;
	SkinItem *item;

	~SkinItemNode()
	{
		//
	}
};

/////////////////////////////////////////SkinButton////////////////////////////////////////////
class SkinButton : public SkinItem
{
public:
	CString		m_cButtonText;
public:
	SkinButton(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinButton(){}
//	BOOL Command(CString lCommand);
	BOOL Draw();
	void SetOption(CString cOption);
	CString	GetOption();
};

/////////////////////////////////////////SkinAnimItem ////////////////////////////////////////////
class SkinAnimItem : public SkinItem
{
public:
	int		m_iCurIndex;
	int		m_iCount;
public:
	SkinAnimItem(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinAnimItem(){}
	//	BOOL Command(CString lCommand);
	BOOL Draw(){return TRUE;};
	BOOL Draw(Graphics* pGraphic);
	void Tick();
};

/////////////////////////////////////////SkinBrowser////////////////////////////////////////////
class SkinBrowser : public SkinItem , public CWnd
{
public:
	CString m_cURL;
public:
	SkinBrowser(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	virtual ~SkinBrowser();
//	BOOL Command(CString lCommand);
	void BrowseURL();
	BOOL Draw();
//	void Stop();
};

/////////////////////////////////////////SkinTextField/////////////////////////////////////////////
class SkinTextField : public SkinItem,public CEdit
{
public:
	CString		m_cText;
	int			m_iSelOrg;
	int			m_iFirstChar;
	int			m_iLastChar;
	int			m_iDir;
public:
	SkinTextField(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinTextField(){};
//	BOOL Command(CString lCommand);
	BOOL Draw();
	void OnChar(UINT nChar);
	void TextChange();
	void LButtonDown(CPoint p);
	void Drag(CPoint p);
	void SetOption(CString cOption);
	CString	GetOption();
};
/////////////////////////////////////////SkinPasswordField/////////////////////////////////////////////
class SkinPasswordField : public SkinTextField
{
public:
	SkinPasswordField(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	void TextChange();
};
/////////////////////////////////////////SkinProgressBar/////////////////////////////////////////////
class SkinProgressBar : public SkinItem
{
public:
	int		m_iPos;
	int		m_iOldPos;
	int		m_iMin;
	int		m_iMax;
public:
	SkinProgressBar(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinProgressBar(){};
	BOOL Draw();
	void SetPos(int i);
};
/////////////////////////////////////////SkinListBox/////////////////////////////////////////////
class SkinListBox : public SkinItem
{
public:
	int				m_iSelectedLine;
	int				m_iHoverLine;
	int				m_iFirstLine;
	int				m_iTotalLine;
	int				m_iBoxLine;
	int				m_iLineHeight;
	int				m_iHoverY;
	bool			m_bHover;
	Color			m_cbgHover;
	Color			m_cbgSelected;
	CString			*m_cMemberText;
	SkinScrollBar	*m_sScrollBar;
	SkinComboBox	*m_sParentComboBox;
public:
	SkinListBox(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinListBox()
	{
		Release();
	}
	void Release()
	{
		if (m_cMemberText)
		{
			delete []m_cMemberText;
		}
	}
	BOOL SetFirstLine(int iLine);
	BOOL Press(int iY);
	BOOL SetLine(int iLine);
	BOOL Hover(int iY);
	BOOL Draw();
	BOOL DrawLine(int iLine);
	void SetOption(CString c);
	CString	GetOption();
	void LoadListFromFile(LPCTSTR lFileName);
};
/////////////////////////////////////////SkinScrollBar/////////////////////////////////////////////
class SkinScrollBar : public SkinItem
{
public:
	int			m_iScrollPos;
	int			m_iScrollLen;
	int			m_iSliderHeight;
	SkinListBox	*m_sParentListBox;
public:
	SkinScrollBar(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinScrollBar(){};
//	BOOL Command(CString lCommand);
	BOOL SetPos(int iLine);
	BOOL Press(int iY);
	BOOL Drag(int iY);
	BOOL Draw();
};
/////////////////////////////////////////SkinScrollArrow/////////////////////////////////////////////
class SkinScrollArrow : public SkinItem
{
public:
	SkinListBox	*m_sParentListBox;
	int			m_iArrowType;
public:
	SkinScrollArrow(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinScrollArrow(){};
	BOOL Press();
	BOOL Draw();
};
/////////////////////////////////////////SkinComboBox/////////////////////////////////////////////
class SkinComboBox : public SkinButton
{
public:
	SkinListBox	*m_sListBox;
	BOOL		m_bExpand;
	CRect		m_cExpandRect;
public:
	SkinComboBox(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinComboBox(){};
	BOOL Draw();
	BOOL Expand();
	BOOL Close();
};
/////////////////////////////////////////SkinSlider/////////////////////////////////////////////
class SkinSlider : public SkinItem
{
public:
	int			m_iSliderPos;
	int			m_iSliderLen;
	int			m_iSliderWidth;
	int			m_iMax;
	int			m_iMin;
	int			m_iPos;
	bool		m_bExpand;
	bool		m_bDrawAllExpand;
	int			m_iExpandWidth;
	int			m_iExpandHeight;
	SkinListBox	*m_sParentListBox;
public:
	SkinSlider(CSkinWindow *cParentWnd,XMLLabel *xLabel);
	~SkinSlider(){};
	BOOL SetPos(int iPos);
	int  GetPos();
	void SetOption(CString cOption);
	CString	GetOption();
	BOOL Press(int iX);
	BOOL Drag(int iX);
	BOOL Draw();
};
