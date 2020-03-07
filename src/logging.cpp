/*
 * Copyright (C) 2010-2020 Beat Küng <beat-kueng@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "logging.h"
#include "config.h"
#include "global.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <ctime>



/*********************************************************************//*
 * class CLog
 *//*********************************************************************/


string CLog::toStr(ELOG level)
{
	switch (level) {
	case ERROR: return ("ERROR");
	case WARN: return ("WARN");
	case INFO: return ("INFO");
	case DEBUG: return ("DEBUG");
	case NONE:
	default:
		return ("UNKNOWN LEVEL");
	}
}

bool CLog::parseLevel(const string& level, ELOG& level_out)
{
	string str = toLower(level);
	if (str == "error" || str == "e") level_out = ERROR;
	else if (str == "warn" || str == "w") level_out = WARN;
	else if (str == "info" || str == "i") level_out = INFO;
	else if (str == "debug" || str == "d") level_out = DEBUG;
	else if (str == "none" || str == "n") level_out = NONE;
	else return false;
	return true;
}

void CLog::Log(ELOG level, const char* file, const char* function, int line,
			   const char* fmt, ...)
{

	char buffer[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	
	if (level <= m_file_log) {
		FILE* pFile = fopen(LOG_FILE, "a+");
		if (pFile) {
			if (m_bLog_src_file[level] && file) {
				/* be more verbose in debug mode */
#ifdef _DEBUG
				fprintf(pFile, "%s: %s() Line %d: ", file, function, line);
#else
				fprintf(pFile, "%s(): ", function);
#endif
			}
			if (m_bLog_time_file) fprintf(pFile, "%s %s: ", getDate().c_str(),
											  getTime().c_str());
			fprintf(pFile, "%s: %s\n", toStr(level).c_str(), buffer);
			fclose(pFile);
		}
		++m_file_log_count[level];
	}
	
	if (level <= m_console_log) {
		if (level == ERROR) {
			if (m_bLog_src_file[level] && file) {
#ifdef _DEBUG
				fprintf(stderr, "%s: %s() Line %d: ", file, function, line);
#else
				fprintf(stderr, "%s(): ", function);
#endif
			}
			if (m_bLog_time_console) fprintf(stderr, "%s %s: ", getDate().c_str(),
												 getTime().c_str());
			fprintf(stderr, "%s\n", buffer);
		} else {
			if (m_bLog_src_file[level] && file) {
#ifdef _DEBUG
				printf("%s: %s() Line %d: ", file, function, line);
#else
				printf("%s(): ", function);
#endif
			}
			if (m_bLog_time_console) printf("%s %s: ", getDate().c_str(),
												getTime().c_str());
			printf("%s\n", buffer);
		}
		++m_console_log_count[level];
	}
	
}


int CLog::getConsoleLogCount()
{
	int sum = 0;
	for (int i = 0; i < LOG_LEVEL_COUNT; ++i) sum += m_console_log_count[i];
	return sum;
}

int CLog::getFileLogCount()
{
	int sum = 0;
	for (int i = 0; i < LOG_LEVEL_COUNT; ++i) sum += m_file_log_count[i];
	return sum;
}


string CLog::getDate()
{
	char   timestr[20];
	time_t seconds = time(0);
	struct tm* ptm = localtime(&seconds);
	sprintf(timestr, "%02i.%02i.%02i",
			(int)ptm->tm_mday,
			(int)ptm->tm_mon + 1,
			(int)ptm->tm_year - 100);
	return timestr;
	
}
string CLog::getTime()
{
	char   timestr[20];
	time_t seconds = time(0);
	struct tm* ptm = localtime(&seconds);
	sprintf(timestr, "%02i:%02i:%02i",
			(int)ptm->tm_hour,
			(int)ptm->tm_min,
			(int)ptm->tm_sec);
	return timestr;
}


CLog::CLog() : m_bLog_time_file(true), m_bLog_time_console(false)
	, m_console_log(INFO), m_file_log(INFO)
{
	memset(m_console_log_count, 0, sizeof(m_console_log_count));
	memset(m_file_log_count, 0, sizeof(m_file_log_count));
	memset(m_bLog_src_file, 0, sizeof(m_bLog_src_file));
}

CLog::~CLog()
{
}


CLog::Instance CLog::m_instance;



