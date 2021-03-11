#include "Log.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdarg.h>
#include <string.h>

// #define OS_ANDROID (1)

#if OS_ANDROID
#include <android/log.h>
#endif

#define USE_CALLDEPTH

static const char* const LOG_FOMAT_STR = "[%s]<tid:%5d>[%s@%s(%d)] %s%s";
static const int LOG_SINGLE_LOG_SIZE = 1024;

struct callinfo{
    int depth;
    char mark;
};

//#if CONFIG(OS_WIN)
//const static char s_seperator = '\\';
//#else
const static char s_seperator = '/';
//#endif
const static char s_defMark = 'x';
const static int s_maxdepth = 32;
const static std::string s_marks = "#$&^mvowx";
static int s_marks_count = s_marks.size();
static std::map<int, callinfo> s_mapTid2Depth;

inline unsigned long __gettickcount()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

inline unsigned long __getthreadtime()
{
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return time.tv_sec * 1000 + time.tv_nsec / 1000000;
}

inline int __getthreadid()
{
    // return -1; // todo
    return syscall(__NR_gettid);
}

inline const char* __getCallDepthStr() {
    static char retStr[s_maxdepth + 2] = {0};
#ifdef USE_CALLDEPTH
    int tid = __getthreadid();
    callinfo info = {0, s_defMark};
    if (s_mapTid2Depth.count(tid)) {
        info = s_mapTid2Depth[tid];
    }

    ::memset(retStr, info.mark, info.depth);
    retStr[info.depth] = ' ';
    retStr[info.depth+1] = '\0';
#endif
    return retStr;
}

inline int __incCallDepth()
{
#ifdef USE_CALLDEPTH
    int tid = __getthreadid();
    if (!s_mapTid2Depth.count(tid)) {
        callinfo info = {1, s_defMark};
        --s_marks_count;
        if (s_marks_count >= 0) {
            info.mark = s_marks.at(s_marks_count);
        }
        s_mapTid2Depth[tid] = info;
        return 1;
    }
    else {
        int depth_next = std::min(s_maxdepth, s_mapTid2Depth[tid].depth + 1);
        s_mapTid2Depth[tid].depth = depth_next;
        return depth_next;
    }
#endif
    return 0;
}

inline int __decCallDepth()
{
#ifdef USE_CALLDEPTH
    int tid = __getthreadid();
    if (s_mapTid2Depth.count(tid)) {
        int depth_next = std::max(0, s_mapTid2Depth[tid].depth-1);
        s_mapTid2Depth[tid].depth = depth_next;
        return depth_next;
    }
#endif
    return 0;
}

void logfunc_printf(int loglevel, const char* tag, const char* format, ...)
{
    UNUSED(loglevel);
    char szTempLogInfo[LOG_SINGLE_LOG_SIZE] = {0};
    va_list list;
    va_start(list, format);
    vsnprintf(szTempLogInfo, LOG_SINGLE_LOG_SIZE, format, list);
    va_end(list);

#if OS_ANDROID
   __android_log_print(ANDROID_LOG_DEBUG, "CubeSocket", szTempLogInfo);
#else
    printf("\n[LV%d][%s]%s", loglevel, tag, szTempLogInfo);
#endif
}

//void logfunc_nclog(int loglevel, const char* tag, const char* format, ...)
//{
//    va_list list;
//    va_start(list, format);
//    nutshell::NCLog::output(static_cast<nutshell::NC_LOGTYPE>(loglevel),
//              tag,
//              format,
//              list);
//    va_end(list);
//}

Log_Level Log::s_level = Log_Error;
LogFunc Log::s_pLogFunc = logfunc_printf;

void Log::init_log_system(LogFunc fun)
{
    if (fun) {
        s_pLogFunc = fun;
    }
}

void Log::deinit_log_system()
{
    s_pLogFunc = logfunc_printf;
}

void Log::set_log_level(Log_Level lv)
{
    s_level = lv;
}

unsigned long Log::tickCount()
{
    return __gettickcount();
}

unsigned long Log::threadTime()
{
    return __getthreadtime();
}

void Log::output(const char* tag, int loglevel, const char* format, ...)
{
    if (loglevel > s_level) {
        return;
    }

    if (!s_pLogFunc) {
        return;
    }
    va_list list;
    va_start(list, format);
    s_pLogFunc(loglevel, tag, format, list);
    va_end(list);
}

void Log::output(const char* mainTag,
                   const char* subTag,
                   int loglevel,
                   const char* pszFileName,
                   int iLineNum,
                   const char* pszFuncName,
                   const char* format,
                      ...)
{
    if (loglevel < s_level) {
        return;
    }

    if (!s_pLogFunc) {
        return;
    }

    char szTempLogInfo[LOG_SINGLE_LOG_SIZE] = {0};
    va_list list;
    va_start(list, format);
    vsnprintf(szTempLogInfo, LOG_SINGLE_LOG_SIZE, format, list);
    va_end(list);

    s_pLogFunc(loglevel, mainTag,
               LOG_FOMAT_STR,
               subTag,
               __getthreadid(),
               pszFuncName,
               strrchr(pszFileName, s_seperator) + 1,
               iLineNum,
               __getCallDepthStr(),
               szTempLogInfo);

}

TraceLog::TraceLog(const char* maintag,
                             const char* subtag,
                             const char* file,
                             int line,
                             const char* func,
                             const char* format,
                             ...)
        : m_maintag(maintag),
          m_subtag(subtag),
          m_filename(file),
          m_funcname(func),
          m_line(line)
{
    char szTempLogInfo[LOG_SINGLE_LOG_SIZE] = {0};
    va_list list;
    va_start(list, format);
    vsnprintf(szTempLogInfo, LOG_SINGLE_LOG_SIZE, format, list);
    va_end(list);
    m_blockinfo = szTempLogInfo;
    __incCallDepth();
    Log::output( m_maintag.c_str(),
                 m_subtag.c_str(),
                 Log_Debug,
                 m_filename.c_str(),
                 m_line,
                 m_funcname.c_str(),
                 "%s %s",
                 m_blockinfo.c_str(),
                 "+++");

    m_tickcount = __gettickcount();
    m_threadtime = __getthreadtime();

}

TraceLog::~TraceLog()
{
    Log::output( m_maintag.c_str(),
                 m_subtag.c_str(),
                 Log_Debug,
                 m_filename.c_str(),
                 m_line,
                 m_funcname.c_str(),
                 "%s %s, cost %.3f/%.3f seconds",
                 m_blockinfo.c_str(),
                 "---",
                 (__getthreadtime() - m_threadtime) / 1000.0,
                 (__gettickcount() - m_tickcount) / 1000.0);

    __decCallDepth();
}
