/**
 * @file logger.h
 * @brief Lightweight logging primitives for engine and application diagnostics.
 *
 * This header defines the shared foundation for runtime logging: severity
 * levels, standard library feature detection, and common types used by the
 * logging implementation. The intended logging model is a small, deterministic
 * pipeline that converts an event at the call site into a formatted record sent
 * to one or more output sinks.
 *
 * Logging pipeline:
 *
 * | Stage | Responsibility |
 * | ----- | -------------- |
 * | Emit | A call site submits a message with an associated severity level. |
 * | Filter | The active threshold determines whether the message is recorded. |
 * | Enrich | Metadata such as timestamp, source file, line, and function may be attached. |
 * | Format | The log event is converted into a stable text representation. |
 * | Dispatch | The formatted record is written to console, file, debugger output, or another sink. |
 * | Synchronize | Shared sinks are guarded so concurrent writes do not corrupt a log line. |
 *
 * Severity levels are ordered from most verbose to most restrictive. A logger
 * can use this ordering to suppress records below the currently configured
 * threshold.
 *
 * | Level | Value | Scope | Typical use |
 * | ----- | ----- | ----- | ----------- |
 * | trace | 0 | Highly detailed execution flow. | Per-step diagnostics, allocation details, frame-level state. |
 * | debug | 1 | Development-time state inspection. | Internal counters, selected formats, temporary diagnostic values. |
 * | info  | 2 | Normal operational milestones. | Startup, shutdown, subsystem initialization, asset loading. |
 * | warn  | 3 | Recoverable abnormal conditions. | Fallback paths, degraded configuration, optional feature failure. |
 * | err   | 4 | Failed operations with possible recovery. | Resource load failure, API call failure, invalid runtime state. |
 * | crit  | 5 | Fatal or unrecoverable conditions. | Device creation failure, corrupted required state, forced shutdown. |
 * | off   | 6 | Logging disabled. | Suppresses all log output. |
 *
 * Expected output format:
 *
 *   [HH:MM:SS][level][path/file.cpp:line] message
 *
 * Design notes:
 *
 * - Public logging entry points should preserve source location automatically
 *   when compiler support is available.
 * - Log sinks should be replaceable so console, file, and platform debugger
 *   output can be enabled independently.
 * - The logging path should avoid throwing exceptions during normal operation.
 *   If a sink fails, the implementation should degrade gracefully.
 * - Formatting should remain stable enough for manual inspection and simple
 *   external tooling.
 * - The logger should remain lightweight enough to include across engine modules
 *   without introducing ownership or initialization complexity.
 */

#pragma once

#include <cstdio>
#include <cctype>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <stdexcept>

#if defined (__unix__) || defined (__linux__) || defined (__APPLE__)
#include <unistd.h>
#endif
#if defined(__has_include)
# if __has_include(<source_location>)
#   include <source_location>
#   define VK_LOG_HAS_STD_SOURCE_LOCATION 1
# endif
#endif

#ifndef VK_LOG_HAS_STD_SOURCE_LOCATION
# define  VKVK_LOG_HAS_STD_SOURCE_LOCATION 0
#endif

#ifndef __has_builtin
# define __has__has_builtin(x) 0
#endif

// ANSI Escape Codes untuk mewarnai output terminal (Unix/Linux/macOS & Windows 10+)
#define CONSOLE_COLOR_RESET   "\033[0m"
#define CONSOLE_COLOR_BLACK   "\033[30m"
#define CONSOLE_COLOR_RED     "\033[31m"
#define CONSOLE_COLOR_GREEN   "\033[32m"
#define CONSOLE_COLOR_YELLOW  "\033[33m"
#define CONSOLE_COLOR_BLUE    "\033[34m"
#define CONSOLE_COLOR_MAGENTA "\033[35m"
#define CONSOLE_COLOR_CYAN    "\033[36m"
#define CONSOLE_COLOR_WHITE   "\033[37m"

// Bold Colors
#define CONSOLE_COLOR_BOLD_RED    "\033[1;31m"
#define CONSOLE_COLOR_BOLD_GREEN  "\033[1;32m"
#define CONSOLE_COLOR_BOLD_YELLOW "\033[1;33m"
#define CONSOLE_COLOR_BOLD_BLUE   "\033[1;34m"

