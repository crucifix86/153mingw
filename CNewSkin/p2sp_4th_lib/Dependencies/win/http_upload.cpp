
#include "http_upload.h"

#include "string_conversion.h"

#include <fstream>

#pragma comment(lib, "Wininet")

static const wchar_t kUserAgent[] = L"Breakpad/1.0 (Windows)";

// Helper class which closes an internet handle when it goes away
class CHTTPUpload::CAutoInternetHandle
{
public:
    explicit CAutoInternetHandle(HINTERNET handle) : handle_(handle) {}
    ~CAutoInternetHandle()
    {
        if(handle_)
        {
            ::InternetCloseHandle(handle_);
        }
    }

    HINTERNET Get() { return handle_; }

private:
    HINTERNET handle_;
};

// static
bool CHTTPUpload::SendRequest(const std::wstring& url,
                              const std::map<std::wstring, std::wstring>& parameters,
                              const std::wstring& upload_file,
                              const std::wstring& file_part_name,
                              int* timeout,
                              std::wstring* response_body,
                              int* response_code)
{
    if(response_code)
    {
        *response_code = 0;
    }

    // TODO(bryner): support non-ASCII parameter names
    if(!CheckParameters(parameters))
    {
        return false;
    }

    // Break up the URL and make sure we can handle it
    wchar_t scheme[INTERNET_MAX_SCHEME_LENGTH];
    wchar_t host[INTERNET_MAX_HOST_NAME_LENGTH];
    wchar_t path[INTERNET_MAX_PATH_LENGTH];
    URL_COMPONENTSW components;
    memset(&components, 0, sizeof(components));
    components.dwStructSize = sizeof(components);
    components.lpszScheme = scheme;
    components.dwSchemeLength = sizeof(scheme) / sizeof(scheme[0]);
    components.lpszHostName = host;
    components.dwHostNameLength = sizeof(host) / sizeof(host[0]);
    components.lpszUrlPath = path;
    components.dwUrlPathLength = sizeof(path) / sizeof(path[0]);
    if(!::InternetCrackUrlW(url.c_str(), static_cast<DWORD>(url.size()),
        ICU_DECODE, &components))
    {
        return false;
    }
    bool secure = false;
    if(wcscmp(scheme, L"https") == 0)
    {
        secure = true;
    }
    else if(wcscmp(scheme, L"http") != 0)
    {
        return false; // 不支持非http https协议
    }

    CAutoInternetHandle internet(::InternetOpenW(kUserAgent,
        INTERNET_OPEN_TYPE_PRECONFIG,
        NULL, // proxy name
        NULL, // proxy bypass
        0));  // flags
    if(!internet.Get())
    {
        return false;
    }

    CAutoInternetHandle connection(::InternetConnectW(internet.Get(),
        host,
        components.nPort,
        NULL,   // user name
        NULL,   // password
        INTERNET_SERVICE_HTTP,
        0,      // flags
        NULL)); // context
    if(!connection.Get())
    {
        return false;
    }

    DWORD http_open_flags = secure ? INTERNET_FLAG_SECURE : 0;
    http_open_flags |= INTERNET_FLAG_NO_COOKIES;
    CAutoInternetHandle request(::HttpOpenRequestW(connection.Get(),
        L"POST",
        path,
        NULL,    // version
        NULL,    // referer
        NULL,    // agent type
        http_open_flags,
        NULL));  // context
    if(!request.Get())
    {
        return false;
    }

    std::wstring boundary = GenerateMultipartBoundary();
    std::wstring content_type_header = GenerateRequestHeader(boundary);
    ::HttpAddRequestHeadersW(request.Get(), content_type_header.c_str(),
        static_cast<DWORD>(-1), HTTP_ADDREQ_FLAG_ADD);

    std::string request_body;
    if(!GenerateRequestBody(parameters, upload_file,
        file_part_name, boundary, &request_body))
    {
        return false;
    }

    if(timeout)
    {
        if(!::InternetSetOptionW(request.Get(), INTERNET_OPTION_SEND_TIMEOUT,
            timeout, sizeof(timeout)))
        {
            fwprintf(stderr, L"Could not unset send timeout, continuing...\n");
        }

        if(!::InternetSetOptionW(request.Get(), INTERNET_OPTION_RECEIVE_TIMEOUT,
            timeout, sizeof(timeout)))
        {
            fwprintf(stderr, L"Could not unset receive timeout, continuing...\n");
        }
    }

    if(!::HttpSendRequestW(request.Get(), NULL, 0,
        const_cast<char*>(request_body.data()),
        static_cast<DWORD>(request_body.size())))
    {
        return false;
    }

    // The server indicates a successful upload with HTTP status 200.
    wchar_t http_status[4];
    DWORD http_status_size = sizeof(http_status);
    if(!::HttpQueryInfoW(request.Get(), HTTP_QUERY_STATUS_CODE,
        static_cast<LPVOID>(&http_status), &http_status_size, 0))
    {
        return false;
    }

    int http_response = wcstol(http_status, NULL, 10);
    if(response_code)
    {
        *response_code = http_response;
    }

    bool result = (http_response == 200);

    if(result)
    {
        result = ReadResponse(request.Get(), response_body);
    }

    return result;
}

