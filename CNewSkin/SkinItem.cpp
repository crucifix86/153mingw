#include "stdafx.h"
#include "SkinItem.h"
#include "SkinWindow.h"
#include "XMLFile.h"
#include <Mmsystem.h>
#include <math.h>
#include "ControlWindow.h"

int String2Int(CString cString)
{
	if(cString==TEXT(""))
		return 0;
	int i=0;
	int r=0;
	while(i<cString.GetLength())
	{
		if(cString.GetAt(i)>='0'&&cString.GetAt(i)<='9')
		{
			r*=10;
			r+=cString.GetAt(i)-48;
		}
		i++;
	}
	return r;
}

Color GetGDIPlusColor(CString cString)
{
	if(cString == TEXT(""))
		return Color(0,0,0);
	int i = 0;
	int iComma = 0;
	bool bUseAlpha = false;
	int a = 0;
	int r = 0;
	int g = 0;
	int b = 0;
	while (i < cString.GetLength())
	{
		if (cString.GetAt(i) == ',')
		{
			++iComma;
		}
		++i;
	}
	if (3 == iComma)
	{
		bUseAlpha = true;
	}

	i = 0;
	while(cString.GetAt(i) != ',')
	{
		if(cString.GetAt(i) != ' ')
		{
			r *= 10;
			r += cString.GetAt(i) - 48;
		}
		i++;
	}
	i++;
	while(cString.GetAt(i) != ',')
	{
		if(cString.GetAt(i) != ' ')
		{
			g *= 10;
			g += cString.GetAt(i) - 48;
		}
		i++;
	}
	i++;
	if (bUseAlpha)
	{
		while(cString.GetAt(i) != ',')
		{
			if(cString.GetAt(i) != ' ')
			{
				b *= 10;
				b += cString.GetAt(i) - 48;
			}
			i++;
		}
		i++;
		while(i < cString.GetLength())
		{
			if(cString.GetAt(i) != ' ')
			{
				a *= 10;
				a += cString.GetAt(i) - 48;
			}
			i++;
		}
		return Color(a,r,g,b);

	}
	else
	{
		while(i < cString.GetLength())
		{
			if(cString.GetAt(i) != ' ')
			{
				b *= 10;
				b += cString.GetAt(i) - 48;
			}
			i++;
		}
		// alpha是255的话文字会变得透明
		return Color(254,r,g,b);
	}
}
/////////////////////////////////////////SkinArt//////////////////////////////////////////////
SkinArt::SkinArt(XMLLabel *xLabel)
{
	m_xLabel=xLabel;
	pMap = NULL;
	pBG = NULL;
	pSelected = NULL;
	pFocus = NULL;
	pHover = NULL;
	pDisable = NULL;
	pDisableAndSel = NULL;
	pUnfilled = NULL;
	pFilled = NULL;
	pExpandBG = NULL;
	pDisFocus = NULL;
	pClicked = NULL;
	pFocusClicked = NULL;
// 	for (int i=0;i<MAX_FRAME;i++)
// 	{
// 		pAnimFrame[i] = NULL;
// 	}
}

SkinArt::~SkinArt()
{
	Release();
}

void SkinArt::Release()
{
	Gdiplus::DllExports::GdipFree(pMap);
	Gdiplus::DllExports::GdipFree(pBG);
	Gdiplus::DllExports::GdipFree(pSelected);
	Gdiplus::DllExports::GdipFree(pClicked);
	Gdiplus::DllExports::GdipFree(pFocus);
	Gdiplus::DllExports::GdipFree(pFocusClicked);
	Gdiplus::DllExports::GdipFree(pHover);
	Gdiplus::DllExports::GdipFree(pDisable);
	Gdiplus::DllExports::GdipFree(pDisableAndSel);
	Gdiplus::DllExports::GdipFree(pUnfilled);
	Gdiplus::DllExports::GdipFree(pFilled);
	Gdiplus::DllExports::GdipFree(pDisFocus);
	Gdiplus::DllExports::GdipFree(pExpandBG);

	for (int i=0;i<pAnimFrame.size();i++)
	{
		Gdiplus::DllExports::GdipFree(pAnimFrame[i]);
	}
}

