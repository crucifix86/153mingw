// Test program for MultiPart PCK implementation
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "AngelicaFile/Header/AFilePackMan.h"
#include "AngelicaFile/Header/AFilePackageMultiPart.h"
#include "AngelicaFile/Header/AFI.h"

extern AFilePackMan g_AFilePackMan;

void TestDirectMultiPart(const char* pckFile)
{
    printf("\n=== Direct MultiPart Test: %s ===\n", pckFile);
    
    AFilePackageMultiPart multiPart;
    
    if (!multiPart.Open(pckFile, AFilePackage::OPENEXIST, false)) {
        printf("ERROR: Failed to open PCK file!\n");
        return;
    }
    
    printf("Successfully opened PCK file\n");
    printf("File count: %d\n", multiPart.GetFileNumber());
    printf("Package size: %u bytes\n", multiPart.GetPackageFileSize());
    
    // List first 10 files
    printf("\nFirst 10 files:\n");
    for (int i = 0; i < min(10, multiPart.GetFileNumber()); i++) {
        const AFilePackage::FILEENTRY* pEntry = multiPart.GetFileEntryByIndex(i);
        if (pEntry) {
            printf("  [%d] %s - Size: %u bytes, Compressed: %u bytes\n", 
                i, pEntry->szFileName, pEntry->dwLength, pEntry->dwCompressedLength);
        }
    }
    
    multiPart.Close();
}

void TestFilePackMan(const char* pckFile)
{
    printf("\n=== AFilePackMan Test: %s ===\n", pckFile);
    
    // Disable WinPCK backend to force MultiPart detection
    g_AFilePackMan.UseWinPCKBackend(false);
    
    // Initialize
    af_Initialize();
    
    // Open PCK file
    if (!g_AFilePackMan.OpenFilePackageInGame(pckFile, false)) {
        printf("ERROR: Failed to open PCK file through AFilePackMan!\n");
        return;
    }
    
    printf("Successfully opened PCK file through AFilePackMan\n");
    
    // Try to read a test file
    const char* testFiles[] = {
        "models\\test.ecm",
        "litmodels\\test.ecm",
        "gfx\\test.gfx",
        "sfx\\test.ogg",
        NULL
    };
    
    for (int i = 0; testFiles[i]; i++) {
        AFile file;
        if (file.Open(testFiles[i], AFILE_OPENEXIST | AFILE_TEMPMEMORY)) {
            DWORD size = file.GetFileLength();
            printf("Found: %s - Size: %u bytes\n", testFiles[i], size);
            file.Close();
        }
    }
    
    // Close all packages
    g_AFilePackMan.CloseAllPackages();
}

int main(int argc, char* argv[])
{
    printf("MultiPart PCK Final Test\n");
    printf("========================\n");
    
    if (argc < 2) {
        printf("Usage: %s <pck_file>\n", argv[0]);
        printf("Example: %s litmodels.pck\n", argv[0]);
        return 1;
    }
    
    const char* pckFile = argv[1];
    
    // Test direct MultiPart class
    TestDirectMultiPart(pckFile);
    
    // Test through AFilePackMan
    TestFilePackMan(pckFile);
    
    printf("\nAll tests completed.\n");
    
    return 0;
}