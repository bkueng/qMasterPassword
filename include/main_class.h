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

#ifndef MAIN_CLASS_H_
#define MAIN_CLASS_H_

#include <QTranslator>

#include "command_line.h"
#include "global.h"

/*********************************************************************//*
 * class CMain
 *
 * main class with the main task and command line parser
 *//*********************************************************************/

class CMain {
   public:
    CMain();
    ~CMain();

    /* parse the command line parameters */
    void init(int argc, char* argv[]);

    int exec();

   private:
    void parseCommandLine(int argc, char* argv[]);
    void printHelp();
    void wrongUsage(const char* fmt, ...);
    void printVersion();

    int processArgs();

    void loadTranslation();
    QTranslator m_app_trans;
    QTranslator m_qt_trans;
    QTranslator m_qtbase_trans;

    CCommandLineParser* m_parameters;
    ECLParsingResult m_cl_parse_result;
    int m_argc;
    char** m_argv;
};

#endif /* MAIN_CLASS_H_ */
