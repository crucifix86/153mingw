/*
 * FILE: ADebugLog.h
 *
 * DESCRIPTION: Comprehensive debug logging system for Angelica Engine
 *              Works from very early startup, before other systems initialize
 *
 * CREATED BY: Debug System, 2024
 *
 * USAGE:
 *   ALOG_DEBUG("message %s", arg);   // Debug level (most verbose)
 *   ALOG_INFO("message %s", arg);    // Info level
 *   ALOG_WARN("message %s", arg);    // Warning level
 *   ALOG_ERROR("message %s", arg);   // Error level (least verbose)
 *
 * CONFIGURATION:
 *   Set ADEBUG_LOG_LEVEL to control verbosity (0=all, 1=info+, 2=warn+, 3=error only)
 *   Set ADEBUG_LOG_ENABLED to 0 to disable all logging
 */

#ifndef _ADEBUGLOG_H_
#define _ADEBUGLOG_H_

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

// Configuration - can be overridden in CMakeLists.txt or before including this header
#ifndef ADEBUG_LOG_ENABLED
#define ADEBUG_LOG_ENABLED 1
#endif

#ifndef ADEBUG_LOG_LEVEL
#define ADEBUG_LOG_LEVEL 0  // 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR
#endif

#ifndef ADEBUG_LOG_FILE
#define ADEBUG_LOG_FILE "logs/debug.log"
#endif

// Log levels
#define ALOG_LEVEL_DEBUG 0
#define ALOG_LEVEL_INFO  1
#define ALOG_LEVEL_WARN  2
#define ALOG_LEVEL_ERROR 3

// Internal implementation
#ifdef __cplusplus
extern "C" {
#endif

// Get singleton file handle - creates file on first call
static inline FILE* _ADebugLog_GetFile()
{
    static FILE* s_pLogFile = NULL;
    static int s_bTriedOpen = 0;

    if (!s_bTriedOpen)
    {
        s_bTriedOpen = 1;

        // Try to create logs directory first
        #ifdef _WIN32
        CreateDirectoryA("logs", NULL);
        #else
        mkdir("logs", 0755);
        #endif

        s_pLogFile = fopen(ADEBUG_LOG_FILE, "w");
        if (s_pLogFile)
        {
            // Write header
            time_t now = time(NULL);
            struct tm* t = localtime(&now);
            fprintf(s_pLogFile, "=== Angelica Debug Log Started: %04d-%02d-%02d %02d:%02d:%02d ===\n",
                    t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                    t->tm_hour, t->tm_min, t->tm_sec);
            fprintf(s_pLogFile, "=== Log Level: %d (0=DEBUG, 1=INFO, 2=WARN, 3=ERROR) ===\n\n", ADEBUG_LOG_LEVEL);
            fflush(s_pLogFile);
        }
    }
    return s_pLogFile;
}

// Get level name string
static inline const char* _ADebugLog_LevelName(int level)
{
    switch(level)
    {
        case ALOG_LEVEL_DEBUG: return "DEBUG";
        case ALOG_LEVEL_INFO:  return "INFO ";
        case ALOG_LEVEL_WARN:  return "WARN ";
        case ALOG_LEVEL_ERROR: return "ERROR";
        default: return "?????";
    }
}

// Extract just the filename from a path
static inline const char* _ADebugLog_Basename(const char* path)
{
    const char* p = path;
    const char* last = path;
    while (*p)
    {
        if (*p == '/' || *p == '\\')
            last = p + 1;
        p++;
    }
    return last;
}

// Main logging function
static inline void _ADebugLog_Write(int level, const char* file, int line, const char* func, const char* fmt, ...)
{
    if (level < ADEBUG_LOG_LEVEL)
        return;

    FILE* fp = _ADebugLog_GetFile();
    if (!fp)
        return;

    // Get timestamp
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    // Write prefix: [TIME] [LEVEL] [file:line] func():
    fprintf(fp, "[%02d:%02d:%02d] [%s] [%s:%d] %s(): ",
            t->tm_hour, t->tm_min, t->tm_sec,
            _ADebugLog_LevelName(level),
            _ADebugLog_Basename(file), line,
            func);

    // Write message
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);

    fprintf(fp, "\n");
    fflush(fp);  // Always flush immediately so we don't lose logs on crash
}

