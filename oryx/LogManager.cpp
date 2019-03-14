#include "LogManager.h"
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <iostream>
Logger::Logger()
{
	BZERO(file_name, MAX_FILE_NAME_SIZE);
	isInit = false;
	isPrintScreen = true;
	max_log_type = LOG_LEVEL_DEBUG;
}

bool Logger::init(const char * filename, int logTypeMax, bool print_screen)
{
	if (filename == NULL || strlen(filename) >= MAX_FILE_NAME_SIZE) {
		return false;
	}

	strncpy(file_name, filename, MAX_FILE_NAME_SIZE - 1);
	this->max_log_type = (LOG_LEVEL)logTypeMax;

	isInit = true;
	this->isPrintScreen = print_screen;
	return true;

}

bool Logger::log(LOG_LEVEL iLogLevel, const char * file, int line, const char *pcContent, va_list& ap)
{
	if (file == NULL || pcContent == NULL) {
		return false;
	}

	if (iLogLevel < max_log_type) {
		return true;
	}
	time_t tNow = time(NULL);
	struct tm stTempTm;
	localtime_r(&tNow, &stTempTm);

	const char * fileNameBegin = file + strlen(file);
	while (fileNameBegin > file) {
		if (*fileNameBegin == '/') {
			fileNameBegin++;
			break;
		}
		fileNameBegin--;
	}

	if (this->isPrintScreen){
		printf("<%02d:%02d:%02d>[%s:%d]", stTempTm.tm_hour, stTempTm.tm_min, stTempTm.tm_sec, fileNameBegin,line);
		vprintf(pcContent, ap);
		printf("\n");
	}
	
	if(strlen(file_name) == 0){
		return false;
	}

	char fileName[MAX_FILE_NAME_SIZE];

	sprintf(fileName, "%s-%04d%02d%02d-%02d.log", this->file_name,
		stTempTm.tm_year + 1900,
		stTempTm.tm_mon + 1,
		stTempTm.tm_mday,
		stTempTm.tm_hour);

	FILE * pFile = fopen(fileName, "a+");
	if (pFile == NULL) {
		return false;
	}

	fprintf(pFile, "<%02d:%02d:%02d>[%s:%d]", stTempTm.tm_hour, stTempTm.tm_min, stTempTm.tm_sec, fileNameBegin,line);
	vfprintf(pFile, pcContent, ap);
	fprintf(pFile, "\n");

	fflush(pFile);
	fclose(pFile);

	return true;

}

SINGLETON_DEFINE(LogManager)

LogManager::LogManager() {};

bool LogManager::init()
{
	// logic_Logger.init("../log/gameLogic");
	// epoll_Logger.init("../log/gameEpoll");
	// async_Logger.init("../log/gameAsync");
	return true;
}

bool LogManager::logLogic(LOG_LEVEL iLogLevel, const char * file, int line, const char *pcContent, ...)
{
	if (file == NULL || pcContent == NULL) {
		return false;
	}

	va_list ap;
	va_start(ap, pcContent);
	bool iRet = logic_Logger.log(iLogLevel, file, line, pcContent, ap);
	va_end(ap);

	return iRet;
}

bool LogManager::logEpoll(LOG_LEVEL iLogLevel, const char * file, int line, const char *pcContent, ...)
{
	va_list ap;
	va_start(ap, pcContent);
	bool iRet = epoll_Logger.log(iLogLevel, file, line, pcContent, ap);
	va_end(ap);

	return iRet;
}

bool LogManager::logAsync(LOG_LEVEL iLogLevel, const char * file, int line, const char *pcContent, ...)
{
	va_list ap;
	va_start(ap, pcContent);
	bool iRet = async_Logger.log(iLogLevel, file, line, pcContent, ap);
	va_end(ap);

	return iRet;
}
