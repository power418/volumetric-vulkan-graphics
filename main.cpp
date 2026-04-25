#if defined (_WIN32) || defined (_WIN64)
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
#include <windows.h>
#include <consoleapi.h>
#elif defined (__APPLE__)
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#elif defined (__unix__) || defined (__linux__)
#include <X11/X11.h>
#include <unistd.h>
#endif

#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#if (defined (_WIN32) || defined (_WIN64)) && !defined (VK_USE_PLATFORM_WIN32_KHR)
#  define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

#include <constant/shell.h>

enum class ConsoleMode 
{
  AllocNew,
  AttachParent,
};

template<ConsoleMode mode =  ConsoleMode::AllocNew>
bool OpenConsole()
{
#if defined (_WIN32) || defined (_WIN64)
  constexpr const char* ConsoleOutputFile = "CONOUT$";
  constexpr const char* ConsoleInputFile  = "CONIN$";

  BOOL ok = FALSE;

  if constexpr (mode == ConsoleMode::AttachParent)
  {
    ok = AttachConsole(ATTACH_PARENT_PROCESS);
  }
  else
  {
    ok = AllocConsole();
  }

  if(!ok)
    return false; 

  static std::ofstream console_out;
  static std::ifstream console_input;
  static std::ofstream console_error;

  console_out.open(ConsoleOutputFile);
  console_input.open(ConsoleInputFile);
  console_error.open(ConsoleOutputFile);

  if (!console_out.is_open() || !console_input.is_open() || !console_error.is_open())
  {
    FreeConsole();
    return false;
  }

  std::cout.rdbuf(console_out.rdbuf());
  std::cin.rdbuf(console_input.rdbuf());
  std::cerr.rdbuf(console_error.rdbuf());

  std::cout.clear();
  std::cin.clear();
  std::cerr.clear();

  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "debug console opened successfully!" << "\n";
  return true;
#endif
  return false;
}

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
  std::cout << "Hello, World!" << "\n";
  return 0;
}
#endif
