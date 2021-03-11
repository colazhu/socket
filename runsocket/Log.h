#ifndef LOG_H
#define LOG_H

#include "Base.h"
#include <string>
#include <map>

typedef void (*LogFunc)(int loglevel, const char* tag, const char* format, ...);

enum Log_Level {
    Log_Error,
    Log_Warn,
    Log_Info,
    Log_Debug,
};

class Log
{
public:
    static void init_log_system(LogFunc func = NULL);
    static void deinit_log_system();
    static void set_log_level(Log_Level lv);

    static void output(const char* tag, int loglevel, const char* format, ...);
    static void output(const char* mainTag,
                           const char* subTag,
                           int loglevel,
                           const char* file,
                           int line,
                           const char* func,
                           const char* format,
                           ...);

    static unsigned long tickCount();
    static unsigned long threadTime();

private:
    Log() {}

    static Log_Level s_level;
    static LogFunc s_pLogFunc;
    DISABLE_COPY(Log)
};

class TraceLog
{
    friend class Log;
public:
    TraceLog(const char* mainTag,
                  const char* subTag,
                  const char* file,
                  int line,
                  const char* func,
                  const char* szFormat,
                  ...);

    ~TraceLog();

private:
    std::string m_maintag;
    std::string m_subtag;
    std::string m_blockinfo;    ///< code block
    std::string m_filename;     ///< file name
    std::string m_funcname;     ///< func name
    int m_line;                 ///< line number
    unsigned long m_tickcount;  ///< tick count
    unsigned long m_threadtime; ///< thread time

    DISABLE_COPY(TraceLog)
};

#ifndef LOG
#define LOG_D(maintag, subtag, ...) ((void)Log::output(maintag, subtag, Log_Debug, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define LOG_I(maintag, subtag, ...) ((void)Log::output(maintag, subtag, Log_Info, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define LOG_W(maintag, subtag, ...) ((void)Log::output(maintag, subtag, Log_Warn, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define LOG_E(maintag, subtag, ...) ((void)Log::output(maintag, subtag, Log_Error, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))
#define LOG LOG_D
#define LOG_TRACE(maintag, subtag, ...) TraceLog _tracelog(maintag, subtag, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DUMP(maintag, subtag, data, size) \
{\
    std::string strData("");\    
    LOG_D(maintag, subtag, "DUMP(size:%d)", size);\
    LOG_D(maintag, subtag, "-------------------------------------------------");\
    for (size_t count = 1; count <= size; ++count) {\
        char tmp[8] = {0};\
        snprintf(tmp, 8," 0x%02x", (char)(data[count-1]));\
        strData += tmp;\
        if (0 == count%16) {\
            LOG_D(maintag, subtag, strData.c_str());\
            strData = "";\
        }\
    }\
    if (!strData.empty()) {\
        LOG_D(maintag, subtag, strData.c_str());\
    }\
    LOG_D(maintag, subtag, "-------------------------------------------------");\
}

#define BACKTRACE() \
    void *array[32]; \
    size_t size; \
    char **strings; \
    size = backtrace(array, 32); \
    strings = backtrace_symbols(array, size); \
    if (NULL != strings) { \
        for (size_t i = 0; i < size; i++) { \
            Log::output("DUMP", Log_Error, "%s", strings[i]); \
        } \
    }
#endif

#ifndef LOG_BASE_MAIN
#define LOG_BASE_MAIN "Common"
#endif

#ifndef LOG_BASE
#define LOG_BASE_TAG   "Base"
#define LOG_BASE_D(...) LOG_D(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#define LOG_BASE_I(...) LOG_I(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#define LOG_BASE_W(...) LOG_W(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#define LOG_BASE_E(...) LOG_E(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#define LOG_BASE(...)   LOG_D(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#define LOG_BASE_DUMP(data, size)  LOG_DUMP(LOG_BASE_MAIN, LOG_BASE_TAG, data, size)
#define LOG_BASE_TRACE(...) LOG_TRACE(LOG_BASE_MAIN, LOG_BASE_TAG, __VA_ARGS__)
#endif

#endif
