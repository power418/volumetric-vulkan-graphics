#pragma once

/**
 * @file platform.h
 * @brief Platform-specific macro definitions and header inclusions for Windows, macOS, and Linux/Unix.
 */

// Platform detection and system header inclusions
#if defined(_WIN32) || defined(_WIN64)
#  ifndef UNICODE
#    define UNICODE
#  endif
#  ifndef _UNICODE
#    define _UNICODE
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include <consoleapi.h>
#elif defined(__APPLE__)
#  import <Foundation/Foundation.h>
#  import <Cocoa/Cocoa.h>
#elif defined(__unix__) || defined(__linux__)
#  include <X11/X.h>
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#  include <unistd.h>
#endif

// Vulkan platform configuration macros
#if (defined(_WIN32) || defined(_WIN64)) && !defined(VK_USE_PLATFORM_WIN32_KHR)
#  define VK_USE_PLATFORM_WIN32_KHR
#elif (defined(__unix__) || defined(__linux__)) && !defined(VK_USE_PLATFORM_XLIB_KHR)
#  define VK_USE_PLATFORM_XLIB_KHR
#endif
