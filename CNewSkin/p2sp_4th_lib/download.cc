

#include "p2spStableHeadres.h"
#include "download.h"

namespace
{
	/************************************************************************/
	/* Auto HINTERNET                                                       */
	/************************************************************************/
	static const CHAR kUserAgent[] = "Breakpad/1.0 (Windows) - cdcsr";

	/************************************************************************/
	/* Auto HINTERNET                                                       */
	/************************************************************************/
	class InternetHandle {
	public:
		explicit InternetHandle(HINTERNET handle) : handle_(handle) { 
		}
		~InternetHandle() {
			if(handle_) {
				::InternetCloseHandle(handle_);
				handle_ = NULL;
			}
		}
		HINTERNET& Get() { return handle_; }

	private:
		HINTERNET handle_;
	};

	/************************************************************************/
	/* 检查参数是否符合HTTP标准                                             */
	/************************************************************************/
	bool CheckParameters(const cdcsr::StringMap* parameters)
	{
		for(cdcsr::StringMap::const_iterator it = parameters->begin(); it != parameters->end(); ++it) {
			const std::string& str = it->first;
			if(str.empty()) return false; // disallow empty parameter names
			for(size_t i = 0; i < str.size(); ++i) {
				CHAR c = str[i];
				if(c < 32 || c == '"' || c > 127) {
					return false;
				}
			}
		}
		return true;
	}

	/************************************************************************/
	/* 读取页面                                                             */
	/************************************************************************/
	bool ReadResponse(HINTERNET request, std::string* response)
	{
		bool has_content_length_header = false;
		CHAR content_length[32];
		DWORD content_length_size = sizeof(content_length);
		DWORD claimed_size = 0;
		std::string response_body;

		if(::HttpQueryInfoA(request, HTTP_QUERY_CONTENT_LENGTH,
			static_cast<LPVOID>(&content_length), &content_length_size, 0)) {
				has_content_length_header = true;
				claimed_size = strtol(content_length, NULL, 10);
				response_body.reserve(claimed_size);
		}

		DWORD bytes_available;
		DWORD total_read = 0;
		BOOL return_code;
		while(((return_code = ::InternetQueryDataAvailable(request, &bytes_available, 0, 0)) != 0) && bytes_available > 0)
		{
			std::vector<char> response_buffer(bytes_available);
			DWORD size_read;
			return_code = ::InternetReadFile(request, &response_buffer[0], bytes_available, &size_read);
			if(return_code && size_read > 0) {
				total_read += size_read;
				response_body.append(&response_buffer[0], size_read);
			}
			else break;
		}

		bool succeeded = return_code && (!has_content_length_header || (total_read==claimed_size));
		if(succeeded && response) {
			response->swap(response_body);
			//response_body.swap(*response);
		}
		return succeeded;
	}

	/*****************************************************************************************************************
		生成任务边界分割行
	*/
	cdcsr::String GenerateMultipartBoundary()
	{
		// The boundary has 27 '-' characters followed by 16 hex digits
		static const CHAR kBoundaryPrefix[] = "---------------------------";
		static const int kBoundaryLength = 27 + 16 + 1;

		// Generate some random numbers to fill out the boundary
		int r0 = rand();
		int r1 = rand();

		CHAR temp[kBoundaryLength] = { 0 };
		sprintf_s(temp, kBoundaryLength, "%s%08X%08X", kBoundaryPrefix, r0, r1);

		// remove when VC++7.1 is no longer supported
		temp[kBoundaryLength-1] = '\0';

		return cdcsr::String(temp);
	}

	/*****************************************************************************************************************
		生成请求头行
	*/
	cdcsr::String GenerateRequestHeader(const cdcsr::String& boundary)
	{
		cdcsr::String header = "Content-Type: multipart/form-data; boundary=";
		header += boundary;
		return header;
	}

