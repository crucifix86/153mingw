#define _WIN32_DCOM
#include <iostream>

using namespace std;
#include <comdef.h>
#include <wbemidl.h>

#include <codecvt>
#pragma comment(lib, "wbemuuid.lib")

class HwLoader {
public:
	HwLoader();
	virtual ~HwLoader();
	string getDeviceFingerPrint();

};