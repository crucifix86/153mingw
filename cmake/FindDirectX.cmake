# FindDirectX.cmake
# Finds DirectX libraries for MinGW cross-compilation
#
# Sets:
#   DIRECTX_FOUND
#   DIRECTX_INCLUDE_DIRS
#   DIRECTX_LIBRARIES

set(DIRECTX_FOUND TRUE)

# MinGW includes DirectX headers in its standard include path
# No special include directory needed

# Core DirectX libraries
set(DIRECTX_D3D9_LIBRARY d3d9)
set(DIRECTX_D3DX9_LIBRARY d3dx9_43)
set(DIRECTX_DDRAW_LIBRARY ddraw)
set(DIRECTX_DSOUND_LIBRARY dsound)
set(DIRECTX_DINPUT_LIBRARY dinput8)
set(DIRECTX_DXGUID_LIBRARY dxguid)

set(DIRECTX_LIBRARIES
    ${DIRECTX_D3D9_LIBRARY}
    ${DIRECTX_D3DX9_LIBRARY}
    ${DIRECTX_DDRAW_LIBRARY}
    ${DIRECTX_DSOUND_LIBRARY}
    ${DIRECTX_DINPUT_LIBRARY}
    ${DIRECTX_DXGUID_LIBRARY}
)

# Additional Windows libraries commonly needed
set(WIN32_LIBRARIES
    ws2_32      # Winsock
    winmm       # Windows multimedia
    imm32       # Input Method Manager
    version     # Version info
    comctl32    # Common controls
    comdlg32    # Common dialogs
    ole32       # OLE
    oleaut32    # OLE Automation
    uuid        # UUIDs
    gdi32       # GDI
    user32      # User32
    kernel32    # Kernel32
    advapi32    # Advanced API
    shell32     # Shell
    shlwapi     # Shell lightweight API
)

message(STATUS "DirectX SDK found (MinGW built-in)")