	/*****************************************************************************************************************
	*/
	static void ReadFileContents(const cdcsr::wString& filename, std::vector<char>* contents)
	{
		// The "open" method on pre-MSVC8 ifstream implementations doesn't accept a
		// wchar_t* filename, so use _wfopen directly in that case.  For VC8 and
		// later, _wfopen has been deprecated in favor of _wfopen_s, which does
		// not exist in earlier versions, so let the ifstream open the file itself.
#	if _MSC_VER >= 1400  // MSVC 2005/8
		std::ifstream file;
		file.open(filename.c_str(), std::ios::binary);
#	else
		ifstream file(_wfopen(filename.c_str(), L"rb"));
#	endif  // _MSC_VER >= 1400
		
		if(!file.is_open()) {
			contents->clear();
			return;
		}
		
		file.seekg(0, std::ios::end);
		std::streamoff length = file.tellg();
		contents->resize(length);
		if(length != 0) {
			file.seekg(0, std::ios::beg);
			file.read(&((*contents)[0]), length);
		}
		file.close();
	}

	/*****************************************************************************************************************
	*/
	bool GenerateRequestBody(const cdcsr::StringMap* parameters, const cdcsr::wString& upload_file, 
		const cdcsr::String& file_part_name, const cdcsr::String& boundary, cdcsr::String* request_body)
	{
		std::vector<char> contents;
		ReadFileContents(upload_file, &contents);
		if(contents.empty()) {
			return false;
		}

		if(boundary.empty()) {
			return false;
		}

		if(!request_body) {
			return false;
		}

		request_body->clear();

		// Append each of the parameter pairs as a form-data part
		for(cdcsr::StringMap::const_iterator pos = parameters->begin(); pos != parameters->end(); ++pos)
		{
			request_body->append("--" + boundary + "\r\n");
			request_body->append("Content-Disposition: form-data; name=\"" + pos->first + "\"\r\n\r\n" + pos->second + "\r\n");
		}

		// Now append the upload file as a binary (octet-stream) part
		std::string filename_utf8 = WideToUTF8(upload_file);
		if(filename_utf8.empty()) {
			return false;
		}

		request_body->append("--" + boundary + "\r\n");
		request_body->append("Content-Disposition: form-data; " "name=\"" + file_part_name + "\"; " "filename=\"" + filename_utf8 + "\"\r\n");
		request_body->append("Content-Type: application/octet-stream\r\n");
		request_body->append("\r\n");

		if(!contents.empty()) {
			request_body->append(&(contents[0]), contents.size());
		}
		request_body->append("\r\n");
		request_body->append("--" + boundary + "--\r\n");

		return true;
	}

} //namespace

