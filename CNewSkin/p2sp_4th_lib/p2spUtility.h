
#ifndef __p2spUtil_h__
#define __p2spUtil_h__

#include "p2spStableHeadres.h"
#include "p2spInternalStruct.h"

namespace p2sp4 
{
	class Utility
	{
	public:
		static const TiXmlElement* FindTiXmlElement(const TiXmlDocument& doc, const std::string& linkEleName)
		{
			const TiXmlElement* pRoot = doc.RootElement();
			if(!pRoot) return 0;

			const TiXmlElement* pElement = 0;
			const TiXmlElement* pOldElement = 0;
			fagex::StringVector arrayName = fagex::StringUtil::split(linkEleName, "/");
			if(arrayName.size() == 0) return pElement;

			pElement = pRoot;
			pOldElement = pElement;
			fagex::StringVector::iterator it = arrayName.begin();
			for(++it; it != arrayName.end(); ++it)
			{
				if(pElement)
				{
					pOldElement = pElement;
					pElement = pElement->FirstChildElement((*it).c_str());
				}
				if(!pElement) break;
			}
			return pElement;
		}

		//--------------------------------------------------------------------------------------------------
		static const TiXmlElement* FindTiXmlElement(const TiXmlElement* element, const std::string& linkEleName)
		{
			if(!element) return NULL;

			fagex::StringVector arrayName = fagex::StringUtil::split(linkEleName, "/");
			if(arrayName.size() == 0) return NULL;

			const TiXmlElement* pElement = element;
			fagex::StringVector::iterator it = arrayName.begin();
			for(; it != arrayName.end(); ++it) {
				if(pElement) {
					pElement = pElement->FirstChildElement((*it).c_str());
				}
				if(!pElement) break;
			}
			return pElement;
		}

		//--------------------------------------------------------------------------------------------------
		static TiXmlElement* FindTiXmlElement(TiXmlElement* element, const std::string& linkEleName)
		{
			if(!element) return NULL;

			fagex::StringVector arrayName = fagex::StringUtil::split(linkEleName, "/");
			if(arrayName.size() == 0) return NULL;

			TiXmlElement* pElement = element;
			fagex::StringVector::iterator it = arrayName.begin();
			for(; it != arrayName.end(); ++it) {
				if(pElement) {
					pElement = pElement->FirstChildElement((*it).c_str());
				}
				if(!pElement) break;
			}
			return pElement;
		}

		//--------------------------------------------------------------------------------------------------
		static TiXmlElement* MakeTiXmlElement(TiXmlElement *e, const std::string& linkEleName, 
			const fagex::StringMap* attributesMap, const std::string* value, bool created)
		{
			TiXmlElement* pRoot = e;
			TiXmlElement* pElement = 0;
			TiXmlElement* pOldElement = 0;

			fagex::StringVector arrayName = fagex::StringUtil::split(linkEleName, "/");
			if(arrayName.size() == 0) return 0;

			fagex::StringVector::iterator it = arrayName.begin();
			if(!pRoot) 
			{
				return pElement;
			}

			pElement = pRoot;
			pOldElement = pElement;
			bool isfind = false;
			for(it; it != arrayName.end(); ++it)
			{
				if(pElement)
				{
					pOldElement = pElement;
					pElement = pElement->FirstChildElement((*it).c_str());
					isfind = true;
				}
				if(!pElement)
				{
					TiXmlElement element((*it).c_str()); 
					TiXmlNode * pNode = pOldElement->InsertEndChild(element) ;
					pElement = pNode->ToElement();
					isfind = false;
				}
			}
			if(isfind && created)
			{
				TiXmlElement element((*arrayName.rbegin()).c_str()); 
				TiXmlNode * pNode = pOldElement->InsertEndChild(element) ;
				pElement = pNode->ToElement();
			}

			if(attributesMap != 0)
			{
				fagex::StringMap::const_iterator itor = attributesMap->begin();
				while(itor != attributesMap->end())
				{
					pElement->SetAttribute(itor->first.c_str(), itor->second.c_str());
					++itor;
				}
			}
			if(value != 0)
			{	
				pElement->Clear();
				TiXmlText *text = new TiXmlText(value->c_str());
				pElement->LinkEndChild(text);
			}

			return pElement;
		}

