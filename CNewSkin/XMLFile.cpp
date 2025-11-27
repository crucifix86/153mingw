#include "StdAfx.h"
#include "XMLFile.h"
#include <stdio.h>
#include <wchar.h>

XMLLabelAttribute* String2Attirb(LPCTSTR lString)
{
	XMLLabelAttribute *xAttrib=new XMLLabelAttribute;
	if(lString==NULL||_tcsclen(lString)==0)
		return xAttrib;
	Attribute *attrib;
	int i=0;
	int len=_tcslen(lString);
	while(TRUE)
	{
		attrib=new Attribute;
		while(i<len&&lString[i]==TEXT(' '))
			i++;
		attrib->cName="";
		while(i<len&&lString[i]!=TEXT('='))
		{
			attrib->cName+=lString[i];
			i++;
		}
		i++;
		attrib->cValue="";
		while(i<len&&lString[i]!=TEXT(',')&&lString[i]!=TEXT('\0'))
		{
			attrib->cValue+=lString[i];
			i++;
		}
		xAttrib->AddAttribute(attrib);
		if(lString[i]==TEXT('\0'))
			break;
		else
			i++;
	}
	return xAttrib;
}

XMLLabelAttribute::XMLLabelAttribute()
{
	Head=new AttributeNode;
	Head->next=NULL;
	Head->data=NULL;
}

XMLLabelAttribute::~XMLLabelAttribute()
{
	Release();
}

void XMLLabelAttribute::Release()
{
	AttributeNode *Node=Head;
	while(Head!=NULL)
	{
		Node=Head;
		Head=Head->next;
		delete Node;
	}
}

void XMLLabelAttribute::AddAttribute(Attribute *attrib)
{
	AttributeNode *AttribNode=new AttributeNode;
	AttribNode->data=attrib;
	AttribNode->next=Head->next;
	Head->next=AttribNode;
}

LPCTSTR XMLLabelAttribute::GetAttribute(CString cName)
{
	AttributeNode *Node=Head;
	while((Node=Node->next)!=NULL)
		if(Node->data->cName==cName)
			return Node->data->cValue;
	return NULL;
}

BOOL XMLLabelAttribute::SetAttribute(CString cName,CString cValue)
{
	AttributeNode *Node=Head;
	while((Node=Node->next)!=NULL)
		if(Node->data->cName==cName)
		{
			Node->data->cValue=cValue;
			return TRUE;
		}
	return FALSE;
}

XMLFile::XMLFile()
{
	RootLabel=new XMLLabel;
	RootLabel->FirstChildLabel=NULL;
	RootLabel->NextLabel=NULL;
	RootLabel->Attrib=NULL;
	RootLabel->ParentLabel=NULL;
}
XMLFile::~XMLFile()
{
	Release();
}

void XMLLabel::ReleaseChild(XMLLabel* pLabel)
{
	if (pLabel == NULL || pLabel->FirstChildLabel == NULL)
	{
		return;
	}

	XMLLabel* pTemp = pLabel->FirstChildLabel->NextLabel;
	while (pTemp != NULL)
	{
		XMLLabel* pDel = pTemp;
		pTemp = pTemp->NextLabel;
		ReleaseChild(pDel);
		delete pDel;
	}

	ReleaseChild(pLabel->FirstChildLabel);

	delete pLabel->FirstChildLabel;
	pLabel->FirstChildLabel = NULL;
}
void XMLLabel::ReleaseAll()
{
	XMLLabel* pLabel = NextLabel;
	while(pLabel != NULL)
	{
		XMLLabel *pDel = pLabel;
		ReleaseChild(pLabel);
		pLabel = pLabel->NextLabel;
		delete pDel;
	}
	NextLabel = NULL;
	ReleaseChild(FirstChildLabel);
	if (FirstChildLabel)
	{
		delete FirstChildLabel;
		FirstChildLabel = NULL;
	}
}
void XMLFile::Release()
{
	if (RootLabel)
	{
		RootLabel->ReleaseAll();
		delete RootLabel;
		RootLabel = NULL;
	}
}