namespace cdcsr 
{
	//-------------------------------------------------------------------------------------------------------
	bool SimpleHttp::download( const String& url, String* response_body /*= NULL */, 
		const StringMap* parameters /*= NULL*/, int* response_code /*= NULL*/, int timeout /*= 30 * 1000*/ )
	{
		if(parameters && !CheckParameters(parameters)) {
			return false;
		}

		WininetHelper::URIComponentsA uri(url);
		if(uri.error()) {
			return false;
		}

		// 不支持非http https协议
		bool secure = String("https") == uri.url_components_.lpszScheme;
		if(!secure && String("http") != uri.url_components_.lpszScheme) return false;  

		InternetHandle internet(
			::InternetOpenA(kUserAgent,
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL, // proxy name
			NULL, // proxy bypass
			0));  // flags
		if(!internet.Get()) {
			return false;
		}

		InternetHandle connection(
			::InternetConnectA(internet.Get(),
			uri.url_components_.lpszHostName,
			uri.url_components_.nPort,
			NULL,   // user name
			NULL,   // password
			INTERNET_SERVICE_HTTP,
			0,      // flags
			NULL)); // context
		if(!connection.Get()) {
			return false;
		}

		DWORD dwOpenFlags = secure ? INTERNET_FLAG_SECURE : 0;
		dwOpenFlags |= INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_DONT_CACHE;
		InternetHandle request(
			::HttpOpenRequestA(connection.Get(),
			"GET",
			uri.url_components_.lpszUrlPath,
			HTTP_VERSIONA,		// version
			NULL,				// referer
			NULL,				// agent type
			dwOpenFlags,
			NULL));				// context
		if(!request.Get()) {
			return false;
		}

		if(timeout) {
			if(!::InternetSetOptionA(request.Get(), INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset send timeout, continuing...\n");
			}
			if(!::InternetSetOptionW(request.Get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset receive timeout, continuing...\n");
			}
		}

		if(parameters) {
			for(StringMap::const_iterator i = parameters->begin(); i != parameters->end(); ++i) {
				const String& name = i->first;
				const String& value = i->second;
				String line(name + ":" + value + "\r\n");
				::HttpAddRequestHeadersA(request.Get(), line.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);
			}
		}

		if(!::HttpSendRequestA(request.Get(), NULL, 0, NULL, 0)) {
			return false;
		}

		// The server indicates a successful upload with HTTP status 200.
		CHAR http_status[4];
		DWORD http_status_size = sizeof(http_status);
		if(!::HttpQueryInfoA(request.Get(), HTTP_QUERY_STATUS_CODE, static_cast<LPVOID>(&http_status), &http_status_size, 0)) {
			return false;
		}

		int http_response = strtol(http_status, NULL, 10);
		if(response_code) {
			*response_code = http_response;
		}

		bool result = (http_response == 200);
		if(result) {
			result = ReadResponse(request.Get(), response_body);
		}
		return result;
	}

	//-------------------------------------------------------------------------------------------------------
	bool SimpleHttp::upload( const String& url, const wString& upload_file, const String& file_part_name, 
		const StringMap* parameters /*= NULL*/, String* response_body /*= NULL*/, int* response_code /*= NULL*/, int timeout /*= 30 * 1000 毫秒*/ )
	{
		if(parameters && !CheckParameters(parameters)) {
			return false;
		}

		WininetHelper::URIComponentsA uri(url);
		if(uri.error()) {
			return false;
		}

		// 不支持非http https协议
		bool secure = String("https") == uri.url_components_.lpszScheme;
		if(!secure && String("http") != uri.url_components_.lpszScheme) return false;  

		InternetHandle internet(
			::InternetOpenA(kUserAgent,
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL, // proxy name
			NULL, // proxy bypass
			0));  // flags
		if(!internet.Get()) {
			return false;
		}

		InternetHandle connection(
			::InternetConnectA(internet.Get(),
			uri.url_components_.lpszHostName,
			uri.url_components_.nPort,
			NULL,   // user name
			NULL,   // password
			INTERNET_SERVICE_HTTP,
			0,      // flags
			NULL)); // context
		if(!connection.Get()) {
			return false;
		}

		DWORD dwOpenFlags = secure ? INTERNET_FLAG_SECURE : 0;
		dwOpenFlags |= INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_DONT_CACHE;
		InternetHandle request(
			::HttpOpenRequestA(connection.Get(),
			"GET",
			uri.url_components_.lpszUrlPath,
			HTTP_VERSIONA,		// version
			NULL,				// referer
			NULL,				// agent type
			dwOpenFlags,
			NULL));				// context
		if(!request.Get()) {
			return false;
		}

		if(timeout) {
			if(!::InternetSetOptionA(request.Get(), INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset send timeout, continuing...\n");
			}
			if(!::InternetSetOptionW(request.Get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset receive timeout, continuing...\n");
			}
		}

		String boundary(GenerateMultipartBoundary());
		String content_type_header(GenerateRequestHeader(boundary));
		::HttpAddRequestHeadersA(request.Get(), content_type_header.c_str(), static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);

		String request_body;
		if(!GenerateRequestBody(parameters, upload_file, file_part_name, boundary, &request_body)) {
			return false;
		}

		if(timeout) {
			if(!::InternetSetOptionA(request.Get(), INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset send timeout, continuing...\n");
			}
			if(!::InternetSetOptionW(request.Get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(int))) {
				::OutputDebugStringA("Could not unset receive timeout, continuing...\n");
			}
		}

		if(!::HttpSendRequestA(request.Get(), NULL, 0, const_cast<char*>(request_body.data()), static_cast<DWORD>(request_body.size()))) {
			return false;
		}

		// The server indicates a successful upload with HTTP status 200.
		CHAR http_status[4];
		DWORD http_status_size = sizeof(http_status);
		if(!::HttpQueryInfoA(request.Get(), HTTP_QUERY_STATUS_CODE, static_cast<LPVOID>(&http_status), &http_status_size, 0)) {
			return false;
		}

		int http_response = strtol(http_status, NULL, 10);
		if(response_code) {
			*response_code = http_response;
		}
		bool result = (http_response == 200);
		if(result) {
			result = ReadResponse(request.Get(), response_body);
		}

		return result;
	}

} //namespace cdcsr