		//--------------------------------------------------------------------------------------------------
		static bool GetElementText(const TiXmlElement* pElement, 
			const std::string& linkEleName, const char** lplpText)
		{
			bool ok = true;
			do {
				const TiXmlElement* element = NULL;
				element = FindTiXmlElement(pElement, linkEleName);
				if(element == NULL || element->GetText() == NULL) { ok = false; break; }

				*lplpText = element->GetText();
			}while(false);
			return ok;
		}

		//-----------------------------------------------------------------------------------------------------
		static bool ParseServerList(const TiXmlElement* pRootelement, 
			const std::string& linkEleName, ServerParamVector* siv)
		{
			bool ok = true;
			do {
				const TiXmlElement* element = NULL;
				element = Utility::FindTiXmlElement(pRootelement, linkEleName);
				fagex::StringVector arrayName = fagex::StringUtil::split(linkEleName, "/");
				if(!element) { ok = false; break; }
				while(element != NULL) {
					P2SPServerParam t;
					if(!element->Attribute("id", (int*)&t.id)) { ok = false; break; }

					const char* sz = element->Attribute("ip");
					if(sz == NULL) { ok = false; break; }
					t.ip = ntohl((u_long)inet_addr(sz));

					sz = element->Attribute("port");
					if(sz == NULL) { ok = false; break; }
					t.port = (unsigned short)atoi(sz);

					sz = element->Attribute("arga");
					if(sz == NULL) { ok = false; break; }
					if(std::string("NORTH_SOUTH") == sz) t.isp = P2SPSA_DUPLEX;
					else if(std::string("NORTH") == sz)  t.isp = P2SPSA_UNICOM;
					else if(std::string("SOUTH") == sz)  t.isp = P2SPSA_TELECOM; 
					else t.isp = P2SPSA_DUPLEX;

					siv->push_back(t);
					element = element->NextSiblingElement(arrayName[arrayName.size() - 1].c_str());
				}
			}while(false);
			return ok;
		}

		//--------------------------------------------------------------------------------------------------
		static bool MakeExecuteAppDir(const std::wstring& wsPathApped, std::wstring& wsOutPath)
		{
			CFilePath module_path;
			if(!PathProvider(DIR_EXE, module_path)) 
				return false;
			module_path.Append(wsPathApped.c_str());

			wsOutPath = module_path.value();
			int ret = EnsureDirectoryForFile(wsOutPath.c_str(), NULL);
			if(ret != ERROR_SUCCESS && ret != ERROR_ALREADY_EXISTS) 
				return false;
			return true;
		}

		//--------------------------------------------------------------------------------------------------
		static bool MakeExecuteAppDir(const std::wstring& wsPathApped, CFilePath& wsOutPath)
		{
			if(!PathProvider(DIR_EXE, wsOutPath)) 
				return false;
			wsOutPath.Append(wsPathApped.c_str());
			
			int ret = EnsureDirectoryForFile(wsOutPath.value(), NULL);
			if(ret != ERROR_SUCCESS && ret != ERROR_ALREADY_EXISTS) 
				return false;
			return true;
		}

		//--------------------------------------------------------------------------------------------------
		static bool HttpDownloadToString(const std::string& url, std::string& content)
		{
			content.clear();

			IStream *pStream = NULL;
			if(!HttpDownloadToStream(NULL, NULL, url.c_str(), &pStream))
			{
				if(pStream) {
					pStream->Release();
				};
				return false;
			}

			char ch = 0;
			ULONG cbRead = 0;
			pStream->Read(&ch, 1, &cbRead);
			while(1 == cbRead) {
				content += ch;
				pStream->Read(&ch, 1, &cbRead);
			}
			pStream->Release();

			return true;
		}

		//--------------------------------------------------------------------------------------------------
		static std::string FileMD5(const std::wstring& wsFilename)
		{
			FILE *fp = NULL;
			_wfopen_s(&fp, wsFilename.c_str(), L"rb");
			if(!fp) return p2sp4::sNullString;

			MD5Digest digest;
			MD5Context ctx; MD5Init(&ctx);
			const int BUF_LENGTH_4K = 4 * 1024;
			char szBuffer[BUF_LENGTH_4K] = { 0 };
			while(!feof(fp))
			{
				size_t iReadCount = fread(szBuffer, 1, BUF_LENGTH_4K, fp);
				MD5Update(&ctx, szBuffer, iReadCount);
			}
			fclose(fp);
			MD5Final(&digest, &ctx);
			return MD5DigestToBase16(digest);
		}
	};
}
#endif //__p2spUtil_h__