// Hex dump function for binary data
static inline void _ADebugLog_HexDump(int level, const char* file, int line, const char* func,
                                       const char* desc, const void* data, int len)
{
    if (level < ADEBUG_LOG_LEVEL)
        return;

    FILE* fp = _ADebugLog_GetFile();
    if (!fp)
        return;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    fprintf(fp, "[%02d:%02d:%02d] [%s] [%s:%d] %s(): HexDump '%s' (%d bytes):\n",
            t->tm_hour, t->tm_min, t->tm_sec,
            _ADebugLog_LevelName(level),
            _ADebugLog_Basename(file), line,
            func, desc, len);

    const unsigned char* p = (const unsigned char*)data;
    int i, j;
    for (i = 0; i < len; i += 16)
    {
        fprintf(fp, "  %04X: ", i);

        // Hex bytes
        for (j = 0; j < 16; j++)
        {
            if (i + j < len)
                fprintf(fp, "%02X ", p[i + j]);
            else
                fprintf(fp, "   ");
            if (j == 7) fprintf(fp, " ");
        }

        fprintf(fp, " |");

        // ASCII
        for (j = 0; j < 16 && i + j < len; j++)
        {
            unsigned char c = p[i + j];
            fprintf(fp, "%c", (c >= 32 && c < 127) ? c : '.');
        }

        fprintf(fp, "|\n");
    }
    fflush(fp);
}

#ifdef __cplusplus
}
#endif

// Public macros - these are what you use in code
#if ADEBUG_LOG_ENABLED

#define ALOG_DEBUG(fmt, ...) \
    _ADebugLog_Write(ALOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ALOG_INFO(fmt, ...) \
    _ADebugLog_Write(ALOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ALOG_WARN(fmt, ...) \
    _ADebugLog_Write(ALOG_LEVEL_WARN, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ALOG_ERROR(fmt, ...) \
    _ADebugLog_Write(ALOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#define ALOG_HEXDUMP(level, desc, data, len) \
    _ADebugLog_HexDump(level, __FILE__, __LINE__, __FUNCTION__, desc, data, len)

// Convenience macro for function entry/exit tracing
#define ALOG_FUNC_ENTER() ALOG_DEBUG(">>> ENTER")
#define ALOG_FUNC_EXIT()  ALOG_DEBUG("<<< EXIT")
#define ALOG_FUNC_EXIT_VAL(val) ALOG_DEBUG("<<< EXIT (return %d)", (int)(val))

#else

// Disabled - compile to nothing
#define ALOG_DEBUG(fmt, ...) ((void)0)
#define ALOG_INFO(fmt, ...)  ((void)0)
#define ALOG_WARN(fmt, ...)  ((void)0)
#define ALOG_ERROR(fmt, ...) ((void)0)
#define ALOG_HEXDUMP(level, desc, data, len) ((void)0)
#define ALOG_FUNC_ENTER() ((void)0)
#define ALOG_FUNC_EXIT()  ((void)0)
#define ALOG_FUNC_EXIT_VAL(val) ((void)0)

#endif

// ============================================================================
// MBCS-safe string functions for GBK/GB2312 encoded paths
// ============================================================================

// MBCS-safe strlwr - preserves GBK multibyte characters
// GBK lead bytes are 0x81-0xFE, trail bytes are 0x40-0xFE
// We only lowercase ASCII characters (0x00-0x7F)
static inline char* strlwr_mbcs(char* str)
{
    if (!str) return str;  // Handle NULL pointer
    unsigned char* p = (unsigned char*)str;
    while (*p)
    {
        // Check if this is a GBK lead byte (0x81-0xFE)
        if (*p >= 0x81 && *p <= 0xFE)
        {
            // Skip both bytes of the GBK character - don't modify!
            if (*(p+1))
            {
                p += 2;
            }
            else
            {
                break;  // Incomplete GBK character at end
            }
        }
        else
        {
            // ASCII character - safe to lowercase
            if (*p >= 'A' && *p <= 'Z')
                *p = *p + ('a' - 'A');
            p++;
        }
    }
    return str;
}

// MBCS-safe tolower for single character in context of a string
// Returns the lowercased character, or original if it's part of a GBK sequence
// Usage: For NormalizeFileName type functions that process char-by-char
static inline char tolower_mbcs_char(unsigned char c, unsigned char prev)
{
    // If previous char was a GBK lead byte, this is a trail byte - don't modify
    if (prev >= 0x81 && prev <= 0xFE)
        return c;

    // If this is a GBK lead byte, don't modify
    if (c >= 0x81 && c <= 0xFE)
        return c;

    // ASCII uppercase - safe to lowercase
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');

    return c;
}

#endif  // _ADEBUGLOG_H_