// Background Colors
#define CONSOLE_BG_RED        "\033[41m"
#define CONSOLE_BG_GREEN      "\033[42m"
#define CONSOLE_BG_YELLOW     "\033[43m"
#define CONSOLE_BG_BLUE       "\033[44m"

/**
 * @brief Ordered severity values used to classify and filter log records.
 *
 * The numeric order is intentional. Lower values represent more verbose
 * diagnostics, while higher values represent increasingly severe conditions.
 * A logger can emit a record when its level is greater than or equal to the
 * active threshold.
 *
 * Example threshold rule:
 *
 *   emit = (message_level >= active_level) && (active_level != log_level::off)
 *
 * These values are intended for local filtering and classification. They should
 * not be treated as a serialized ABI unless a stable external log format is
 * explicitly defined.
 */
enum class log_level {
  trace = 0,
  debug = 1,
  info  = 2,
  warn  = 3,
  err   = 4,
  crit  = 5,
  off   = 6, 
};

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

  // Enable Virtual Terminal Processing for ANSI colors
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut != INVALID_HANDLE_VALUE)
  {
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode))
    {
      dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
      SetConsoleMode(hOut, dwMode);
    }
  }

  std::cout << "debug console opened successfully!" << "\n";
  return true;
#elif defined (__unix__) || defined (__linux__) || defined (__APPLE__)
  if constexpr (mode == ConsoleMode::AttachParent)
  {
    if (isatty(STDOUT_FILENO))
    {
      std::cout << "debug console attached!\n";
      return true;
    }
    FILE* f_out = freopen("/dev/tty", "w", stdout);
    FILE* f_err = freopen("/dev/tty", "w", stderr);
    FILE* f_in  = freopen("/dev/tty", "r", stdin);

    if (f_out && f_err && f_in)
    {
      std::cout.clear(); std::cin.clear(); std::cerr.clear();
      std::cout << "debug console attached to tty!\n";
      return true;
    }
    return false;
  }
  else
  {
    #if defined(__APPLE__)
    // macOS: Use AppleScript to open Terminal.app and tail a temporary log file
    const char* log_file = "/tmp/vk_volumetric_debug.log";
    
    // Clear old log file
    FILE* clear_log = fopen(log_file, "w");
    if (clear_log) fclose(clear_log);
    
    // Tell Terminal.app to open a new window and tail the log
    (void)system("osascript -e 'tell app \"Terminal\" to do script \"clear && echo \\\"Volumetric Vulkan Debug Console\\\" && tail -f /tmp/vk_volumetric_debug.log\"'");
    
    // Redirect our output to the log file
    FILE* f_out = freopen(log_file, "a", stdout);
    FILE* f_err = freopen(log_file, "a", stderr);
    
    if (f_out && f_err) {
      setvbuf(stdout, NULL, _IOLBF, 0);
      setvbuf(stderr, NULL, _IOLBF, 0);
      std::cout.clear(); std::cerr.clear();
      std::cout << std::unitbuf; std::cerr << std::unitbuf;
      std::cout << "macOS debug console opened successfully!\n";
      return true;
    }
    return false;
    #else
    // Linux/Other Unix: AllocNew using fork, pipe, and xterm
    int pipefd[2];
    if (pipe(pipefd) == -1) return false;

    pid_t pid = fork();
    if (pid == -1) return false;

    if (pid == 0) {
      // Child process: Redirect stdin to pipe, then spawn terminal running 'cat'
      close(pipefd[1]);
      dup2(pipefd[0], STDIN_FILENO);

      // Try xterm first, standard on almost all Unix systems
      execlp("xterm", "xterm", "-T", "Volumetric Vulkan Debug Console", "-bg", "black", "-fg", "white", "-e", "cat", NULL);
      // Fallback to gnome-terminal
      execlp("gnome-terminal", "gnome-terminal", "--title=Debug Console", "--", "cat", NULL);
      // Fallback to konsole
      execlp("konsole", "konsole", "-e", "cat", NULL);
      
      exit(1); // Exit if no terminal emulator is found
    } else {
      // Parent process: Redirect our stdout/stderr to the pipe
      close(pipefd[0]);
      dup2(pipefd[1], STDOUT_FILENO);
      dup2(pipefd[1], STDERR_FILENO);
      
      // Make C-style outputs line buffered
      setvbuf(stdout, NULL, _IOLBF, 0);
      setvbuf(stderr, NULL, _IOLBF, 0);

      std::cout.clear();
      std::cerr.clear();
      std::cout << std::unitbuf;
      std::cerr << std::unitbuf;
      
      std::cout << "Unix debug console opened successfully (spawned new terminal)!\n";
      return true;
    }
    #endif
  }
