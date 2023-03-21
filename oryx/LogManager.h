#ifndef __LOG_MANAGER__
#define __LOG_MANAGER__

#define MAX_FILE_NAME_SIZE 2048

#include "common.h"
#include <sys/file.h>

enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_ERROR = 3
};

class Logger
{
public:
    Logger();
    bool init(const char *filename, int logTypeMax = LOG_LEVEL_DEBUG);

    bool debug(const char *file, int line, const char *pcContent, va_list &ap);
    bool info(const char *file, int line, const char *pcContent, va_list &ap);
    bool warn(const char *file, int line, const char *pcContent, va_list &ap);
    bool error(const char *file, int line, const char *pcContent, va_list &ap);

    bool log(LOG_LEVEL iLogLevel, const char *file, int line, const char *pcContent, va_list &ap);

    inline bool isInited() { return this->isInit; }

private:
    char file_name[MAX_FILE_NAME_SIZE];
    LOG_LEVEL max_log_type;

    bool isInit;
};

class LogManager
{
    SINGLETON_DECLEAR(LogManager)

public:
    bool init();

    bool logLogic(LOG_LEVEL iLogLevel, const char *file, int line, const char *pcContent, ...);
    bool logEpoll(LOG_LEVEL iLogLevel, const char *file, int line, const char *pcContent, ...);
    bool logAsync(LOG_LEVEL iLogLevel, const char *file, int line, const char *pcContent, ...);

private:
    Logger logic_Logger;
    Logger epoll_Logger;
    Logger async_Logger;
};

#define TRACEGAME(LEVEL, format, ...) LogManager::getInstance()->logLogic(LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define TRACEDEBUG(format, ...) LogManager::getInstance()->logLogic(LOG_LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TRACEINFO(format, ...) LogManager::getInstance()->logLogic(LOG_LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TRACEWARN(format, ...) LogManager::getInstance()->logLogic(LOG_LEVEL_WARN, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TRACEERROR(format, ...) LogManager::getInstance()->logLogic(LOG_LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define TRACELOGIC(LEVEL, format, ...) LogManager::getInstance()->logLogic(LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TRACEEPOLL(LEVEL, format, ...) LogManager::getInstance()->logEpoll(LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define TRACEASYNC(LEVEL, format, ...) LogManager::getInstance()->logAsync(LEVEL, __FILE__, __LINE__, format, ##__VA_ARGS__)

#endif
