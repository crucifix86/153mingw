/*
 * FILE: BolaDebug.cpp
 *
 * DESCRIPTION: Comprehensive debug logging system implementation
 * Hooks into Angelica logging and provides full execution tracing
 *
 * CREATED BY: BOLA Debug Team, 2025
 */

#include "BolaDebug.h"
#include "ALog.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/time.h>
#endif

// Global state
static FILE* g_pBolaLogFile = NULL;
static int g_iTraceDepth = 0;
static bool g_bInitialized = false;
static LPFNDEFLOGOUTPUT g_pOldLogOutput = NULL;
static char g_szLogDir[MAX_PATH] = "Logs";

// Log level names
static const char* g_szLevelNames[] = {
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "FATAL"
};

// Forward declaration
static void BolaDebug_AngelicaLogHandler(const char* szMsg);

// Get timestamp with milliseconds
static void GetTimestamp(char* buf, size_t bufSize) {
#ifdef _WIN32
    SYSTEMTIME st;
    GetLocalTime(&st);
    _snprintf(buf, bufSize, "%02d:%02d:%02d.%03d",
              st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else
    struct timeval tv;
    struct tm* tm_info;
    gettimeofday(&tv, NULL);
    tm_info = localtime(&tv.tv_sec);
    snprintf(buf, bufSize, "%02d:%02d:%02d.%03ld",
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, tv.tv_usec / 1000);
#endif
}

// Extract just the filename from a path
static const char* GetFileName(const char* path) {
    const char* p1 = strrchr(path, '/');
    const char* p2 = strrchr(path, '\\');
    const char* p = (p1 > p2) ? p1 : p2;
    return p ? p + 1 : path;
}

// Create directory if it doesn't exist
static void EnsureDir(const char* dir) {
#ifdef _WIN32
    if (_access(dir, 0) == -1) {
        CreateDirectoryA(dir, NULL);
    }
#else
    struct stat st;
    if (stat(dir, &st) == -1) {
        mkdir(dir, 0755);
    }
#endif
}

void BolaDebug_Init(const char* szLogDir) {
    if (g_bInitialized) return;

    if (szLogDir && szLogDir[0]) {
        strncpy(g_szLogDir, szLogDir, MAX_PATH - 1);
        g_szLogDir[MAX_PATH - 1] = '\0';
    }

    EnsureDir(g_szLogDir);

    char szLogPath[MAX_PATH];
    snprintf(szLogPath, MAX_PATH, "%s/BolaTrace.log", g_szLogDir);

    g_pBolaLogFile = fopen(szLogPath, "wt");
    if (!g_pBolaLogFile) {
        // Try current directory as fallback
        g_pBolaLogFile = fopen("BolaTrace.log", "wt");
    }

    if (g_pBolaLogFile) {
        // Write header
        char timestamp[32];
        GetTimestamp(timestamp, sizeof(timestamp));

        fprintf(g_pBolaLogFile, "===========================================\n");
        fprintf(g_pBolaLogFile, "BOLA DEBUG TRACE LOG\n");
        fprintf(g_pBolaLogFile, "Started: %s\n", timestamp);
        fprintf(g_pBolaLogFile, "===========================================\n\n");
        fflush(g_pBolaLogFile);
    }

    // Hook into Angelica logging system
    g_pOldLogOutput = a_RedirectDefLogOutput(BolaDebug_AngelicaLogHandler);

    g_bInitialized = true;
    g_iTraceDepth = 0;

    BOLA_INFO("BOLA Debug System Initialized");
}

void BolaDebug_Shutdown() {
    if (!g_bInitialized) return;

    BOLA_INFO("BOLA Debug System Shutting Down");

    // Restore old log handler
    if (g_pOldLogOutput) {
        a_RedirectDefLogOutput(g_pOldLogOutput);
        g_pOldLogOutput = NULL;
    }

    if (g_pBolaLogFile) {
        char timestamp[32];
        GetTimestamp(timestamp, sizeof(timestamp));
        fprintf(g_pBolaLogFile, "\n===========================================\n");
        fprintf(g_pBolaLogFile, "Log Closed: %s\n", timestamp);
        fprintf(g_pBolaLogFile, "===========================================\n");
        fclose(g_pBolaLogFile);
        g_pBolaLogFile = NULL;
    }

    g_bInitialized = false;
}

// Handler for Angelica's a_LogOutput calls
static void BolaDebug_AngelicaLogHandler(const char* szMsg) {
    if (!g_pBolaLogFile) return;

    char timestamp[32];
    GetTimestamp(timestamp, sizeof(timestamp));

    // Determine if it's an error or info
    const char* level = "ALOG ";
    if (szMsg && szMsg[0] == '<' && szMsg[1] == '!' && szMsg[2] == '>') {
        level = "AERR ";
    }

    // Write to our log with ANGELICA prefix
    fprintf(g_pBolaLogFile, "[%s] [%s] [ANGELICA] %s", timestamp, level, szMsg);
    fflush(g_pBolaLogFile);

    // Also pass to original handler if any
    if (g_pOldLogOutput) {
        g_pOldLogOutput(szMsg);
    }

    // And to debug output
#ifdef _WIN32
    OutputDebugStringA(szMsg);
#endif
}

void BolaDebug_Log(int level, const char* file, int line, const char* func, const char* fmt, ...) {
    if (!g_pBolaLogFile) return;
    if (level < BOLA_LOG_LEVEL) return;

    char timestamp[32];
    GetTimestamp(timestamp, sizeof(timestamp));

    const char* filename = GetFileName(file);
    const char* levelName = (level >= 0 && level <= 5) ? g_szLevelNames[level] : "?????";

    // Build indentation based on trace depth
    char indent[128] = "";
    int depth = (g_iTraceDepth > 30) ? 30 : g_iTraceDepth;
    for (int i = 0; i < depth; i++) {
        strcat(indent, "  ");
    }

    // Format the user message
    char userMsg[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(userMsg, sizeof(userMsg), fmt, args);
    va_end(args);

    // Write to log file
    fprintf(g_pBolaLogFile, "[%s] [%s] %s%s:%d (%s): %s\n",
            timestamp, levelName, indent, filename, line, func, userMsg);

    // Always flush for debugging
    fflush(g_pBolaLogFile);

    // Also output to debugger
#ifdef _WIN32
    char debugMsg[2560];
    _snprintf(debugMsg, sizeof(debugMsg), "[BOLA][%s] %s:%d (%s): %s\n",
              levelName, filename, line, func, userMsg);
    OutputDebugStringA(debugMsg);
#endif
}

void BolaDebug_FuncEnter(const char* file, int line, const char* func) {
    if (!g_pBolaLogFile) return;

    char timestamp[32];
    GetTimestamp(timestamp, sizeof(timestamp));

    const char* filename = GetFileName(file);

    // Build indentation
    char indent[128] = "";
    int depth = (g_iTraceDepth > 30) ? 30 : g_iTraceDepth;
    for (int i = 0; i < depth; i++) {
        strcat(indent, "  ");
    }

    fprintf(g_pBolaLogFile, "[%s] [ENTER] %s--> %s (%s:%d)\n",
            timestamp, indent, func, filename, line);
    fflush(g_pBolaLogFile);

    g_iTraceDepth++;

#ifdef _WIN32
    char debugMsg[512];
    _snprintf(debugMsg, sizeof(debugMsg), "[BOLA][ENTER] --> %s (%s:%d)\n", func, filename, line);
    OutputDebugStringA(debugMsg);
#endif
}

void BolaDebug_FuncExit(const char* file, int line, const char* func) {
    if (!g_pBolaLogFile) return;

    if (g_iTraceDepth > 0) g_iTraceDepth--;

    char timestamp[32];
    GetTimestamp(timestamp, sizeof(timestamp));

    const char* filename = GetFileName(file);

    // Build indentation
    char indent[128] = "";
    int depth = (g_iTraceDepth > 30) ? 30 : g_iTraceDepth;
    for (int i = 0; i < depth; i++) {
        strcat(indent, "  ");
    }

    fprintf(g_pBolaLogFile, "[%s] [EXIT ] %s<-- %s (%s:%d)\n",
            timestamp, indent, func, filename, line);
    fflush(g_pBolaLogFile);

#ifdef _WIN32
    char debugMsg[512];
    _snprintf(debugMsg, sizeof(debugMsg), "[BOLA][EXIT ] <-- %s (%s:%d)\n", func, filename, line);
    OutputDebugStringA(debugMsg);
#endif
}

void BolaDebug_Flush() {
    if (g_pBolaLogFile) {
        fflush(g_pBolaLogFile);
    }
}

int BolaDebug_GetDepth() {
    return g_iTraceDepth;
}
