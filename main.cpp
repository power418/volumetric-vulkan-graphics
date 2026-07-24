/*
  simulation volumetric engine -- Cross-platform Volumetric Vulkan Graphics
  July 2026

  Copyright (C) 2026 power418

  Hello! This software is provided 'as-is' for personal, educational, and 
  open-source purposes. The author won't be held liable for any damages 
  arising from the use of this code.

  You are totally free to use, modify, and share this software, subject to 
  a few sensible restrictions:

  1. Commercial sale of this software is strictly prohibited. This project 
     uses (or will use) GPL-licensed components, so keeping it free and 
     open is a must.
  2. Please don't claim you wrote the original software. If you use it, 
     a little shout-out or acknowledgment would be highly appreciated!
  3. If you modify the source code, please mark it clearly so people know 
     it's not the original version.
  4. Don't remove or alter this notice from the source distribution.

  Happy coding!
*/

#include <cstdlib>
#include <cstdio>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <vulkan/vulkan.h>

#include <platform/constant/shell.h>
#include <platform/target.h>
#include <utils/logger.h>

/**
 * @file main.cpp
 * @brief Application Entry Point & Cross-Platform Window Creation
 *
 * This file serves as the main entry point for the Volumetric Vulkan Graphics application.
 * It contains the platform-specific windowing setup and event loop implementation.
 * 
 * To ensure the application runs natively across different operating systems without 
 * relying on external windowing libraries (like GLFW or SDL), this file uses conditional 
 * compilation (macros) to switch between different OS APIs:
 *
 * - `_WIN32` / `_WIN64` : Compiles the Windows (Win32 API) implementation. Uses `WinMain`, `HWND`, and `WNDCLASSEXW`.
 * - `__APPLE__`         : Compiles the macOS (Cocoa) implementation. Uses Objective-C (`NSApplication`, `AppDelegate`).
 * - `#else` (Unix/X11)  : Fallback for Linux and other Unix-like systems. Uses raw X11/Xlib (`Display*`, `Window`).
 */

// ==============================================================================
// WIN32 / WINDOWS IMPLEMENTATION
// ==============================================================================
#if defined(_WIN32) || defined(_WIN64)

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CLOSE:
      DestroyWindow(hwnd);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }
}

int WINAPI WinMain (HINSTANCE hInst, 
                    HINSTANCE hPrevInst, 
                    PSTR nCmd,
                    INT showCmd) 
{
#if defined (WR_DEBUG_CONSOLE)
  OpenConsole<ConsoleMode::AllocNew>();
  WR_LOG_INFO("Windows debug console is running!");
#endif

  const wchar_t CLASS_NAME[] = L"VolumetricVulkanClass";

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInst;
  wc.lpszClassName = CLASS_NAME;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = CreateSolidBrush(PLATFORM_COLOR_WHITE);

  RegisterClassExW(&wc);

  HWND hwnd = CreateWindowExW(
      0,                              // Optional window styles
      CLASS_NAME,                     // Window class
      L"Volumetric Vulkan Graphics",  // Window text
      WS_OVERLAPPEDWINDOW,            // Window style
      CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, // Size and position
      NULL,                           // Parent window
      NULL,                           // Menu
      hInst,                          // Instance handle
      NULL                            // Additional application data
  );

  if (hwnd == NULL) {
    std::cerr << "[ERROR] Failed to create Win32 window!\n";
    return 0;
  }

  ShowWindow(hwnd, showCmd);

  WR_LOG_INFO("Win32 window created successfully. Close the window to exit.");

  MSG msg = {};
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  WR_LOG_INFO("Window ditutup. Menghentikan aplikasi...");

  return 0;
}
// ==============================================================================
// MACOS (COCOA) IMPLEMENTATION
// ==============================================================================
#elif defined(__APPLE__)

// --- Objective-C Class Interface untuk App Delegate ---
@interface AppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *_window;
}
@end

// --- Objective-C Class Implementation ---
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    WR_LOG_INFO("Aplikasi berjalan! Membuat window Cocoa...");

    NSRect frame = NSMakeRect(0, 0, 800, 600);
    NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;

    _window = [[NSWindow alloc] initWithContentRect:frame
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    [_window setBackgroundColor:PLATFORM_COLOR_WHITE];

    [_window setTitle:@"Volumetric Vulkan Graphics"];
    [_window center];
    [_window makeKeyAndOrderFront:nil];
    
    WR_LOG_INFO("Cocoa window created successfully.");
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(id)sender {
    return YES;
}

@end

int main(int argc, const char * argv[]) {
    WR_LOG_INFO("Inisialisasi NSApplication...");
    @autoreleasepool {
        id app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    WR_LOG_INFO("Window ditutup. Menghentikan aplikasi...");
    return 0;
}

// ==============================================================================
// LINUX / UNIX (X11) IMPLEMENTATION
// ==============================================================================
#else
int main(int argc, char **argv) {
#if defined (WR_DEBUG_CONSOLE)
  OpenConsole<ConsoleMode::AllocNew>();
  WR_LOG_INFO("program is running successfully!");
#endif

  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    std::cerr << "Cannot open X11 display\n";
    return 1;
  }

  int screen = DefaultScreen(display);
  Window root = RootWindow(display, screen);

  Window window = XCreateSimpleWindow(display, root, 10, 10, 800, 600, 1,
                                      BlackPixel(display, screen), PLATFORM_COLOR_WHITE);

  XStoreName(display, window, "Volumetric Vulkan Graphics");

  XSelectInput(display, window, ExposureMask | StructureNotifyMask);
  XMapWindow(display, window);

  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wmDeleteMessage, 1);

  bool running = true;
  XEvent event;

  WR_LOG_INFO("X11 window created successfully. Close the window to exit.");

  while (running) {
    XNextEvent(display, &event);

    switch (event.type) {
      case ClientMessage:
        if (static_cast<Atom>(event.xclient.data.l[0]) == wmDeleteMessage) {
          running = false;
        }
        break;
      case DestroyNotify:
        running = false;
        break;
    }
  }

  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
#endif
