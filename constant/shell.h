#pragma once

#if defined (_WIN32) || defined (_WIN64)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  ifndef WINDOWS_TARGET_VERSION
#    define WINDOWS_TARGET_VERSION 0x0A00
#  endif
#  ifndef WINVER
#    define WINVER WINDOWS_TARGET_VERSION
#  endif
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT WINDOWS_TARGET_VERSION
#  endif

#  include <windows.h>

#  define RPC_WINDOWS_COMMON_CONTROLS_MANIFEST \
  "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " \
  "version='6.0.0.0' processorArchitecture='*' " \
  "publicKeyToken='6595b64144ccf1df' language='*'\""

#  pragma comment(linker, RPC_WINDOWS_COMMON_CONTROLS_MANIFEST)
#endif
