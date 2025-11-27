/*
 * FILE: BolaDebug.h
 *
 * DESCRIPTION: Comprehensive debug logging system for PWI client
 * Hooks into Angelica logging and provides function tracing
 *
 * CREATED BY: BOLA Debug Team, 2025
 */

#ifndef _BOLADEBUG_H_
#define _BOLADEBUG_H_

#include "ACPlatform.h"
#include <stdio.h>
#include <stdarg.h>

// Enable all debug logging
#define BOLA_DEBUG_ENABLED 1

// Debug log levels
#define BOLA_LOG_TRACE    0
#define BOLA_LOG_DEBUG    1
#define BOLA_LOG_INFO     2
#define BOLA_LOG_WARNING  3
#define BOLA_LOG_ERROR    4
#define BOLA_LOG_FATAL    5

// Current minimum log level (0 = everything, 5 = only fatal)
#define BOLA_LOG_LEVEL    0

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the BOLA debug system - call this FIRST in main/WinMain
void BolaDebug_Init(const char* szLogDir);

// Shutdown the debug system
void BolaDebug_Shutdown();

// Core logging function
void BolaDebug_Log(int level, const char* file, int line, const char* func, const char* fmt, ...);

// Function entry/exit tracking
void BolaDebug_FuncEnter(const char* file, int line, const char* func);
void BolaDebug_FuncExit(const char* file, int line, const char* func);

// Flush the log file (call before potential crash points)
void BolaDebug_Flush();

// Get current trace depth (for indentation)
int BolaDebug_GetDepth();

#ifdef __cplusplus
}
#endif

// Convenience macros
#if BOLA_DEBUG_ENABLED

// Basic logging macros
#define BOLA_TRACE(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_TRACE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define BOLA_DEBUG(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define BOLA_INFO(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define BOLA_WARN(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define BOLA_ERROR(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define BOLA_FATAL(fmt, ...) \
    BolaDebug_Log(BOLA_LOG_FATAL, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

// Function tracing - use BOLA_FUNC_ENTER at start of function
#define BOLA_FUNC_ENTER() \
    BolaDebug_FuncEnter(__FILE__, __LINE__, __FUNCTION__)

#define BOLA_FUNC_EXIT() \
    BolaDebug_FuncExit(__FILE__, __LINE__, __FUNCTION__)

// RAII-style function tracer for C++
#ifdef __cplusplus
class BolaFuncTracer {
public:
    BolaFuncTracer(const char* file, int line, const char* func)
        : m_file(file), m_line(line), m_func(func) {
        BolaDebug_FuncEnter(file, line, func);
    }
    ~BolaFuncTracer() {
        BolaDebug_FuncExit(m_file, m_line, m_func);
    }
private:
    const char* m_file;
    int m_line;
    const char* m_func;
};

// Use this at the start of a function - will auto-log exit
#define BOLA_FUNC() \
    BolaFuncTracer _bola_tracer(__FILE__, __LINE__, __FUNCTION__)
#endif

// Flush before risky operations
#define BOLA_CHECKPOINT(msg) do { \
    BolaDebug_Log(BOLA_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, "CHECKPOINT: %s", msg); \
    BolaDebug_Flush(); \
} while(0)

// Assert with logging
#define BOLA_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        BolaDebug_Log(BOLA_LOG_FATAL, __FILE__, __LINE__, __FUNCTION__, "ASSERT FAILED: %s - %s", #cond, msg); \
        BolaDebug_Flush(); \
    } \
} while(0)

#else
// Disabled - no-op macros
#define BOLA_TRACE(fmt, ...)
#define BOLA_DEBUG(fmt, ...)
#define BOLA_INFO(fmt, ...)
#define BOLA_WARN(fmt, ...)
#define BOLA_ERROR(fmt, ...)
#define BOLA_FATAL(fmt, ...)
#define BOLA_FUNC_ENTER()
#define BOLA_FUNC_EXIT()
#define BOLA_FUNC()
#define BOLA_CHECKPOINT(msg)
#define BOLA_ASSERT(cond, msg)
#endif

#endif // _BOLADEBUG_H_