void SkinArt::LoadArt()
{
	XMLLabel *xLabel=m_xLabel->FirstChildLabel;
	while(xLabel!=NULL)
	{
		if(xLabel->LabelType==TEXT("SkinArt"))
			break;
		xLabel=xLabel->NextLabel;
	}
	if(xLabel==NULL)
		return;

	XMLLabel *chLabel=xLabel->FirstChildLabel;
	while(chLabel!=NULL)
	{
		if(chLabel->LabelType==TEXT("Image"))
		{
			CString lType=chLabel->Attrib->GetAttribute(TEXT("Type"));
			LPCTSTR lPath=chLabel->Attrib->GetAttribute(TEXT("Path"));
			{
				if(lType==TEXT("BackgroundImage"))
				{
					pBG = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("MapImage"))
				{
					pMap = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("SelectedImage"))
				{
					pSelected = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("ClickedImage"))
				{
					pClicked = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("FocusedImage"))
				{
					pFocus = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("FocusClickedImage"))
				{
					pFocusClicked = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("HoverImage"))
				{
					pHover = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("DisabledImage"))
				{
					pDisable = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("DisabledAndSelectedImage"))
				{
					pDisableAndSel = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("UnfilledImage"))
				{
					pUnfilled = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("FilledImage"))
				{
					pFilled = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("ExpandBackgroundImage"))
				{
					pExpandBG = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if(lType==TEXT("DisableFocusedImage"))
				{
					pDisFocus = Gdiplus::Bitmap::FromFile(lPath);
				}
				else if (lType.Left(5) == TEXT("Frame"))
				{
// 					int index = String2Int(lType.Mid(5));
// 					if(index<1) index = 1;
// 					if(index>MAX_FRAME) index = MAX_FRAME;
// 					pAnimFrame[index-1] = Gdiplus::Bitmap::FromFile(lPath);
					Bitmap* p = Gdiplus::Bitmap::FromFile(lPath);
					pAnimFrame.push_back(p);
				}
			}
		}
		chLabel=chLabel->NextLabel;
	}
}

/////////////////////////////////////////SkinStyle/////////////////////////////////////////////
SkinStyle::SkinStyle(XMLLabel *xLabel)
{
	m_xLabel=xLabel;
	m_pFont = NULL;
}

void SkinStyle::LoadStyle()
{
	CString cString;
	
	m_cName=m_xLabel->Attrib->GetAttribute(TEXT("Name"));
	
	cString=m_xLabel->Attrib->GetAttribute(TEXT("Default"));
	if(cString!=TEXT(""))
		m_cDefault = GetGDIPlusColor(cString);
	m_cHover = m_cDisabled = m_cDefault;
	
	cString = m_xLabel->Attrib->GetAttribute(TEXT("Disabled"));
	if(cString != TEXT(""))
		m_cDisabled = GetGDIPlusColor(cString);
	
	cString = m_xLabel->Attrib->GetAttribute(TEXT("Hover"));
	if(cString != TEXT(""))
		m_cHover = GetGDIPlusColor(cString);

	int iWeight,iFontStyle,iHeight;
	
	cString = m_xLabel->Attrib->GetAttribute(TEXT("Weight"));
	if(cString != TEXT(""))
	{
		iWeight = String2Int(cString);
		iFontStyle = iWeight > 600 ? FontStyleBold : FontStyleRegular;
	}
	else
		iFontStyle = FontStyleRegular;
	cString = m_xLabel->Attrib->GetAttribute(TEXT("Height"));
	if(cString != TEXT(""))
		iHeight = String2Int(cString);
	else
		iHeight = 16;

	cString = m_xLabel->Attrib->GetAttribute(TEXT("Face"));
	CString cFace = TEXT("");
	int i=0;

	while(TRUE)
	{
		if(i == cString.GetLength() || cString.GetAt(i) == ',')
		{
			Font font(cFace,static_cast<float>(iHeight),iFontStyle,UnitPixel);
			if(font.IsAvailable() || i == cString.GetLength())
			{
				m_pFont = font.Clone();
				break;
			}
			cFace = TEXT("");
		}
		else
			cFace += cString.GetAt(i);
		i++;
	}
}

/////////////////////////////////////////SkinOption/////////////////////////////////////////////
SkinOption::SkinOption(SkinWindowList *sWindowList,XMLLabel *xLabel)
{
	m_xLabel=xLabel;
	m_sWindowList=sWindowList;
	m_cName=m_xLabel->Attrib->GetAttribute(TEXT("Name"));
	m_sItemHead=new SkinItemNode;
	m_sItemHead->item=NULL;
	m_sItemHead->next=NULL;
	m_xAttrib=new XMLLabelAttribute;
}

SkinOption::~SkinOption()
{
	if (m_sItemHead)
	{
		SkinItemNode* pNode = m_sItemHead->next;
		while(pNode != NULL)
		{
			SkinItemNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		delete m_sItemHead;
		m_sItemHead = NULL;
	}
	if (m_xAttrib)
	{
		delete m_xAttrib;
		m_xAttrib = NULL;
	}
}

void SkinOption::LoadOption(BOOL bDraw)
{
	CString cName=m_xLabel->Attrib->GetAttribute(TEXT("FileName"));
	FILE *fStream;
	fStream=_tfopen(cName,TEXT("rb"));
	if(fStream==NULL)
		return;
#ifdef _UNICODE
	_fgettc(fStream);
#endif
	TCHAR s[400];
	while(_fgetts(s,400,fStream)!=NULL)
	{
		TCHAR cItemName[100];
		TCHAR c[400];
		_stscanf(s,TEXT("%s %s"),cItemName,c);
		if(m_xAttrib->GetAttribute(cItemName)==NULL)
		{
			Attribute *attribNode=new Attribute;
			attribNode->cName=cItemName;
			attribNode->cValue=c;
			m_xAttrib->AddAttribute(attribNode);
		}
		else
			m_xAttrib->SetAttribute(cItemName,c);
		SkinItem *sItem=m_sWindowList->GetItem(cItemName);
		if(bDraw&&sItem!=NULL&&sItem->m_bIsEnable)
		{
			if(sItem->m_iType==SK_BUTTON)
			{
				((SkinButton*)sItem)->SetOption(c);

			}
			else if(sItem->m_iType==SK_TEXTFIELD)
			{
				((SkinTextField*)sItem)->SetOption(c);

			}
			else if(sItem->m_iType==SK_LISTBOX)
			{
				((SkinListBox*)sItem)->SetOption(c);
			//	if(((SkinListBox*)sItem)->m_sParentComboBox!=NULL)
			//		((SkinListBox*)sItem)->m_sParentComboBox->Draw();
			//	((SkinListBox*)sItem)->Draw();
				//sItem->m_cParentWnd->UpdateView();
			}
			else if(sItem->m_iType==SK_Slider)
			{
				((SkinSlider*)sItem)->SetOption(c);

			}

			sItem->m_cParentWnd->UpdateView();
		}
	}
	
	fclose(fStream);
}

void SkinOption::SaveOption()
{
	CString cName=m_xLabel->Attrib->GetAttribute(TEXT("FileName"));
	FILE *fStream;
	fStream=_tfopen(cName,TEXT("wb"));
	if(fStream==NULL)
		return;
#ifdef _UNICODE
	_fputtc(0xfeff,fStream);
#endif
	TCHAR s[400];
	CString c;
	SkinItemNode *sNode=m_sItemHead;
	while((sNode=sNode->next)!=NULL)
	{
		if(sNode->item->m_iType==SK_BUTTON)
			c=((SkinButton*)sNode->item)->GetOption();
		else if(sNode->item->m_iType==SK_TEXTFIELD)
			c=((SkinTextField*)sNode->item)->GetOption();
		else if(sNode->item->m_iType==SK_LISTBOX)
			c=((SkinListBox*)sNode->item)->GetOption();
		else if(sNode->item->m_iType==SK_Slider)
			c=((SkinSlider*)sNode->item)->GetOption();
		_stprintf(s,TEXT("%s %s\r\n"),sNode->item->m_cName,c);
		_fputts(s,fStream);
		m_xAttrib->SetAttribute(sNode->item->m_cName,c);
	}
	fclose(fStream);
}

void SkinOption::AddItem(SkinItem *sItem)
{
	SkinItemNode *sItemNode=new SkinItemNode;
	sItemNode->item=sItem;
	sItemNode->next=m_sItemHead->next;
	m_sItemHead->next=sItemNode;
}

/////////////////////////////////////////SkinGroup/////////////////////////////////////////////
SkinGroup::SkinGroup(SkinWindowList *sWindowList,XMLLabel *xLabel)
{
	m_xLabel=xLabel;
	m_sWindowList=sWindowList;
	m_cName=xLabel->Attrib->GetAttribute(TEXT("Name"));
	m_sItemHead=new SkinItemNode;
	m_sItemHead->item=NULL;
	m_sItemHead->next=NULL;
}

SkinGroup::~SkinGroup()
{
	if (m_sItemHead)
	{
		SkinItemNode* pNode = m_sItemHead->next;
		while(pNode != NULL)
		{
			SkinItemNode* pDel = pNode;
			pNode = pNode->next;
			delete pDel;
		}
		delete m_sItemHead;
		m_sItemHead = NULL;
	}
}

void SkinGroup::AddItem(SkinItem *sItem)
{
	SkinItemNode *sItemNode=new SkinItemNode;
	sItemNode->item=sItem;
	sItemNode->next=m_sItemHead->next;
	m_sItemHead->next=sItemNode;
}

void SkinGroup::SetSelected(SkinItem *sItem)
{
	SkinItemNode *sItemNode=m_sItemHead;
	while((sItemNode=sItemNode->next)!=NULL)
		if(sItemNode->item!=sItem&&sItemNode->item->m_bIsSelected)
		{
			sItemNode->item->m_bIsSelected=FALSE;
			m_sWindowList->Event(TEXT("toggleOff"),sItemNode->item);

			sItemNode->item->m_cParentWnd->UpdateView();
		}
}

/////////////////////////////////////////SkinItem/////////////////////////////////////////////
SkinItem::SkinItem(CSkinWindow *cParentWnd,XMLLabel *xLabel)
{
	CString cString;
	m_bIsClilcked=FALSE;
	m_cParentWnd=cParentWnd;
	m_cName=xLabel->Attrib->GetAttribute(TEXT("Name"));
	m_iTabID=String2Int(xLabel->Attrib->GetAttribute(TEXT("TabID")));
	m_cMapColor = GetGDIPlusColor(xLabel->Attrib->GetAttribute(TEXT("MapColor")));
	m_cMapColor.SetValue(Color::MakeARGB(255,m_cMapColor.GetR(),m_cMapColor.GetG(),m_cMapColor.GetB()));
	m_sStyle=m_cParentWnd->m_sWindowList->GetStyle(xLabel->Attrib->GetAttribute(TEXT("Style")));
	cString=xLabel->Attrib->GetAttribute(TEXT("Enable"));
	if(cString==TEXT("false"))
		m_bIsEnable=FALSE;
	else
		m_bIsEnable=TRUE;
	m_bIsFocus=FALSE;
	cString=xLabel->Attrib->GetAttribute(TEXT("Selected"));
	m_sGroup=NULL;
	if(cString==TEXT(""))
	{
		m_bToggle=FALSE;
		m_bIsSelected=FALSE;
	}
	else
	{
		m_bToggle=TRUE;
		if(cString==TEXT("true"))
			m_bIsSelected=TRUE;
		else
			m_bIsSelected=FALSE;
		cString=xLabel->Attrib->GetAttribute(TEXT("Group"));
		if(cString!="")
		{
			m_sGroup=m_cParentWnd->m_sWindowList->GetGroup(cString);
			if(m_sGroup!=NULL)
				m_sGroup->AddItem(this);
		}
	}
	cString=xLabel->Attrib->GetAttribute(TEXT("PlainText"));
	if(cString==TEXT(""))
		m_bPlainText=FALSE;
	else
		if(cString==TEXT("true"))
			m_bPlainText=TRUE;
		else
			m_bPlainText=FALSE;
	cString=xLabel->Attrib->GetAttribute("Option");
	if(cString!="")
	{
		SkinOption *s;
		s=m_cParentWnd->m_sWindowList->GetOption(cString);
		if(s!=NULL)
			s->AddItem(this);
	}
	m_bIsHover=FALSE;
	m_xLabel=xLabel;
	m_cAlign=xLabel->Attrib->GetAttribute(TEXT("Align"));
	m_cVAlign=xLabel->Attrib->GetAttribute(TEXT("VAlign"));
	m_sArt=new SkinArt(xLabel);

	m_sArt->LoadArt();

	SetRect(&m_cPosition,MAXLONG,MAXLONG,0,0);

	Bitmap* pBitmap = cParentWnd->m_sArt->pMap;
	m_sUpperComboBox=NULL;
	CString c;
	SkinComboBox *comboBox=NULL;
	if(xLabel->LabelType==TEXT("SkinListBox")&&(c=xLabel->Attrib->GetAttribute(TEXT("ComboBox")))!="")
	{
		comboBox=(SkinComboBox*)m_cParentWnd->GetItem(c);
		pBitmap = comboBox->m_sArt->pMap;
	}
	if(	(xLabel->LabelType==TEXT("SkinScrollBar")||xLabel->LabelType==TEXT("SkinScrollArrow"))&&
		(c=xLabel->Attrib->GetAttribute(TEXT("ListBox")))!="")
	{
		SkinListBox *listBox=(SkinListBox*)m_cParentWnd->GetItem(c);
		if(listBox!=NULL)
		{
			comboBox=listBox->m_sParentComboBox;
			if(comboBox!=NULL)
			{
				pBitmap = comboBox->m_sArt->pMap;
			}
		}
	}

	BitmapData* pData = new BitmapData;
	Rect rect(0,0,pBitmap->GetWidth(),pBitmap->GetHeight());
	pBitmap->LockBits(&rect,ImageLockModeRead,PixelFormat32bppARGB,pData);
	DWORD* pixels = (DWORD*)pData->Scan0;

	for (int Row = 0; Row < pBitmap->GetHeight();++Row)
	{

		for (int Column = 0;Column < pBitmap->GetWidth();++Column)
		{		

			if (m_cMapColor.GetValue() == pixels[Row * pData->Stride / 4 + Column])
			{
				if(Column<m_cPosition.left)
					m_cPosition.left=Column;
				if(Column>m_cPosition.right)
					m_cPosition.right=Column;
				if(Row<m_cPosition.top)
					m_cPosition.top=Row;
				if(Row>m_cPosition.bottom)
					m_cPosition.bottom=Row;
			}

		} 
	}

	pBitmap->UnlockBits(pData);
	m_cPosition.bottom++;
	m_cPosition.right++;
	if(xLabel->LabelType==TEXT("SkinListBox"))
	{
		CString c=xLabel->Attrib->GetAttribute(TEXT("ComboBox"));
		if(c!="")
		{
			SkinComboBox *comboBox=(SkinComboBox*)m_cParentWnd->GetItem(c);
			if(comboBox!=NULL)
			{
				comboBox->m_cExpandRect.left=comboBox->m_cPosition.left;
				comboBox->m_cExpandRect.right=comboBox->m_cPosition.left+m_sArt->pBG->GetWidth();
				comboBox->m_cExpandRect.top=comboBox->m_cPosition.bottom;
				comboBox->m_cExpandRect.bottom=comboBox->m_cPosition.bottom+m_sArt->pBG->GetHeight();
			}
		}
	}

	m_cRectF.X = static_cast<float>(m_cPosition.left);
	m_cRectF.Y = static_cast<float>(m_cPosition.top) + 1;//GDI+绘制文字，垂直居中有误差。+1是抵消这个误差
	m_cRectF.Width = static_cast<float>(m_cPosition.right) - static_cast<float>(m_cPosition.left);
	m_cRectF.Height = static_cast<float>(m_cPosition.bottom) - static_cast<float>(m_cPosition.top) + 2;

}

BOOL SkinItem::Draw()
{
	Gdiplus::Bitmap* pSrcImage;
	BOOL b=TRUE;
	if(!m_bIsEnable)
		if(m_bIsSelected&&m_sArt->pDisableAndSel!=NULL)
			pSrcImage=m_sArt->pDisableAndSel;
		else
			pSrcImage=m_sArt->pDisable;
	else if(m_bIsClilcked)
		if(m_cParentWnd->m_sTabItem==this)
			pSrcImage=m_sArt->pFocusClicked;
		else
			pSrcImage=m_sArt->pClicked;
	else if(m_bIsSelected)
		pSrcImage=m_sArt->pSelected;
	else if(m_bIsHover)
		pSrcImage=m_sArt->pHover;
	else if(m_cParentWnd->m_sTabItem==this)
		pSrcImage=m_sArt->pFocus;
	else	
		pSrcImage=m_sArt->pBG;
	if(pSrcImage==NULL)
		pSrcImage=m_sArt->pBG;
	if(pSrcImage==NULL)
	{
		b=FALSE;
		pSrcImage=m_cParentWnd->m_sArt->pBG;
	}

	Gdiplus::Graphics* pGraph = m_cParentWnd->GetGraphics();
	if (pGraph == NULL)
	{
		return TRUE;
	}

	if (b)
	{
		pGraph->DrawImage(pSrcImage,m_cPosition.left,m_cPosition.top,pSrcImage->GetWidth(),pSrcImage->GetHeight());
	}

	return TRUE;
}

/////////////////////////////////////////SkinButton/////////////////////////////////////////////
extern bool bIsOverseaVersion;
SkinButton::SkinButton(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_cButtonText=xLabel->Attrib->GetAttribute(TEXT("Text"));
	m_iType=SK_BUTTON;
	SetOption(m_cParentWnd->m_sWindowList->GetOptionValue(m_cName));
	CString sName=xLabel->Attrib->GetAttribute(TEXT("Name"));
	if (!sName.Compare(TEXT("P2SPDownload")))
	{
		bIsOverseaVersion = false;
	}
}

void SkinButton::SetOption(CString cOption)
{
	if(cOption==TEXT("1"))
		m_bIsSelected=TRUE;
	else if(cOption==TEXT("0"))
		m_bIsSelected=FALSE;
}

CString SkinButton::GetOption()
{
	if(m_bIsSelected)
		return TEXT("1");
	else
		return TEXT("0");
}

BOOL SkinButton::Draw()
{
	SkinItem::Draw();
	
	Graphics* pGraph = m_cParentWnd->GetGraphics();
	if (pGraph == NULL)
	{
		return TRUE;
	}

	if(m_cButtonText!=TEXT(""))
	{
		Color cText;
		if(m_sStyle!=NULL)
		{
			if(!m_bIsEnable)
				cText = m_sStyle->m_cDisabled;
			else if(m_bIsHover)
				cText = m_sStyle->m_cHover;
			else
				cText = m_sStyle->m_cDefault;
		}
		SolidBrush brush(cText);

		Gdiplus::StringFormat format;

		if(m_cAlign==TEXT("left"))
			format.SetAlignment(StringAlignmentNear);
		else if(m_cAlign==TEXT("right"))
			format.SetAlignment(StringAlignmentFar);
		else
			format.SetAlignment(StringAlignmentCenter);

		if(m_cVAlign==TEXT("top"))
			format.SetLineAlignment(StringAlignmentNear);
		else if(m_cVAlign==TEXT("bottom"))
			format.SetLineAlignment(StringAlignmentFar);
		else
			format.SetLineAlignment(StringAlignmentCenter);

		if(m_iType!=SK_COMBOBOX)
			pGraph->DrawString(m_cButtonText,m_cButtonText.GetLength(),m_sStyle->m_pFont,m_cRectF,&format,&brush);
		else
		{
			RectF rect = m_cRectF;
			rect.X -= 6;

			pGraph->DrawString(m_cButtonText,m_cButtonText.GetLength(),m_sStyle->m_pFont,rect,&format,&brush);
		}
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
SkinAnimItem::SkinAnimItem(CSkinWindow *cParentWnd,XMLLabel *xLabel)
:SkinItem(cParentWnd,xLabel)
{
	m_iType=SK_ANIMITEM;
	m_iCurIndex = 0;
	m_iCount=String2Int(xLabel->Attrib->GetAttribute(TEXT("Count")));
	if(m_iCount<1) m_iCount = 1;
}

void SkinAnimItem::Tick()
{
	m_iCurIndex++; 
	m_iCurIndex = m_iCurIndex % m_iCount;
}
BOOL SkinAnimItem::Draw(Graphics* pGraph)
{
	Gdiplus::Bitmap* pSrcImage = NULL;
	
// 	if (m_iCurIndex>=0 && m_iCurIndex<MAX_FRAME)
// 	{
// 		pSrcImage=m_sArt->pAnimFrame[m_iCurIndex];
// 	}
	if (m_iCurIndex>=0 && m_iCurIndex<m_sArt->pAnimFrame.size())
	{
		pSrcImage = m_sArt->pAnimFrame[m_iCurIndex];
	}

//	Gdiplus::Graphics* pGraph = m_cParentWnd->GetGraphics();
	if (pGraph == NULL)
	{
		return TRUE;
	}

	if (pSrcImage)
	{
	//	pGraph->DrawImage(pSrcImage,m_cPosition.left,m_cPosition.top,pSrcImage->GetWidth(),pSrcImage->GetHeight());
		pGraph->DrawImage(pSrcImage,0,0,pSrcImage->GetWidth(),pSrcImage->GetHeight());
	}	

	return TRUE;
}

/////////////////////////////////////////SkinBrowser/////////////////////////////////////////////
SkinBrowser::SkinBrowser(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_iType=SK_BROWSER;
	m_cURL=xLabel->Attrib->GetAttribute(TEXT("InitURL"));
	RECT rect;
	rect.left = 0;
	rect.right = m_cPosition.right - m_cPosition.left;
	rect.top = 0;
	rect.bottom = m_cPosition.bottom - m_cPosition.top;
	CreateControl(CLSID_WebBrowser,NULL,WS_VISIBLE|WS_CHILD,rect,m_cParentWnd->GetCtrlWnd(),IDX_BROWSER);
	m_cParentWnd->GetCtrlWnd()->SetOleWnd(this);
}
SkinBrowser::~SkinBrowser()
{
	
//	IUnknown* pIUnknown = GetControlUnknown();
//	if (pIUnknown)
//	{
//		pIUnknown->Release();
//	}
//	DestroyWindow();
	
}


void SkinBrowser::BrowseURL()
{
	if(m_cURL==TEXT(""))
		return;

	static BYTE parms[] =VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	COleVariant varEmpty[4];
	
	InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, NULL, parms,m_cURL, &varEmpty[0], &varEmpty[1], &varEmpty[2], &varEmpty[3]);
}
BOOL SkinBrowser::Draw()
{
	return TRUE;
}

/////////////////////////////////////////SkinTextField/////////////////////////////////////////////
SkinTextField::SkinTextField(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_iType=SK_TEXTFIELD;
	m_iSelOrg=0;
	m_iFirstChar=0;
	m_iLastChar=0;
	m_iDir=-1;
	m_cText=TEXT("");
	Create(WS_CHILD|ES_AUTOHSCROLL,m_cPosition,m_cParentWnd,IDC_EDIT);

	SetOption(m_cParentWnd->m_sWindowList->GetOptionValue(m_cName));
}

void SkinTextField::SetOption(CString cOption)
{
	m_cText=cOption;
	SetWindowText(cOption);
	SetSel(cOption.GetLength(),cOption.GetLength());
}

CString SkinTextField::GetOption()
{
	CString c;
	GetWindowText(c);
	return c;
}

void SkinTextField::LButtonDown(CPoint p)
{
	int sel=CharFromPos(p);
	SetSel(sel,sel);
	int nStartChar,nEndChar;
	GetSel(nStartChar,nEndChar);
	m_iSelOrg=nStartChar;
	TextChange();
}

void SkinTextField::Drag(CPoint p)
{
	p.y=m_cPosition.Height()/2;
	int sel=CharFromPos(p);
	int nStartChar,nEndChar;
	GetSel(nStartChar,nEndChar);
	if(nEndChar==m_iSelOrg)
		SetSel(sel,nEndChar);
	else
		SetSel(nStartChar,sel);
	TextChange();
}

void SkinTextField::TextChange() 
{
	GetWindowText(m_cText);
	m_cParentWnd->UpdateView();
}

void GDIPlusFontToGDIFont(Graphics* pGraph,Font* pFontIn,CFont& fontOut)
{
	LOGFONT logFont;
	pFontIn->GetLogFontW(pGraph,&logFont);

	fontOut.CreateFontIndirect(&logFont);
}

BOOL SkinTextField::Draw()
{
	SkinItem::Draw();
	int i=m_cText.GetLength();

	CDC *dc=m_cParentWnd->GetMemoryDC();
	if(dc==NULL)
		return TRUE;

	Graphics* pGraph = m_cParentWnd->GetGraphics();

	CFont font;
	GDIPlusFontToGDIFont(pGraph,m_sStyle->m_pFont,font);

	CEdit::SetFont(&font);

	CFont* oldFont;

	CString cString;
	CRect cTextRect;
	CRect cRect,cRect1,cRect2;
	UINT uAlign;
	if(m_sStyle!=NULL)
	{
		if(m_bIsHover)
			dc->SetTextColor(m_sStyle->m_cHover.ToCOLORREF());
		else if(!m_bIsEnable)
			dc->SetTextColor(m_sStyle->m_cDisabled.ToCOLORREF());
		else
			dc->SetTextColor(m_sStyle->m_cDefault.ToCOLORREF());
		oldFont = dc->SelectObject(&font);
	}
	dc->SetBkMode(TRANSPARENT);
	int nStartChar,nEndChar;
	GetSel(nStartChar,nEndChar);
	int iNewOrg;
	if(nEndChar==m_iSelOrg)
		iNewOrg=nStartChar;
	else
		iNewOrg=nEndChar;
	i=0;
	if(iNewOrg!=m_iSelOrg)
	{
		if(iNewOrg<m_iFirstChar)
		{
			m_iFirstChar=iNewOrg;
			i=-1;
		}
		if(iNewOrg>m_iLastChar)
		{
			m_iLastChar=iNewOrg;
			i=1;
		}
		if(nStartChar==nEndChar)
			m_iSelOrg=iNewOrg;
		cRect.SetRect(0,0,0,0);
		dc->DrawText(m_cText.Mid(m_iFirstChar,m_iLastChar-m_iFirstChar),cRect,DT_SINGLELINE|DT_CALCRECT);
		if(i!=0&&cRect.Width()>=m_cPosition.Width())
		{
			m_iDir=i;
			do
			{
				if(m_iDir==-1)
					m_iLastChar--;
				else
					m_iFirstChar++;
				cRect.SetRect(0,0,0,0);
				dc->DrawText(m_cText.Mid(m_iFirstChar,m_iLastChar-m_iFirstChar),cRect,DT_SINGLELINE|DT_CALCRECT);
			}while(cRect.Width()>=m_cPosition.Width());
		}
	}

	if(m_iDir==-1)
	{
		uAlign=DT_SINGLELINE|DT_VCENTER|DT_LEFT;
		dc->DrawText(m_cText.Mid(m_iFirstChar,m_iLastChar-m_iFirstChar),m_cPosition,uAlign);
		i=m_cPosition.left;
		if(iNewOrg>m_iFirstChar)
		{
			cRect.SetRect(0,0,0,0);
			dc->DrawText(m_cText.Mid(m_iFirstChar,iNewOrg-m_iFirstChar),cRect,DT_SINGLELINE|DT_CALCRECT);
			i+=cRect.Width();
		}
	}
	else
	{
		uAlign=DT_SINGLELINE|DT_VCENTER|DT_RIGHT;
		dc->DrawText(m_cText.Mid(m_iFirstChar,m_iLastChar-m_iFirstChar),m_cPosition,uAlign);
		i=m_cPosition.right-1;
		if(iNewOrg<m_iLastChar)
		{
			cRect.SetRect(0,0,0,0);
			dc->DrawText(m_cText.Mid(iNewOrg,m_iLastChar-iNewOrg),cRect,DT_SINGLELINE|DT_CALCRECT);
			i-=cRect.Width();
		}
	}
	if(nStartChar<m_iSelOrg)
	{
		uAlign=DT_SINGLELINE|DT_VCENTER|DT_LEFT;
		cRect=m_cPosition;
		cRect.left=i;
		dc->SetBkMode(OPAQUE);
		dc->SetBkColor(dc->GetTextColor());
		dc->SetTextColor(0xffffffff-dc->GetTextColor());
		dc->DrawText(m_cText.Mid(nStartChar,m_iSelOrg-nStartChar),cRect,uAlign);
	}
	else
	{
		uAlign=DT_SINGLELINE|DT_VCENTER|DT_RIGHT;
		cRect=m_cPosition;
		cRect.right=i;
		dc->SetBkMode(OPAQUE);
		dc->SetBkColor(dc->GetTextColor());
		dc->SetTextColor(0xffffffff-dc->GetTextColor());
		dc->DrawText(m_cText.Mid(m_iSelOrg,nEndChar-m_iSelOrg),cRect,uAlign);
	}
	CPen pen(PS_SOLID,1,m_sStyle->m_cDefault.ToCOLORREF());
	CPen* oldpen;
	oldpen=dc->SelectObject(&pen);
	dc->MoveTo(i,m_cPosition.top);
	dc->LineTo(i,m_cPosition.bottom);
	dc->SelectObject(oldpen);
	dc->SelectObject(oldFont);

	return TRUE;
}

void SkinTextField::OnChar(UINT nChar)
{
	if(nChar==VK_BACK)
		m_cText.Delete(m_cText.GetLength()-1);
	else if(nChar>=0x30&&nChar<0x40)
		m_cText+=(_TCHAR)nChar;
	else if(nChar>=0x41&&nChar<=0x5a)
	{
		int i=GetKeyState(VK_SHIFT);
		int j=GetKeyState(VK_CAPITAL)&1;
		if(i<0&&j!=1||i>0&&j==1)
			nChar+=32;
		m_cText+=(_TCHAR)nChar;
	}

	m_cParentWnd->UpdateView();
}



//////////////////////////////////////SkinPasswordField/////////////////////////////////////////////
SkinPasswordField::SkinPasswordField(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinTextField(cParentWnd,xLabel)
{
	m_iType=SK_PASSWORDFIELD;
}

void SkinPasswordField::TextChange()
{
	GetWindowText(m_cText);
	for(int i=m_cText.GetLength()-1;i>=0;i--)
		m_cText.SetAt(i,TEXT('*'));

	m_cParentWnd->UpdateView();
}
/////////////////////////////////////////SkinProgressBar/////////////////////////////////////////////
SkinProgressBar::SkinProgressBar(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_iMin=String2Int(xLabel->Attrib->GetAttribute(TEXT("Min")));
	m_iMax=String2Int(xLabel->Attrib->GetAttribute(TEXT("Max")));
	if(m_iMin==m_iMax&&m_iMax==0)
	{
		m_iMin=0;m_iMax=100;
	}
	m_iPos=m_iMax;
	m_iType=SK_PROGRESSBAR;
	m_iOldPos=m_iMin;
}

BOOL SkinProgressBar::Draw()
{
	Graphics* pGraph = m_cParentWnd->GetGraphics();
	if (pGraph == NULL)
	{
		return TRUE;
	}

	int pos1,pos2;
	pos1=m_cPosition.Width()*(m_iOldPos-m_iMin)/(m_iMax-m_iMin);
	pos2=m_cPosition.Width()*(m_iPos-m_iMin)/(m_iMax-m_iMin);
	if (m_sArt->pBG != NULL)
	{
		Rect DestRect(m_cPosition.left+pos2,m_cPosition.top,m_cPosition.Width()-pos2,m_cPosition.Height());
		pGraph->DrawImage(m_sArt->pUnfilled,DestRect,0,0,m_cPosition.Width()-pos2,m_cPosition.Height(),UnitPixel,NULL,NULL,NULL);
	}
	Rect DestRect(m_cPosition.left,m_cPosition.top,pos2,m_cPosition.Height());
	pGraph->DrawImage(m_sArt->pFilled,DestRect,0,0,pos2,m_cPosition.Height(),UnitPixel,NULL,NULL,NULL);

	return TRUE;
}

void SkinProgressBar::SetPos(int i)
{
	m_iOldPos=m_iPos;
	m_iPos=i;
	if(m_iPos<m_iMin)
		m_iPos=m_iMin;
	if(m_iPos>m_iMax)
		m_iPos=m_iMax;

	m_cParentWnd->UpdateView();
}

/////////////////////////////////////////SkinListBox/////////////////////////////////////////////
SkinListBox::SkinListBox(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_sParentComboBox=NULL;
	m_iType=SK_LISTBOX;
	m_iSelectedLine=-1;
	m_iHoverLine=-1;
	m_iFirstLine=0;
	m_iTotalLine=0;
	m_cMemberText=NULL;
	m_bHover = false;
	m_iHoverY = 0;
	m_cbgSelected = GetGDIPlusColor(xLabel->Attrib->GetAttribute(TEXT("bgSelected")));
	m_cbgHover = GetGDIPlusColor(xLabel->Attrib->GetAttribute(TEXT("bgHover")));
	CString c=xLabel->Attrib->GetAttribute(TEXT("ComboBox"));
	if(c!="")
	{
		m_sParentComboBox=(SkinComboBox*)m_cParentWnd->GetItem(c);
		m_sParentComboBox->m_sListBox=this;
		m_cPosition.top+=m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.bottom+=m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.left+=m_sParentComboBox->m_cPosition.left;
		m_cPosition.right+=m_sParentComboBox->m_cPosition.left;
	}
	c=xLabel->Attrib->GetAttribute("ListFile");
	if(c=="")
	{
		XMLLabel *chLabel;
		chLabel=xLabel->FirstChildLabel;
		while(chLabel!=NULL)
		{
			if(chLabel->LabelType==TEXT("Member"))
				m_iTotalLine++;
			chLabel=chLabel->NextLabel;
		}
		m_cMemberText=new CString[m_iTotalLine];
		chLabel=xLabel->FirstChildLabel;
		int iLine=0;
		while(chLabel!=NULL)
		{
			if(chLabel->LabelType==TEXT("Member"))
				m_cMemberText[iLine++]=chLabel->Attrib->GetAttribute(TEXT("Text"));
			chLabel=chLabel->NextLabel;
		}
	}
	else
		LoadListFromFile(c);

	c=m_cParentWnd->m_sWindowList->GetOptionValue(m_cName);
	SetOption(c);
}

void SkinListBox::LoadListFromFile(LPCTSTR lFileName)
{
	m_iTotalLine=0;
	FILE *f;
	if((f=_tfopen(lFileName,TEXT("rb")))!=NULL)
	{
		TCHAR s[400];
		TCHAR c;
#ifdef _UNICODE
		_ftscanf(f,TEXT("%c"),&c);
#endif
		while(_fgetts(s,400,f)!=NULL)
			m_iTotalLine++;
		fseek(f,0,SEEK_SET);
#ifdef _UNICODE
		_ftscanf(f,TEXT("%c"),&c);
#endif
		if(m_cMemberText!=NULL)
			delete []m_cMemberText;
		m_cMemberText=new CString[m_iTotalLine];
		int iLine=0;
		while(_fgetts(s,400,f)!=NULL)
		{
			int i=0;
			int iLen=_tcsclen(s);
			while(i<iLen&&s[i]!=TEXT('"'))
				i++;
			if(i==iLen)
			{
				m_iTotalLine--;
				continue;
			}
			i++;
			int j=i;
			while(j<iLen&&s[j]!=TEXT('"'))
				j++;
			if(j==iLen)
			{
				m_iTotalLine--;
				continue;
			}
			TCHAR ss[400];
			_tcsncpy(ss,s+i,j-i);
			ss[j-i]=TEXT('\0');
			m_cMemberText[iLine]=ss;
			iLine++;
		}
		fclose(f);
	}
}

BOOL SkinListBox::SetFirstLine(int iLine)
{
	if(iLine>m_iTotalLine-m_iBoxLine)
		iLine=m_iTotalLine-m_iBoxLine;
	if(iLine<0)
		iLine=0;
	if(iLine==m_iFirstLine)
		return TRUE;
	m_iFirstLine=iLine;
	if(m_sScrollBar!=NULL)
		m_sScrollBar->SetPos(m_iFirstLine);

	m_cParentWnd->UpdateView();
	return TRUE;
}

BOOL SkinListBox::Press(int iY)
{
	int iOld=m_iSelectedLine;
	if(iY/m_iLineHeight>=m_iBoxLine)
		m_iSelectedLine=m_iBoxLine+m_iFirstLine-1;
	else
		m_iSelectedLine=iY/m_iLineHeight+m_iFirstLine;
	if(m_iSelectedLine>=m_iTotalLine)
	{
		m_iSelectedLine=iOld;
		return FALSE;
	}
	if(iOld==m_iSelectedLine)
		m_iSelectedLine=-1;

	if(m_sParentComboBox!=NULL)
	{
		if(m_iSelectedLine==-1)
			m_iSelectedLine=iOld;
		m_sParentComboBox->m_cButtonText=m_cMemberText[m_iSelectedLine];
		m_sParentComboBox->Close();
	}
	m_cParentWnd->m_sWindowList->Event(TEXT("SelChange"),this);
	return TRUE;
}

BOOL SkinListBox::SetLine(int iLine)
{
	if(iLine>=m_iTotalLine)
		return FALSE;
	if(iLine<-1)
		return FALSE;
	m_iSelectedLine=iLine;
	if(m_iSelectedLine>m_iTotalLine-m_iBoxLine)
		SetFirstLine(m_iTotalLine-m_iBoxLine);
	else
		SetFirstLine(m_iSelectedLine);
	if(m_sParentComboBox==NULL)
		m_cParentWnd->UpdateView();
	else
	{
		if(m_iSelectedLine!=-1)
			m_sParentComboBox->m_cButtonText=m_cMemberText[m_iSelectedLine];
		else
			m_sParentComboBox->m_cButtonText=m_sParentComboBox->m_xLabel->Attrib->GetAttribute(TEXT("Text"));
		m_cParentWnd->UpdateView();
	}
	return TRUE;
}

BOOL SkinListBox::Hover(int iY)
{
	m_bHover = true;
	m_iHoverY = iY;
	m_cParentWnd->UpdateView();
	return TRUE;
}

BOOL SkinListBox::DrawLine(int iLine)
{
	if(m_sParentComboBox!=NULL&&!m_sParentComboBox->m_bExpand)
		return TRUE;
	int i=iLine;
	if(i<0||i>=m_iBoxLine||i+m_iFirstLine>=m_iTotalLine)
		return TRUE;
	
	Graphics* pGraph = m_cParentWnd->GetGraphics();

	if (pGraph == NULL)
	{
		return TRUE;
	}
	
	Gdiplus::StringFormat format;
	
	if(m_cAlign==TEXT("left"))
		format.SetAlignment(StringAlignmentNear);
	else if(m_cAlign==TEXT("right"))
		format.SetAlignment(StringAlignmentFar);
	else
		format.SetAlignment(StringAlignmentCenter);

	Color cText;
	if(m_sStyle!=NULL)
	{
		if(!m_bIsEnable)
			cText = m_sStyle->m_cDisabled;
		else
			cText = m_sStyle->m_cDefault;
	}
	SolidBrush brush(cText);
	
	RectF rect;
	rect.X = static_cast<float>(m_cPosition.left);
	rect.Y = static_cast<float>(m_cPosition.top) + m_iLineHeight * i;
	rect.Width = static_cast<float>(m_cPosition.right) - m_cPosition.left;
	rect.Height = static_cast<float>(m_iLineHeight);


	if(i + m_iFirstLine == m_iSelectedLine)
	{
		Pen pen(m_cbgSelected,3);
		SolidBrush brush(m_cbgSelected);
		pGraph->DrawRectangle(&pen,rect);
		pGraph->FillRectangle(&brush,rect);
	}
	else if(i + m_iFirstLine == m_iHoverLine)
	{
		Pen pen(m_cbgHover,3);
		SolidBrush brush(m_cbgHover);
		pGraph->DrawRectangle(&pen,rect);
		pGraph->FillRectangle(&brush,rect);
	}
	else
	{
		Graphics* pGraph = m_cParentWnd->GetGraphics();
		if (pGraph)
		{
			pGraph->DrawImage(m_sArt->pBG,rect,rect.X-m_cPosition.left,rect.Y-m_cPosition.top,rect.Width,rect.Height,UnitPixel,NULL,NULL,NULL);
		}
	}

	pGraph->DrawString(m_cMemberText[i+m_iFirstLine],-1,m_sStyle->m_pFont,rect,&format,&brush);

	return TRUE;
}

BOOL SkinListBox::Draw()
{
	if(m_sParentComboBox!=NULL&&!m_sParentComboBox->m_bExpand)
		return TRUE;

	Graphics* pGraph = m_cParentWnd->GetGraphics();
	if(pGraph == NULL)
		return TRUE;

	m_iLineHeight = static_cast<int>(m_sStyle->m_pFont->GetHeight(pGraph));
	m_iBoxLine = m_cPosition.Height() / m_iLineHeight;

	Gdiplus::StringFormat format;
	if(m_cAlign==TEXT("left"))
		format.SetAlignment(StringAlignmentNear);
	else if(m_cAlign==TEXT("right"))
		format.SetAlignment(StringAlignmentFar);
	else
		format.SetAlignment(StringAlignmentCenter);

	pGraph->DrawImage(m_sArt->pBG,m_cPosition.left,m_cPosition.top,m_cPosition.Width(),m_cPosition.Height());

	Color cText;
	if(m_sStyle!=NULL)
	{
		if(!m_bIsEnable)
			cText = m_sStyle->m_cDisabled;
		else
			cText = m_sStyle->m_cDefault;
	}
	SolidBrush brush(cText);
	RectF rect;
	rect.X = static_cast<float>(m_cPosition.left);
	rect.Y = static_cast<float>(m_cPosition.top);
	rect.Width = static_cast<float>(m_cPosition.right) - m_cPosition.left;
	rect.Height = static_cast<float>(m_iLineHeight);

	for(int i=0;i<m_iBoxLine;i++)
	{
		if (i)
			rect.Offset(0,static_cast<float>(m_iLineHeight));

		if(i + m_iFirstLine >= m_iTotalLine)
			break;
		
		if(i + m_iFirstLine == m_iSelectedLine)
		{
			Pen pen(m_cbgSelected,3);
			SolidBrush brush(m_cbgSelected);
			pGraph->DrawRectangle(&pen,rect);
			pGraph->FillRectangle(&brush,rect);

		}
		else if(i+m_iFirstLine==m_iHoverLine)
		{
			Pen pen(m_cbgHover,3);
			SolidBrush brush(m_cbgHover);
			pGraph->DrawRectangle(&pen,rect);
			pGraph->FillRectangle(&brush,rect);

		}

		pGraph->DrawString(m_cMemberText[i+m_iFirstLine],-1,m_sStyle->m_pFont,rect,&format,&brush);
	}

	if (m_bHover)
	{
		m_bHover = false;
		int iOld=m_iHoverLine;
		if(m_iHoverY/m_iLineHeight>=m_iBoxLine)
			m_iHoverLine=m_iBoxLine+m_iFirstLine-1;
		else
			m_iHoverLine=m_iHoverY/m_iLineHeight+m_iFirstLine;
		if(iOld!=m_iHoverLine)
		{
			DrawLine(m_iHoverLine-m_iFirstLine);
			DrawLine(iOld-m_iFirstLine);
		}
	}
	return TRUE;
}

void SkinListBox::SetOption(CString c)
{
	if(c!="")
	{
		for(int i=0;i<m_iTotalLine;i++)
			if(m_cMemberText[i]==c)
			{
				if(m_sParentComboBox!=NULL)
					m_sParentComboBox->m_cButtonText=c;
				m_iSelectedLine=i;
				m_cParentWnd->m_sWindowList->Event(TEXT("SelChange"),this);
				return;
			}
	}
	c=m_xLabel->Attrib->GetAttribute("DefaultSel");
	if(c!="")
	{
		for(int i=0;i<m_iTotalLine;i++)
			if(m_cMemberText[i]==c)
			{
				if(m_sParentComboBox!=NULL)
					m_sParentComboBox->m_cButtonText=c;
				m_iSelectedLine=i;
				m_cParentWnd->m_sWindowList->Event(TEXT("SelChange"),this);
				return;
			}
	}
	SetLine(-1);
	m_cParentWnd->m_sWindowList->Event(TEXT("SelChange"),this);
}

CString SkinListBox::GetOption()
{
	if(m_sParentComboBox!=NULL)
		return m_sParentComboBox->m_cButtonText;
	return "";
}
/////////////////////////////////////////SkinScrollBar/////////////////////////////////////////////
SkinScrollBar::SkinScrollBar(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_iType=SK_SCROLLBAR;
	m_iScrollPos=0;

	Bitmap* pBitmap = m_sArt->pFocus;
	if (pBitmap)
	{
		m_iSliderHeight=pBitmap->GetHeight();
		m_iScrollLen=m_cPosition.bottom-m_cPosition.top-m_iSliderHeight;
	}

	CString c=xLabel->Attrib->GetAttribute(TEXT("ListBox"));
	m_sParentListBox=(SkinListBox *)m_cParentWnd->GetItem(c);
	m_sParentListBox->m_sScrollBar=this;
	if(m_sParentListBox->m_sParentComboBox!=NULL)
	{
		m_cPosition.top+=m_sParentListBox->m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.bottom+=m_sParentListBox->m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.left+=m_sParentListBox->m_sParentComboBox->m_cPosition.left;
		m_cPosition.right+=m_sParentListBox->m_sParentComboBox->m_cPosition.left;
	}
}

BOOL SkinScrollBar::SetPos(int iLine)
{
	m_iScrollPos=iLine*m_iScrollLen/(m_sParentListBox->m_iTotalLine-m_sParentListBox->m_iBoxLine);
	m_cParentWnd->UpdateView();
	return TRUE;
}

BOOL SkinScrollBar::Press(int iY)
{
	iY-=m_iSliderHeight/2;
	double d=(double)iY*(m_sParentListBox->m_iTotalLine-m_sParentListBox->m_iBoxLine)/m_iScrollLen;
	if(fabs(d-m_sParentListBox->m_iFirstLine)<1)
		if(d>m_sParentListBox->m_iFirstLine)
			m_sParentListBox->SetFirstLine(m_sParentListBox->m_iFirstLine+1);
		else
			m_sParentListBox->SetFirstLine(m_sParentListBox->m_iFirstLine-1);
	else
		m_sParentListBox->SetFirstLine((int)(d+0.5));
	return TRUE;
}

BOOL SkinScrollBar::Drag(int iY)
{
	iY-=m_iSliderHeight/2;
	double d=(double)iY*(m_sParentListBox->m_iTotalLine-m_sParentListBox->m_iBoxLine)/m_iScrollLen;
	m_sParentListBox->SetFirstLine((int)(d+0.5));
	return TRUE;
}

BOOL SkinScrollBar::Draw()
{
	if(m_sParentListBox->m_sParentComboBox==NULL||m_sParentListBox->m_sParentComboBox->m_bExpand)
	{
		Graphics* pGraph = m_cParentWnd->GetGraphics();
		if(pGraph==NULL)
			return TRUE;
		pGraph->DrawImage(m_sArt->pBG,m_cPosition.left,m_cPosition.top,m_cPosition.Width(),m_cPosition.Height());
		pGraph->DrawImage(m_sArt->pFocus,m_cPosition.left,m_cPosition.top+m_iScrollPos,m_sArt->pFocus->GetWidth(),m_sArt->pFocus->GetHeight());
	}
	return TRUE;
}

/////////////////////////////////////////SkinScrollArrow/////////////////////////////////////////////
SkinScrollArrow::SkinScrollArrow(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	m_iType=SK_SCROLLARROW;
	CString c=xLabel->Attrib->GetAttribute(TEXT("ListBox"));
	m_sParentListBox=(SkinListBox*)m_cParentWnd->GetItem(c);
	c=xLabel->Attrib->GetAttribute(TEXT("Type"));
	if(c==TEXT("inc"))
		m_iArrowType=1;
	else if(c==TEXT("dec"))
		m_iArrowType=-1;
	if(m_sParentListBox->m_sParentComboBox!=NULL)
	{
		m_cPosition.top+=m_sParentListBox->m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.bottom+=m_sParentListBox->m_sParentComboBox->m_cPosition.bottom;
		m_cPosition.left+=m_sParentListBox->m_sParentComboBox->m_cPosition.left;
		m_cPosition.right+=m_sParentListBox->m_sParentComboBox->m_cPosition.left;
	}
}

BOOL SkinScrollArrow::Press()
{
	m_sParentListBox->SetFirstLine(m_sParentListBox->m_iFirstLine+m_iArrowType);
	m_cParentWnd->UpdateView();
	return TRUE;
}

BOOL SkinScrollArrow::Draw()
{
	if(m_sParentListBox->m_sParentComboBox==NULL||m_sParentListBox->m_sParentComboBox->m_bExpand)
		SkinItem::Draw();
	return TRUE;
}

/////////////////////////////////////////SkinComboBox/////////////////////////////////////////////
SkinComboBox::SkinComboBox(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinButton(cParentWnd,xLabel)
{
	m_iType=SK_COMBOBOX;
	m_bExpand=FALSE;
}

BOOL SkinComboBox::Draw()
{
	m_bIsSelected=m_bExpand;
	SkinButton::Draw();
	if(m_bExpand)
	{
		Graphics* pGraph = m_cParentWnd->GetGraphics();
		if (pGraph)
		{
			pGraph->DrawImage(m_sArt->pExpandBG,m_cExpandRect.left,m_cExpandRect.top,m_cExpandRect.Width(),m_cExpandRect.Height());
		}
	}
	return TRUE;
}

BOOL SkinComboBox::Expand()
{
	m_bExpand=TRUE;
	m_bIsFocus=FALSE;
	m_cParentWnd->UpdateView();

	m_sListBox->SetFirstLine(0);
	SkinItemNode *sNode=m_cParentWnd->m_sItemHead;
	SkinComboBox *comboBox;
	while( (sNode=sNode->next) != NULL)
	{
		comboBox=NULL;
		if(sNode->item->m_iType==SK_LISTBOX)
			comboBox=((SkinListBox*)sNode->item)->m_sParentComboBox;
		if(sNode->item->m_iType==SK_SCROLLBAR)
			comboBox=((SkinScrollBar*)sNode->item)->m_sParentListBox->m_sParentComboBox;
		if(sNode->item->m_iType==SK_SCROLLARROW)
			comboBox=((SkinScrollArrow*)sNode->item)->m_sParentListBox->m_sParentComboBox;
		if(comboBox==this)
			m_cParentWnd->UpdateView();

		if(	comboBox!=this&&
			sNode->item->m_cPosition.right>m_cExpandRect.left&&
			sNode->item->m_cPosition.left<m_cExpandRect.right&&
			sNode->item->m_cPosition.bottom>m_cExpandRect.top&&
			sNode->item->m_cPosition.top<m_cExpandRect.bottom)
		{
			sNode->item->m_sUpperComboBox=this;
		}
	}
	return TRUE;
}

BOOL SkinComboBox::Close()
{
	m_bExpand=FALSE;
	m_bIsFocus=FALSE;
	m_cParentWnd->UpdateView();
	SkinComboBox *comboBox;

	SkinItemNode *sNode=m_cParentWnd->m_sItemHead;
	while( (sNode=sNode->next) != NULL)
	{
		comboBox=NULL;
		if(sNode->item->m_iType==SK_LISTBOX)
			comboBox=((SkinListBox*)sNode->item)->m_sParentComboBox;
		if(sNode->item->m_iType==SK_SCROLLBAR)
			comboBox=((SkinScrollBar*)sNode->item)->m_sParentListBox->m_sParentComboBox;
		if(sNode->item->m_iType==SK_SCROLLARROW)
			comboBox=((SkinScrollArrow*)sNode->item)->m_sParentListBox->m_sParentComboBox;
		if(	comboBox!=this&&
			sNode->item->m_cPosition.right>m_cExpandRect.left&&
			sNode->item->m_cPosition.left<m_cExpandRect.right&&
			sNode->item->m_cPosition.bottom>m_cExpandRect.top&&
			sNode->item->m_cPosition.top<m_cExpandRect.bottom)
		{
			sNode->item->m_sUpperComboBox=NULL;
			m_cParentWnd->UpdateView();
		}
	}
	return TRUE;
}

/////////////////////////////////////////SkinSlider/////////////////////////////////////////////
SkinSlider::SkinSlider(CSkinWindow *cParentWnd,XMLLabel *xLabel)
	:SkinItem(cParentWnd,xLabel)
{
	CString c1=xLabel->Attrib->GetAttribute(TEXT("Min"));
	CString c2=xLabel->Attrib->GetAttribute(TEXT("Max"));
	if(c1==""&&c2=="")
	{
		m_iMin=0;
		m_iMax=1;
	}
	else
	{
		m_iMin=String2Int(c1);
		m_iMax=String2Int(c2);
	}
	
	m_iType=SK_Slider;
	m_iSliderPos=0;

	Bitmap* pBitmap = m_sArt->pFocus;
	m_iSliderWidth = pBitmap->GetWidth();
	m_iSliderLen=m_cPosition.right-m_cPosition.left-m_iSliderWidth;

	m_iPos=0;
	SetOption(m_cParentWnd->m_sWindowList->GetOptionValue(m_cName));
	m_bExpand=false;
	m_bDrawAllExpand=false;
	if(m_sArt->pExpandBG!=NULL)
	{
		m_bExpand=true;
		pBitmap = m_sArt->pExpandBG;
		m_iExpandHeight = pBitmap->GetHeight();
		m_iExpandWidth=m_iExpandHeight;

		CString c=xLabel->Attrib->GetAttribute(TEXT("DrawAllEx"));
		if(c==TEXT("true"))
			m_bDrawAllExpand=true;
		c=xLabel->Attrib->GetAttribute(TEXT("ExpandWidth"));
		if(c!="")
			m_iExpandWidth=String2Int(c);
	}
}

void SkinSlider::SetOption(CString cOption)
{
	m_iPos=String2Int(cOption);
	if(m_iPos<m_iMin)
		m_iPos=m_iMin;
	if(m_iPos>m_iMax)
		m_iPos=m_iMax;
	m_iSliderPos=(m_iPos-m_iMin)*m_iSliderLen/(m_iMax-m_iMin);
}

CString SkinSlider::GetOption()
{
	TCHAR t[20];
	_stprintf(t,TEXT("%d"),m_iPos);
	return t;
}

BOOL SkinSlider::SetPos(int iPos)
{
	m_iPos=iPos;
	if(m_iPos<m_iMin)
		m_iPos=m_iMin;
	if(m_iPos>m_iMax)
		m_iPos=m_iMax;
	m_iSliderPos=(m_iPos-m_iMin)*m_iSliderLen/(m_iMax-m_iMin);

	m_cParentWnd->UpdateView();
	return TRUE;
}

int SkinSlider::GetPos()
{
	return m_iPos;
}

BOOL SkinSlider::Press(int iX)
{
	iX-=m_iSliderWidth/2;
	double d=(double)iX*(m_iMax-m_iMin)/m_iSliderLen+m_iMin;
	SetPos((int)(d+0.5));
	return TRUE;
}

BOOL SkinSlider::Drag(int iX)
{
	iX-=m_iSliderWidth/2;
	double d=(double)iX*(m_iMax-m_iMin)/m_iSliderLen+m_iMin;
	SetPos((int)(d+0.5));
	return TRUE;
}

BOOL SkinSlider::Draw()
{
	Graphics* pGraph = m_cParentWnd->GetGraphics();
	if (pGraph == NULL)
		return TRUE;

	if(m_bIsEnable)
	{
		pGraph->DrawImage(m_sArt->pBG,m_cPosition.left,m_cPosition.top,m_cPosition.Width(),m_cPosition.Height());
		pGraph->DrawImage(m_sArt->pFocus,m_cPosition.left+m_iSliderPos,m_cPosition.top,m_sArt->pFocus->GetWidth(),m_sArt->pFocus->GetHeight());

		if(m_bExpand)
		{
			int pos=0;
			for(int i=0;i<m_iMax-m_iMin+1;i++)
			{
				pos=i*m_iSliderLen/(m_iMax-m_iMin)+m_iSliderWidth/2-m_iExpandWidth/2;
				if(m_bDrawAllExpand||m_iPos==i+m_iMin)
				{
					Rect DestRect(m_cPosition.left+pos,m_cPosition.bottom,m_iExpandWidth,m_iExpandHeight);
					pGraph->DrawImage(m_sArt->pExpandBG,DestRect,pos,0,m_sArt->pExpandBG->GetWidth() - pos,m_sArt->pExpandBG->GetHeight(),UnitPixel,NULL,NULL,NULL);
				}
			}
		}
	}
	else
	{
		pGraph->DrawImage(m_sArt->pDisable,m_cPosition.left,m_cPosition.top,m_cPosition.Width(),m_cPosition.Height());
	}
	return TRUE;
}
