/**
 *
 *
 * 
 *
 * 
 *
 *
 * */

#include <cstdlib>
#include <cstdio>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <platform/platform.h>
#include <vulkan/vulkan.h>

#include <constant/shell.h>
#include <utils/logger.h>

/**
 *
 *
 * */
#if defined(_WIN32)
int WINAPI WinMain (HINSTANCE hInst, 
                    HINSTANCE hPrevInst, 
                    PSTR nCmd,
                    INT showCmd) 
{
#if defined (WR_DEBUG_CONSOLE)
  OpenConsole<ConsoleMode::AllocNew>();
  std::cout << "program is running successfully!\n";
  std::cout.flush();
#endif
  return MessageBoxW(NULL, L"Wellow", L"Wello World!", MB_OKCANCEL);
}
#else
int main(int argc, char **argv) {
#if defined (WR_DEBUG_CONSOLE)
  OpenConsole<ConsoleMode::AllocNew>();
  std::cout << "program is running successfully!\n";
  std::cout.flush();
#endif

  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    std::cerr << "Cannot open X11 display\n";
    return 1;
  }

  int screen = DefaultScreen(display);
  Window root = RootWindow(display, screen);

  Window window = XCreateSimpleWindow(display, root, 10, 10, 800, 600, 1,
                                      BlackPixel(display, screen), WhitePixel(display, screen));

  XStoreName(display, window, "Volumetric Vulkan Graphics");

  XSelectInput(display, window, ExposureMask | KeyPressMask | StructureNotifyMask);
  XMapWindow(display, window);

  Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wmDeleteMessage, 1);

  bool running = true;
  XEvent event;

  std::cout << "X11 window created successfully. Press any key or close the window to exit.\n";

  while (running) {
    XNextEvent(display, &event);

    switch (event.type) {
      case KeyPress:
        running = false;
        break;
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