// static
bool CHTTPUpload::ReadResponse(HINTERNET request, std::wstring* response)
{
    bool has_content_length_header = false;
    wchar_t content_length[32];
    DWORD content_length_size = sizeof(content_length);
    DWORD claimed_size = 0;
    std::string response_body;

    if(::HttpQueryInfoW(request, HTTP_QUERY_CONTENT_LENGTH,
        static_cast<LPVOID>(&content_length), &content_length_size, 0))
    {
        has_content_length_header = true;
        claimed_size = wcstol(content_length, NULL, 10);
        response_body.reserve(claimed_size);
    }


    DWORD bytes_available;
    DWORD total_read = 0;
    BOOL return_code;

    while(((return_code=::InternetQueryDataAvailable(request,
        &bytes_available, 0, 0)) != 0) && bytes_available>0)
    {
        std::vector<char> response_buffer(bytes_available);
        DWORD size_read;

        return_code = ::InternetReadFile(request,
            &response_buffer[0], bytes_available, &size_read);

        if(return_code && size_read > 0)
        {
            total_read += size_read;
            response_body.append(&response_buffer[0], size_read);
        }
        else
        {
            break;
        }
    }

    bool succeeded = return_code && (!has_content_length_header ||
        (total_read==claimed_size));
    if(succeeded && response)
    {
        *response = UTF8ToWide(response_body);
    }

    return succeeded;
}

// static
std::wstring CHTTPUpload::GenerateMultipartBoundary()
{
    // The boundary has 27 '-' characters followed by 16 hex digits
    static const wchar_t kBoundaryPrefix[] = L"---------------------------";
    static const int kBoundaryLength = 27 + 16 + 1;

    // Generate some random numbers to fill out the boundary
    int r0 = rand();
    int r1 = rand();

    wchar_t temp[kBoundaryLength];
    swprintf(temp, kBoundaryLength, L"%s%08X%08X", kBoundaryPrefix, r0, r1);

    // remove when VC++7.1 is no longer supported
    temp[kBoundaryLength-1] = L'\0';

    return std::wstring(temp);
}

// static
std::wstring CHTTPUpload::GenerateRequestHeader(const std::wstring& boundary)
{
    std::wstring header = L"Content-Type: multipart/form-data; boundary=";
    header += boundary;
    return header;
}

// static
bool CHTTPUpload::GenerateRequestBody(const std::map<std::wstring, std::wstring>& parameters,
                                      const std::wstring& upload_file,
                                      const std::wstring& file_part_name,
                                      const std::wstring& boundary,
                                      std::string* request_body)
{
    std::vector<char> contents;
    GetFileContents(upload_file, &contents);
    if(contents.empty())
    {
        return false;
    }

    std::string boundary_str = WideToUTF8(boundary);
    if(boundary_str.empty())
    {
        return false;
    }

    request_body->clear();

    // Append each of the parameter pairs as a form-data part
    for(std::map<std::wstring, std::wstring>::const_iterator pos=parameters.begin();
        pos!=parameters.end(); ++pos)
    {
        request_body->append("--" + boundary_str + "\r\n");
        request_body->append("Content-Disposition: form-data; name=\"" +
            WideToUTF8(pos->first) + "\"\r\n\r\n" +
            WideToUTF8(pos->second) + "\r\n");
    }

    // Now append the upload file as a binary (octet-stream) part
    std::string filename_utf8 = WideToUTF8(upload_file);
    if(filename_utf8.empty())
    {
        return false;
    }

    std::string file_part_name_utf8 = WideToUTF8(file_part_name);
    if(file_part_name_utf8.empty())
    {
        return false;
    }

    request_body->append("--" + boundary_str + "\r\n");
    request_body->append("Content-Disposition: form-data; "
        "name=\"" + file_part_name_utf8 + "\"; "
        "filename=\"" + filename_utf8 + "\"\r\n");
    request_body->append("Content-Type: application/octet-stream\r\n");
    request_body->append("\r\n");

    if(!contents.empty())
    {
        request_body->append(&(contents[0]), contents.size());
    }
    request_body->append("\r\n");
    request_body->append("--" + boundary_str + "--\r\n");
    return true;
}

// static
void CHTTPUpload::GetFileContents(const std::wstring& filename,
                                  std::vector<char>* contents)
{
    // The "open" method on pre-MSVC8 ifstream implementations doesn't accept a
    // wchar_t* filename, so use _wfopen directly in that case.  For VC8 and
    // later, _wfopen has been deprecated in favor of _wfopen_s, which does
    // not exist in earlier versions, so let the ifstream open the file itself.
#if _MSC_VER >= 1400  // MSVC 2005/8
    std::ifstream file;
    file.open(filename.c_str(), std::ios::binary);
#else  // _MSC_VER >= 1400
    ifstream file(_wfopen(filename.c_str(), L"rb"));
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

// static
bool CHTTPUpload::CheckParameters(const std::map<std::wstring, std::wstring>& parameters)
{
    for(std::map<std::wstring, std::wstring>::const_iterator pos=parameters.begin();
        pos!=parameters.end(); ++pos)
    {
        const std::wstring& str = pos->first;
        if(str.size() == 0)
        {
            return false; // disallow empty parameter names
        }
        for(unsigned int i=0; i<str.size(); ++i)
        {
            wchar_t c = str[i];
            if(c<32 || c=='"' || c>127)
            {
                return false;
            }
        }
    }
    return true;
}