#pragma once

/**
 * @file platform.h
 * @brief Platform-specific macro definitions and header inclusions for Windows, macOS, and Linux/Unix.
 */

// Platform detection and system header inclusions
#if defined(_WIN32) || defined(_WIN64)
#  include <windows.h>
#  include <consoleapi.h>
#  define PLATFORM_COLOR_WHITE RGB(255, 255, 255)
#elif defined(__APPLE__)
#  import <Foundation/Foundation.h>
#  import <Cocoa/Cocoa.h>
#  define PLATFORM_COLOR_WHITE [NSColor whiteColor]
#elif defined(__unix__) || defined(__linux__)
#  include <X11/X.h>
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  include <unistd.h>
#  define PLATFORM_COLOR_WHITE 0xFFFFFF
#endif

#include <fonts.h>
