
#ifndef __http_upload_h__
#define __http_upload_h__

#include <map>
#include <string>
#include <vector>
#include <Windows.h>
#include <WinInet.h>

// CHTTPUpload provides a "nice" API to send a multipart HTTP(S) POST
// request using wininet.  It currently supports requests that contain
// a set of string parameters (key/value pairs), and a file to upload.
class CHTTPUpload
{
public:
    // Sends the given set of parameters, along with the contents of
    // upload_file, as a multipart POST request to the given URL.
    // file_part_name contains the name of the file part of the request
    // (i.e. it corresponds to the name= attribute on an <input type="file">.
    // Parameter names must contain only printable ASCII characters,
    // and may not contain a quote (") character.
    // Only HTTP(S) URLs are currently supported.  Returns true on success.
    // If the request is successful and response_body is non-NULL,
    // the response body will be returned in response_body.
    // If response_code is non-NULL, it will be set to the HTTP response code
    // received (or 0 if the request failed before getting an HTTP response).
    static bool SendRequest(const std::wstring& url,
        const std::map<std::wstring, std::wstring>& parameters,
        const std::wstring& upload_file, const std::wstring& file_part_name,
        int* timeout, std::wstring* response_body, int* response_code);

private:
    class CAutoInternetHandle;

    // Retrieves the HTTP response.  If NULL is passed in for response,
    // this merely checks (via the return value) that we were successfully
    // able to retrieve exactly as many bytes of content in the response as
    // were specified in the Content-Length header.
    static bool CHTTPUpload::ReadResponse(HINTERNET request, std::wstring* response);

    // Generates a new multipart boundary for a POST request
    static std::wstring GenerateMultipartBoundary();

    // Generates a HTTP request header for a multipart form submit.
    static std::wstring GenerateRequestHeader(const std::wstring& boundary);

    // Given a set of parameters, an upload filename, and a file part name,
    // generates a multipart request body string with these parameters
    // and minidump contents.  Returns true on success.
    static bool GenerateRequestBody(
        const std::map<std::wstring, std::wstring>& parameters,
        const std::wstring& upload_file, const std::wstring& file_part_name,
        const std::wstring& boundary, std::string* request_body);

    // Fills the supplied vector with the contents of filename.
    static void GetFileContents(const std::wstring& filename,
        std::vector<char>* contents);

    // Checks that the given list of parameters has only printable
    // ASCII characters in the parameter name, and does not contain
    // any quote (") characters.  Returns true if so.
    static bool CheckParameters(const std::map<std::wstring, std::wstring>& parameters);

    // No instances of this class should be created.
    // Disallow all constructors, destructors, and operator=.
    CHTTPUpload();
    explicit CHTTPUpload(const CHTTPUpload&);
    void operator=(const CHTTPUpload&);
    ~CHTTPUpload();
};

#endif //__http_upload_h__