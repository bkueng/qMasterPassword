/*
 * Copyright (C) 2010 Beat Küng <beat-kueng@gmx.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <cstdlib>
#include <string>
using namespace std;

enum ELOG {
	NONE=0,
	ERROR,
	WARN,
	INFO,
	DEBUG
};

#define LOG_LEVEL_COUNT 5


#define LOG(level, fmt, ...) CLog::getInstance().Log(level, __FILE__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)

#define LOG_s(level, fmt, ...) CLog::getInstance().Log(level, fmt, ## __VA_ARGS__)

/*////////////////////////////////////////////////////////////////////////////////////////////////
 ** class CLog
 * file and console logging class
/*////////////////////////////////////////////////////////////////////////////////////////////////

class CLog {
public:
	static CLog& getInstance() {
		return(m_instance.log ? *m_instance.log : *(m_instance.log=new CLog()));
	}
	
	static string toStr(ELOG level);
	
	void Log(ELOG level, const char* file, const char* function, int line, const char* fmt, ...);
	
	ELOG consoleLevel() { return(m_console_log); }
	ELOG fileLevel() { return(m_file_log); }
	
	void setConsoleLevel(ELOG level) { if(level>=NONE && level<=DEBUG) m_console_log=level; }
	void setFileLevel(ELOG level) { if(level>=NONE && level<=DEBUG) m_file_log=level; }
	
	bool logDateTimeFile() { return(m_bLog_time_file); }
	bool logDateTimeConsole() { return(m_bLog_time_console); }
	void setLogDateTime(bool on) { m_bLog_time_file=m_bLog_time_console=on; }
	void setLogDateTimeConsole(bool on) { m_bLog_time_console=on; }
	void setLogDateTimeFile(bool on) { m_bLog_time_file=on; }
	
	
	bool logSourceFile(ELOG level) { return(m_bLog_src_file[level]); }
	void setLogSourceFile(ELOG level, bool on) { m_bLog_src_file[level]=on; }
	void setLogSourceFileAll(bool on) { for(int i=0; i<LOG_LEVEL_COUNT; ++i) m_bLog_src_file[i]=on; }
	
	int getFileLogCount(ELOG log_level) { return(m_file_log_count[log_level]);}
	int getFileLogCount(); //sum all levels
	
	int getConsoleLogCount(ELOG log_level) { return(m_console_log_count[log_level]);}
	int getConsoleLogCount(); //sum all levels
	
	static string getDate(); //format: DD.MM.YY
	static string getTime(); //format: HH:MM:SS
private:
	CLog();
	~CLog();
	
	bool m_bLog_time_file;
	bool m_bLog_time_console;
	bool m_bLog_src_file[LOG_LEVEL_COUNT];
	
	ELOG m_console_log;
	ELOG m_file_log;
	
	int m_file_log_count[LOG_LEVEL_COUNT];
	int m_console_log_count[LOG_LEVEL_COUNT];
	
	struct Instance {
		Instance() : log(NULL) {}
		~Instance() { if(log) delete(log); }
		CLog* log;
	};
	static Instance m_instance;
	
};


#endif /* LOGGING_H_ */
