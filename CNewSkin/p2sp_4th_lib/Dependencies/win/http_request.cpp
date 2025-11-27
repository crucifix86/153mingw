
#include "http_request.h"

#include "string_conversion.h"

#include <fstream>
#include <string>
#include <vector>
#include <WinInet.h>
#include <atlbase.h>

#pragma comment(lib, "WinINet")
#pragma warning(disable: 4267)

interface IHttpRequestCallback
{
    virtual void OnStartReadResponse(size_t content_length) = 0;
    virtual bool OnReportData(const BYTE* data, size_t data_len) = 0;
    virtual void OnStopReadResponse() = 0;
};
class CHttpRequest
{
public:
    CHttpRequest(const char* url, IHttpRequestCallback* callback);
    ~CHttpRequest();

    bool SendRequest(const char* header, size_t header_len,
        const char* body, size_t body_len, int* timeout);

private:
    HINTERNET internet_;
    HINTERNET connect_;
    HINTERNET request_;

    IHttpRequestCallback* callback_;

    BYTE stack_buffer_[8192];
};

CHttpRequest::CHttpRequest(const char* url, IHttpRequestCallback* callback)
: internet_(NULL), connect_(NULL), request_(NULL), callback_(callback)
{
    char scheme[INTERNET_MAX_SCHEME_LENGTH] = { 0 };
    char host[INTERNET_MAX_HOST_NAME_LENGTH] = { 0 };
    char path[INTERNET_MAX_PATH_LENGTH] = { 0 };
    URL_COMPONENTSA components;
    memset(&components, 0, sizeof(components));
    components.dwStructSize = sizeof(components);
    components.lpszScheme = scheme;
    components.dwSchemeLength = sizeof(scheme) / sizeof(scheme[0]);
    components.lpszHostName = host;
    components.dwHostNameLength = sizeof(host) / sizeof(host[0]);
    components.lpszUrlPath = path;
    components.dwUrlPathLength = sizeof(path) / sizeof(path[0]);
    if(::InternetCrackUrlA(url, 0, ICU_DECODE, &components))
    {
        internet_ = ::InternetOpenA(NULL, INTERNET_OPEN_TYPE_PRECONFIG,
            NULL, NULL, 0);
        if(internet_)
        {
            connect_ = ::InternetConnectA(internet_, host, INTERNET_INVALID_PORT_NUMBER,
                NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
            if(connect_)
            {
                DWORD http_open_flags = (components.nScheme == INTERNET_SCHEME_HTTPS)
                    ? INTERNET_FLAG_SECURE : 0;
                http_open_flags |= INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_DONT_CACHE;
                request_ = ::HttpOpenRequestA(connect_, "GET", path, NULL, NULL,
                    NULL, http_open_flags, NULL);
            }
        }
    }
}

CHttpRequest::~CHttpRequest()
{
    if(request_)
    {
        ::InternetCloseHandle(request_);
        request_ = NULL;
    }

    if(connect_)
    {
        ::InternetCloseHandle(connect_);
        connect_ = NULL;
    }

    if(internet_)
    {
        ::InternetCloseHandle(internet_);
        internet_ = NULL;
    }
}

bool CHttpRequest::SendRequest(const char* header, size_t header_len,
                               const char* body, size_t body_len, int* timeout)
{
    if(!request_)
    {
        return false;
    }

    ::HttpAddRequestHeadersA(request_, header,
        static_cast<DWORD>(header_len), HTTP_ADDREQ_FLAG_ADD);

    if(timeout)
    {
        if(!::InternetSetOptionA(request_, INTERNET_OPTION_SEND_TIMEOUT,
            timeout, sizeof(timeout)))
        {
            ATLTRACE("Could not unset send timeout, continuing...\n");
        }
    }

    if(!::HttpSendRequestA(request_, NULL, 0, const_cast<char*>(body),
        static_cast<DWORD>(body_len)))
    {
        return false;
    }

    DWORD status_code;
    DWORD cb = sizeof(status_code);
    if(!::HttpQueryInfoA(request_, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,
        &status_code, &cb, 0))
    {
        return false;
    }

    bool result = (status_code == 200);

    if(!result)
    {
        return false;
    }

    if(callback_)
    {
        DWORD content_length = 0;
        DWORD cb = sizeof(content_length);
        result = !!::HttpQueryInfoA(request_, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,
            &content_length, &cb, 0);
        if(result)
        {
            callback_->OnStartReadResponse(content_length);

            while(result)
            {
                DWORD available = 0;
                result = !!::InternetQueryDataAvailable(request_, &available, 0, 0);
                if(result)
                {
                    BYTE* response_buffer = stack_buffer_;
                    if(available > 8192)
                    {
                        response_buffer = new BYTE[available];
                    }
                    DWORD read;
                    result = !!::InternetReadFile(request_, response_buffer, available, &read);

                    if(result)
                    {
                        result = callback_->OnReportData(response_buffer, read);
                    }

                    if(response_buffer != stack_buffer_)
                    {
                        delete[] response_buffer;
                    }
                }

                if(available == 0)
                {
                    break;
                }
            }

            callback_->OnStopReadResponse();
        }
    }

    return result;
}



//////////////////////////////////////////////////////////////////////////

class CFileHttpRequestCallback : public IHttpRequestCallback
{
public:
    CFileHttpRequestCallback(HWND notify_wnd, bool* stop_flag, HANDLE file);
    ~CFileHttpRequestCallback();

    void OnStartReadResponse(size_t content_length);
    bool OnReportData(const BYTE* data, size_t data_len);
    void OnStopReadResponse();

private:
    CFileHttpRequestCallback(const CFileHttpRequestCallback&);
    void operator=(const CFileHttpRequestCallback&);

    size_t content_length_;
    size_t content_progress_;
    HWND notify_wnd_;
    bool* stop_flag_;
    HANDLE file_;
};

CFileHttpRequestCallback::CFileHttpRequestCallback(HWND notify_wnd,
                                                   bool* stop_flag,
                                                   HANDLE file)
                                                   : content_length_(0),
                                                   content_progress_(0),
                                                   notify_wnd_(notify_wnd),
                                                   stop_flag_(stop_flag),
                                                   file_(file) {}

CFileHttpRequestCallback::~CFileHttpRequestCallback() {}

void CFileHttpRequestCallback::OnStartReadResponse(size_t content_length)
{
    content_length_ = content_length;
}

bool CFileHttpRequestCallback::OnReportData(const BYTE* data, size_t data_len)
{
    if(!file_)
    {
        return false;
    }

    DWORD write = 0;
    bool ret = !!::WriteFile(file_, data, data_len, &write, NULL);

    content_progress_ += data_len;
    if(notify_wnd_ != NULL)
    {
        // inform the dialog box to display current status,
        // don't use PostMessage
        DOWNLOADPROGRESS downloadStatus =
        {
            content_progress_, content_length_
        };
        ::SendMessage(notify_wnd_, WM_USER_DISPLAYSTATUS,
            0, reinterpret_cast<LPARAM>(&downloadStatus));
    }

    if(stop_flag_ != NULL)
    {
        if(*stop_flag_)
        {
            ATLTRACE("Canceled.\n");
            ::SendMessage(notify_wnd_, WM_USER_DISPLAYSTATUS, 0, NULL);
            ret = false;
        }
    }

    return ret;
}

void CFileHttpRequestCallback::OnStopReadResponse() {}

bool HttpDownloadToFile(HWND notify_wnd, bool* stop_flag,
                        const char* url, const wchar_t* file_path)
{
    if(!file_path)
    {
        return false;
    }

    HANDLE doload_file = ::CreateFileW(file_path, GENERIC_WRITE, 0,
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(doload_file == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    CFileHttpRequestCallback callback(notify_wnd, stop_flag, doload_file);
    CHttpRequest http(url, &callback);
    bool ret = http.SendRequest(NULL, 0, NULL, 0, NULL);
    ::CloseHandle(doload_file);

    if(!ret)
    {
        ::DeleteFileW(file_path);
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

class CStreamHttpRequestCallback : public IHttpRequestCallback
{
public:
    CStreamHttpRequestCallback(HWND notify_wnd, bool* stop_flag, IStream* stream);
    ~CStreamHttpRequestCallback();

    void OnStartReadResponse(size_t content_length);
    bool OnReportData(const BYTE* data, size_t data_len);
    void OnStopReadResponse();

private:
    CStreamHttpRequestCallback(const CStreamHttpRequestCallback&);
    void operator=(const CStreamHttpRequestCallback&);

    size_t content_length_;
    size_t content_progress_;
    HWND notify_wnd_;
    bool* stop_flag_;
    IStream* stream_;
};

CStreamHttpRequestCallback::CStreamHttpRequestCallback(HWND notify_wnd,
                                                   bool* stop_flag,
                                                   IStream* stream)
                                                   : content_length_(0),
                                                   content_progress_(0),
                                                   notify_wnd_(notify_wnd),
                                                   stop_flag_(stop_flag),
                                                   stream_(stream) {}

CStreamHttpRequestCallback::~CStreamHttpRequestCallback() {}

void CStreamHttpRequestCallback::OnStartReadResponse(size_t content_length)
{
    content_length_ = content_length;
}

bool CStreamHttpRequestCallback::OnReportData(const BYTE* data, size_t data_len)
{
    if(!stream_)
    {
        return false;
    }

    DWORD write = 0;
    bool ret = SUCCEEDED(stream_->Write(data, data_len, &write));

    content_progress_ += data_len;
    if(notify_wnd_ != NULL)
    {
        // inform the dialog box to display current status,
        // don't use PostMessage
        DOWNLOADPROGRESS downloadStatus =
        {
            content_progress_, content_length_
        };
        ::SendMessage(notify_wnd_, WM_USER_DISPLAYSTATUS,
            0, reinterpret_cast<LPARAM>(&downloadStatus));
    }

    if(stop_flag_ != NULL)
    {
        if(*stop_flag_)
        {
            ATLTRACE("Canceled.\n");
            ::SendMessage(notify_wnd_, WM_USER_DISPLAYSTATUS, 0, NULL);
            ret = false;
        }
    }

    return ret;
}

void CStreamHttpRequestCallback::OnStopReadResponse() {}

bool HttpDownloadToStream(HWND notify_wnd, bool* stop_flag,
                          const char* url, IStream** stream)
{
    if(!stream)
    {
        return false;
    }

    if(FAILED(CreateStreamOnHGlobal(NULL, TRUE, stream)))
    {
        return false;
    }
    ATLASSERT(*stream);

    CStreamHttpRequestCallback callback(notify_wnd, stop_flag, *stream);
    CHttpRequest http(url, &callback);
    bool ret = http.SendRequest(NULL, 0, NULL, 0, NULL);

    if(!ret)
    {
        (*stream)->Release();
        *stream = NULL;
    }
    else
    {
        LARGE_INTEGER reset = { 0 };
        (*stream)->Seek(reset, STREAM_SEEK_SET, NULL);
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

// Generates a new multipart boundary for a POST request
static std::string GenerateMultipartBoundary()
{
    // The boundary has 27 '-' characters followed by 16 hex digits
    static const char kBoundaryPrefix[] = "---------------------------";
    static const int kBoundaryLength = 27 + 16 + 1;

    // Generate some random numbers to fill out the boundary
    int r0 = rand();
    int r1 = rand();

    char temp[kBoundaryLength];
    sprintf_s(temp, kBoundaryLength, "%s%08X%08X", kBoundaryPrefix, r0, r1);

    // remove when VC++7.1 is no longer supported
    temp[kBoundaryLength-1] = '\0';

    return std::string(temp);
}

// Generates a HTTP request header for a multipart form submit.
static std::string GenerateRequestHeader(const std::string& boundary)
{
    std::string header = "Content-Type: multipart/form-data; boundary=";
    header += boundary;
    return header;
}

// Fills the supplied vector with the contents of filename.
static void GetFileContents(const wchar_t* filename, std::vector<char>* contents)
{
    // The "open" method on pre-MSVC8 ifstream implementations doesn't accept a
    // wchar_t* filename, so use _wfopen directly in that case.  For VC8 and
    // later, _wfopen has been deprecated in favor of _wfopen_s, which does
    // not exist in earlier versions, so let the ifstream open the file itself.
#if _MSC_VER >= 1400  // MSVC 2005/8
    std::ifstream file;
    file.open(filename, std::ios::binary);
#else  // _MSC_VER >= 1400
    ifstream file(_wfopen(filename, L"rb"));
#endif  // _MSC_VER >= 1400
    if(file.is_open())
    {
        file.seekg(0, std::ios::end);
        std::streamoff length = file.tellg();
        contents->resize(length);
        if(length != 0)
        {
            file.seekg(0, std::ios::beg);
            file.read(&((*contents)[0]), length);
        }
        file.close();
    }
    else
    {
        contents->clear();
    }
}

// Given a set of parameters, an upload filename, and a file part name,
// generates a multipart request body string with these parameters
// and minidump contents.  Returns true on success.
static bool GenerateRequestBody(const std::wstring& upload_file,
                                const std::string& file_part_name,
                                const std::string& boundary,
                                std::string* request_body)
{
    std::vector<char> contents;
    GetFileContents(upload_file.c_str(), &contents);
    if(contents.empty())
    {
        return false;
    }

    request_body->clear();

    // Now append the upload file as a binary (octet-stream) part
    std::string filename_utf8 = WideToUTF8(upload_file);
    if(filename_utf8.empty())
    {
        return false;
    }

    request_body->append("--" + boundary + "\r\n");
    request_body->append("Content-Disposition: form-data; "
        "name=\"" + file_part_name + "\"; "
        "filename=\"" + filename_utf8 + "\"\r\n");
    request_body->append("Content-Type: application/octet-stream\r\n");
    request_body->append("\r\n");

    if(!contents.empty())
    {
        request_body->append(&(contents[0]), contents.size());
    }
    request_body->append("\r\n");
    request_body->append("--" + boundary + "--\r\n");
    return true;
}

bool HttpUploadFile(const char* url, const wchar_t* file_path,
                    const char* file_part_name, int* timeout)
{
    std::string boundary = GenerateMultipartBoundary();
    std::string content_type_header = GenerateRequestHeader(boundary);
    std::string request_body;
    if(!GenerateRequestBody(file_path, file_part_name,
        boundary, &request_body))
    {
        return false;
    }

    CHttpRequest http(url, NULL);
    bool ret = http.SendRequest(content_type_header.c_str(),
        content_type_header.length(), request_body.c_str(),
        request_body.length(), timeout);

    return ret;
}

//////////////////////////////////////////////////////////////////////////