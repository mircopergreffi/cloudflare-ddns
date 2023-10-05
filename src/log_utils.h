#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#ifdef _WIN32
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3

#define LOG(...) printf(__VA_ARGS__); printf(NEWLINE);

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) if (LOG_LEVEL <= LOG_LEVEL_DEBUG) { printf("[dbg][%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); LOG(__VA_ARGS__); }
#else
#define LOG_DEBUG(...)
#endif

#define TRACE_START() LOG_DEBUG("start");
#define TRACE_END() LOG_DEBUG("end");

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) if (LOG_LEVEL <= LOG_LEVEL_INFO) { printf("[info] "); LOG(__VA_ARGS__); }
#else
#define LOG_INFO(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define LOG_WARNING(...) if (LOG_LEVEL <= LOG_LEVEL_WARNING) { printf("[warn] "); LOG(__VA_ARGS__); }
#else
#define LOG_WARNING(...)
#endif

#define LOG_ERROR(...) if (LOG_LEVEL <= LOG_LEVEL_ERROR) { printf("[err][%s][%s][%d] ", __FILE__, __FUNCTION__, __LINE__); LOG(__VA_ARGS__); }

#endif