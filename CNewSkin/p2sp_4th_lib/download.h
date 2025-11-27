#pragma once

#include <map>
#include <string>

#include <IPHlpApi.h>
#include <WinInet.h>

namespace cdcsr 
{
	class WininetHelper
	{
	public:
		/**********************************************************************************************
			解析http URL
		*/
		template<typename CH> struct _URL_COMPONENTS;
		template<> struct _URL_COMPONENTS<char> : public URL_COMPONENTSA { };
		template<> struct _URL_COMPONENTS<wchar_t> : public URL_COMPONENTSW { };
		template<class STR> 
		static bool _InternetCrackUrl(const STR& str, _URL_COMPONENTS<typename STR::value_type>* url_components) { return false; }
		template<> static bool _InternetCrackUrl<std::string>(const std::string& url, _URL_COMPONENTS<char>* url_components) { 
			return !!::InternetCrackUrlA((LPCSTR)url.c_str(), (DWORD)url.length(), ICU_DECODE, url_components);
		}
		template<> static bool _InternetCrackUrl<std::wstring>(const std::wstring& url, _URL_COMPONENTS<wchar_t>* url_components) { 
			return !!::InternetCrackUrlW((LPCWSTR)url.c_str(), (DWORD)url.length(), ICU_DECODE, url_components);
		}
		template<class STR> 
		class URIComponents
		{
		public:
			URIComponents() : error_(true) {
				init();
			}

			URIComponents(const STR& url) : error_(true) {
				init();
				if(WininetHelper::_InternetCrackUrl(url, &url_components_)) {
					// 仅支持HTTP方式下载 
					if(url_components_.nScheme == INTERNET_SCHEME_HTTP) {
						error_ = false;
					}
				}
			}
			virtual ~URIComponents() {}
			bool error() {	return error_; }
			_URL_COMPONENTS<typename STR::value_type> url_components_;

			URIComponents(const URIComponents<STR>& r) {
				assign(r);
			}

			template<typename U>
			URIComponents(const URIComponents<U>& r) {
				DCHECK(false);
			}

			URIComponents<STR>& operator=(const URIComponents<STR>& r) {
				assign(r);
				return *this;
			}

			void assign(const URIComponents& other) {
				error_ = other.error_;

				url_scheme_ = other.url_scheme_;	
				url_host_ = other.url_host_;	
				url_path_ = other.url_path_;
				user_name_ = other.user_name_;
				password_ = other.password_;	
				extra_info_ = other.extra_info_;

				memset(&url_components_, 0, sizeof(url_components_));
				url_components_.dwStructSize = other.url_components_.dwStructSize;
				url_components_.lpszScheme = &url_scheme_[0];
				url_components_.dwSchemeLength = other.url_components_.dwSchemeLength;
				url_components_.lpszHostName = &url_host_[0];
				url_components_.dwHostNameLength = other.url_components_.dwHostNameLength;
				url_components_.lpszUrlPath = &url_path_[0];
				url_components_.dwUrlPathLength = other.url_components_.dwUrlPathLength;

				url_components_.lpszUserName = &user_name_[0];
				url_components_.dwUserNameLength = other.url_components_.dwUserNameLength;
				url_components_.lpszPassword = &password_[0];
				url_components_.dwPasswordLength = other.url_components_.dwPasswordLength;
				url_components_.lpszExtraInfo = &extra_info_[0];
				url_components_.dwExtraInfoLength = other.url_components_.dwExtraInfoLength;
			}

		protected:
			void init() {
				url_scheme_.resize(INTERNET_MAX_SCHEME_LENGTH, 0);
				url_host_.resize(INTERNET_MAX_HOST_NAME_LENGTH, 0);
				url_path_.resize(INTERNET_MAX_PATH_LENGTH, 0);

				user_name_.resize(INTERNET_MAX_USER_NAME_LENGTH, 0);
				password_.resize(INTERNET_MAX_PASSWORD_LENGTH, 0);	
				extra_info_.resize(INTERNET_MAX_PATH_LENGTH, 0);

				memset(&url_components_, 0, sizeof(url_components_));
				url_components_.dwStructSize = sizeof(url_components_);
				url_components_.lpszScheme = &url_scheme_[0];
				url_components_.dwSchemeLength = (DWORD)url_scheme_.size();
				url_components_.lpszHostName = &url_host_[0];
				url_components_.dwHostNameLength = (DWORD)url_host_.size();
				url_components_.lpszUrlPath = &url_path_[0];
				url_components_.dwUrlPathLength = (DWORD)url_path_.size();
				url_components_.lpszUserName = &user_name_[0];
				url_components_.dwUserNameLength = (DWORD)user_name_.size();
				url_components_.lpszPassword = &password_[0];
				url_components_.dwPasswordLength = (DWORD)password_.size();
				url_components_.lpszExtraInfo = &extra_info_[0];
				url_components_.dwExtraInfoLength = (DWORD)extra_info_.size();
			}

		private:
			bool error_;

			std::vector<typename STR::value_type> url_scheme_;		//[INTERNET_MAX_SCHEME_LENGTH];
			std::vector<typename STR::value_type> url_host_;		//[INTERNET_MAX_HOST_NAME_LENGTH];
			std::vector<typename STR::value_type> url_path_;		//[INTERNET_MAX_PATH_LENGTH];

			std::vector<typename STR::value_type> user_name_;		//[INTERNET_MAX_SCHEME_LENGTH];
			std::vector<typename STR::value_type> password_;		//[INTERNET_MAX_HOST_NAME_LENGTH];
			std::vector<typename STR::value_type> extra_info_;		//[INTERNET_MAX_PATH_LENGTH];
		};
		typedef URIComponents<std::string> URIComponentsA;
		typedef URIComponents<std::wstring> URIComponentsW;
	};
} //namespace cdcsr

namespace cdcsr 
{
	typedef std::string String;
	typedef std::wstring wString;
	typedef std::map<String, String> StringMap;

	class SimpleHttp
	{
	public:
		/***********************************************************************
			简单下载
		***********************************************************************/
		static bool download(const String& url, String* response_body = NULL
			, const StringMap* parameters = NULL, int* response_code = NULL, int timeout = 30 * 1000 /*毫秒*/);

		/***********************************************************************
			简单上传
		***********************************************************************/
		static bool upload(const String& url, const wString& upload_file, const String& file_part_name, 
			const StringMap* parameters = NULL, String* response_body = NULL, int* response_code = NULL, int timeout = 30 * 1000 /*毫秒*/);
	};
} //namespace cdcsr