BOOL XMLFile::LoadFile(LPCTSTR lFileName)
{
	if((fStream=_tfopen(lFileName,TEXT("rb")))==NULL)
		return FALSE;
#ifdef _UNICODE
	_ftscanf(fStream,TEXT("%c"),&c);
#endif
	c=TEXT(' ');
	CString cString;
	LoadString(cString);
	if(cString!=TEXT("<"))
		return FALSE;
	LoadString(cString);
	RootLabel->LabelType=cString;
	RootLabel->Attrib=new XMLLabelAttribute;
	LoadString(cString);
	while(cString!=TEXT(">"))
	{
		if(cString==TEXT("/>"))
			break;
		Attribute *attrib=new Attribute;
		attrib->cName=cString;
		LoadString(attrib->cValue);
		RootLabel->Attrib->AddAttribute(attrib);
		LoadString(cString);
	}
	if(cString==TEXT(">"))
		LoadLabel(RootLabel);
	fclose(fStream);
	return TRUE;
}

BOOL XMLFile::LoadLabel(XMLLabel *ParentLabel)
{
	CString cString;
	XMLLabel *BrotherLabel=NULL;
	do
	{
		LoadString(cString);
		if(cString!=TEXT("<"))
			return FALSE;
		LoadString(cString);
		if(cString.GetAt(0)==TEXT('/'))
		{
			LoadString(cString);
			return TRUE;
		}
		XMLLabel *Label=new XMLLabel;
		Label->LabelType=cString;
		Label->FirstChildLabel=NULL;
		Label->NextLabel=NULL;
		Label->Attrib=new XMLLabelAttribute;
		Label->ParentLabel=ParentLabel;
		if(ParentLabel->FirstChildLabel==NULL)
			ParentLabel->FirstChildLabel=Label;
		else
			BrotherLabel->NextLabel=Label;
		BrotherLabel=Label;
		LoadString(cString);
		while(cString!=TEXT(">"))
		{
			if(cString==TEXT("/>"))
				break;
			Attribute *attrib=new Attribute;
			attrib->cName=cString;
			LoadString(attrib->cValue);
			Label->Attrib->AddAttribute(attrib);
			LoadString(cString);
		}
		if(cString==TEXT(">"))
			LoadLabel(Label);
	}while(TRUE);
	return TRUE;
}

void XMLFile::LoadString(CString &cString)
{
	if(c==TEXT('>'))
	{
		cString=c;
		_ftscanf(fStream,TEXT("%c"),&c);
		return;
	}
	while(c==TEXT(' ')||c==TEXT('\t')||c==TEXT('\n')||c==TEXT('\r'))
		_ftscanf(fStream,TEXT("%c"),&c);
	if(c==TEXT('<')||c==TEXT('>'))
	{
		cString=c;
		_ftscanf(fStream,TEXT("%c"),&c);
		return;
	}
	if(c==TEXT('/'))
	{
		cString=c;
		_ftscanf(fStream,TEXT("%c"),&c);
		if(c==TEXT('>'))
		{
			cString+=c;
			_ftscanf(fStream,TEXT("%c"),&c);
			return;
		}
	}
	else
		cString="";
	if(c==TEXT('"'))
	{
		_ftscanf(fStream,TEXT("%c"),&c);
		while(c!=TEXT('"'))
		{
			cString+=c;
			_ftscanf(fStream,TEXT("%c"),&c);
		};
		_ftscanf(fStream,TEXT("%c"),&c);
	}
	else
	{
		do
		{
			cString+=c;
			_ftscanf(fStream,TEXT("%c"),&c);
		}while(c!=TEXT(' ')&&c!=TEXT('=')&&c!=TEXT('>'));
		if(c==TEXT('='))
			_ftscanf(fStream,TEXT("%c"),&c);
	}
}