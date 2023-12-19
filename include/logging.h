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

#ifndef LOGGING_H_
#define LOGGING_H_

#include <cstdlib>
#include <string>
using namespace std;

enum class LogLevel {
    None = 0,
    Error,
    Warn,
    Info,
    Debug,

    Count
};

#define LOG_LEVEL_COUNT static_cast<int>(LogLevel::Count)

#define LOG(level, fmt, ...) \
    CLog::getInstance().Log(level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_s(level, fmt, ...) CLog::getInstance().Log(level, fmt, ##__VA_ARGS__)

/*********************************************************************//*
 * class CLog
 *
 * file and console logging class
 *//*********************************************************************/

class CLog {
   public:
    static CLog& getInstance()
    {
        return m_instance.log ? *m_instance.log : *(m_instance.log = new CLog());
    }

    static string toStr(LogLevel level);
    static bool parseLevel(const string& level, LogLevel& level_out);

    void Log(LogLevel level, const char* file, const char* function, int line, const char* fmt,
             ...);

    LogLevel consoleLevel() { return m_console_log; }
    LogLevel fileLevel() { return m_file_log; }

    void setConsoleLevel(LogLevel level)
    {
        if (level >= LogLevel::None && level <= LogLevel::Debug) m_console_log = level;
    }
    void setFileLevel(LogLevel level)
    {
        if (level >= LogLevel::None && level <= LogLevel::Debug) m_file_log = level;
    }

    bool logDateTimeFile() { return m_bLog_time_file; }
    bool logDateTimeConsole() { return m_bLog_time_console; }
    void setLogDateTime(bool on) { m_bLog_time_file = m_bLog_time_console = on; }
    void setLogDateTimeConsole(bool on) { m_bLog_time_console = on; }
    void setLogDateTimeFile(bool on) { m_bLog_time_file = on; }

    bool logSourceFile(LogLevel level) { return m_bLog_src_file[static_cast<int>(level)]; }
    void setLogSourceFile(LogLevel level, bool on)
    {
        m_bLog_src_file[static_cast<int>(level)] = on;
    }
    void setLogSourceFileAll(bool on)
    {
        for (int i = 0; i < LOG_LEVEL_COUNT; ++i) m_bLog_src_file[i] = on;
    }

    int getFileLogCount(LogLevel log_level)
    {
        return m_file_log_count[static_cast<int>(log_level)];
    }
    int getFileLogCount();  // sum all levels

    int getConsoleLogCount(LogLevel log_level)
    {
        return m_console_log_count[static_cast<int>(log_level)];
    }
    int getConsoleLogCount();  // sum all levels

    static string getDate();  // format: DD.MM.YY
    static string getTime();  // format: HH:MM:SS
   private:
    CLog();
    ~CLog();

    bool m_bLog_time_file;
    bool m_bLog_time_console;
    bool m_bLog_src_file[LOG_LEVEL_COUNT];

    LogLevel m_console_log;
    LogLevel m_file_log;

    int m_file_log_count[LOG_LEVEL_COUNT];
    int m_console_log_count[LOG_LEVEL_COUNT];

    struct Instance {
        Instance() : log(NULL) {}
        ~Instance()
        {
            if (log) delete (log);
        }
        CLog* log;
    };
    static Instance m_instance;
};

#endif /* LOGGING_H_ */