#else
  return false;
#endif
}

/// Short alias for APIs that prefer the `Level` name.
using Level = log_level;

#ifndef NDEBUG
    inline std::string GetLogTimestamp(const char* color) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "[" << color << std::put_time(std::localtime(&in_time_t), "%H:%M:%S") << CONSOLE_COLOR_RESET << "]";
        return ss.str();
    }

    #define WR_LOG_TRACE(msg) do { std::cout << GetLogTimestamp(CONSOLE_COLOR_WHITE)    << "[" << CONSOLE_COLOR_WHITE    << "TRACE" << CONSOLE_COLOR_RESET << "] " << msg << "\n"; std::cout.flush(); } while(0)
    #define WR_LOG_DEBUG(msg) do { std::cout << GetLogTimestamp(CONSOLE_COLOR_CYAN)     << "[" << CONSOLE_COLOR_CYAN     << "DEBUG" << CONSOLE_COLOR_RESET << "] " << msg << "\n"; std::cout.flush(); } while(0)
    #define WR_LOG_INFO(msg)  do { std::cout << GetLogTimestamp(CONSOLE_COLOR_GREEN)    << "[" << CONSOLE_COLOR_GREEN    << "INFO"  << CONSOLE_COLOR_RESET << "]  " << msg << "\n"; std::cout.flush(); } while(0)
    #define WR_LOG_WARN(msg)  do { std::cout << GetLogTimestamp(CONSOLE_COLOR_YELLOW)   << "[" << CONSOLE_COLOR_YELLOW   << "WARN"  << CONSOLE_COLOR_RESET << "]  " << msg << "\n"; std::cout.flush(); } while(0)
    #define WR_LOG_ERR(msg)   do { std::cerr << GetLogTimestamp(CONSOLE_COLOR_RED)      << "[" << CONSOLE_COLOR_RED      << "ERROR" << CONSOLE_COLOR_RESET << "] " << msg << "\n"; std::cerr.flush(); } while(0)
    #define WR_LOG_CRIT(msg)  do { std::cerr << GetLogTimestamp(CONSOLE_COLOR_BOLD_RED) << "[" << CONSOLE_COLOR_BOLD_RED << "CRIT"  << CONSOLE_COLOR_RESET << "]  " << msg << "\n"; std::cerr.flush(); } while(0)
#else
    #define WR_LOG_TRACE(msg) do {} while(0)
    #define WR_LOG_DEBUG(msg) do {} while(0)
    #define WR_LOG_INFO(msg)  do {} while(0)
    #define WR_LOG_WARN(msg)  do {} while(0)
    #define WR_LOG_ERR(msg)   do {} while(0)
    #define WR_LOG_CRIT(msg)  do {} while(0)
#endif

// --- Assert & Exception Macros ---

#ifndef NDEBUG
    #define WR_ASSERT(condition, msg) \
        do { \
            if (!(condition)) { \
                WR_LOG_CRIT("Assertion Failed: " << #condition << " | " << msg); \
                assert(condition); \
            } \
        } while(0)
#else
    #define WR_ASSERT(condition, msg) do {} while(0)
#endif

#define WR_THROW(msg) \
    do { \
        std::stringstream __wr_throw_ss; \
        __wr_throw_ss << msg; \
        std::string __wr_throw_str = __wr_throw_ss.str(); \
        WR_LOG_ERR(__wr_throw_str); \
        throw std::runtime_error(__wr_throw_str); \
    } while(0)

