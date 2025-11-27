struct Attribute
{
	CString cName;
	CString cValue;
};

struct AttributeNode
{
	AttributeNode	*next;
	Attribute		*data;

	~AttributeNode()
	{
		if (data)
		{
			delete data;
			data = NULL;
		}
	}
};

class XMLLabelAttribute
{
public:
	AttributeNode *Head;
public:
	XMLLabelAttribute();
	~XMLLabelAttribute();
	void Release();
	void AddAttribute(Attribute *attrib);
	LPCTSTR GetAttribute(CString cName);
	BOOL SetAttribute(CString cName,CString cValue);
};

struct XMLLabel
{
	XMLLabelAttribute	*Attrib;
	XMLLabel			*FirstChildLabel;
	XMLLabel			*NextLabel;
	XMLLabel			*ParentLabel;
	CString				LabelType;

	void ReleaseChild(XMLLabel* pLabel);
	void ReleaseAll();
	void Release()
	{
		if (Attrib)
		{
			delete Attrib;
			Attrib = NULL;
		}
	}
	~XMLLabel(){Release();}
};

class XMLFile
{
public:
	XMLLabel *RootLabel;	
	FILE *fStream;
	TCHAR c;
public:
	XMLFile();
	~XMLFile();
	void Release();

	BOOL LoadFile(LPCTSTR lFileName);
	BOOL LoadLabel(XMLLabel *ParentLabel);
	void LoadString(CString &cString);
};

XMLLabelAttribute* String2Attirb(LPCTSTR lString);
