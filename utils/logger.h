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

  std::cout << "debug console opened successfully!" << "\n";
  return true;
#elif defined (__unix__) || defined (__linux__) || defined (__APPLE__)
  if (isatty(STDOUT_FILENO))
  {
    std::cout << "debug console opened successfully!\n";
    return true;
  }

  FILE* f_out = freopen("/dev/tty", "w", stdout);
  FILE* f_err = freopen("/dev/tty", "w", stderr);
  FILE* f_in  = freopen("/dev/tty", "r", stdin);

  if (f_out && f_err && f_in)
  {
    std::cout.clear();
    std::cin.clear();
    std::cerr.clear();
    std::cout << "debug console opened successfully!\n";
    return true;
  }
  return false;
#else
  return false;
#endif
}

/// Short alias for APIs that prefer the `Level` name.
using Level = log_level;

#ifndef NDEBUG
    #define WR_LOG_INFO(msg) do { std::cout << msg << "\n"; std::cout.flush(); } while(0)
#else
    #define WR_LOG_INFO(msg) do {} while(0)
#endif
