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

#include "main_class.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "crypto.h"
#include "main_window.h"
#include "version.h"

#ifdef TESTING_SUPPORT
#include "test/test.h"
#endif

CMain::CMain() : m_parameters(NULL), m_cl_parse_result(Parse_none_found)
{
}

CMain::~CMain()
{
    SAVE_DEL(m_parameters);
}

void CMain::init(int argc, char* argv[])
{
    parseCommandLine(argc, argv);
    m_argc = argc;
    m_argv = argv;
}

void CMain::parseCommandLine(int argc, char* argv[])
{
    SAVE_DEL(m_parameters);
    m_parameters = new CCommandLineParser(argc, argv);

    // init known arguments
    m_parameters->addSwitch("help", 'h');
    m_parameters->addSwitch("version");
    m_parameters->addSwitch("verbose", 'v');
    // control the logging
    m_parameters->addParam("log");
    m_parameters->addSwitch("no-log");
    m_parameters->addParam("file-log");
    m_parameters->addSwitch("no-file-log");

    m_parameters->addSwitch("start-minimized");

#ifdef TESTING_SUPPORT
    m_parameters->addParam("test");
#endif

    m_cl_parse_result = m_parameters->parse();
}

void CMain::printHelp()
{
    printf(
        "Usage:\n"
        " " APP_NAME
        " [-v] \n"
        " " APP_NAME
        " --version\n"
        "  -v, --verbose                   print debug messages\n"
        "                                  (same as --log debug)\n"
        "  -h, --help                      print this message\n"
        "  --version                       print the version\n"
        "\n"
        " logging\n"
        "  --log <level>                   set console log level\n"
        "  --file-log <level>              set file log level\n"
        "   <level>                        none, error, warn, info, debug\n"
        "  --no-log                        no console logging (--log none)\n"
        "  --no-file-log                   no file logging (--file-log none)\n"
        "\n"
        "  --start-minimized               start with hidden main window\n"
        "                                  (if tray icon enabled)\n"
#ifdef TESTING_SUPPORT
        "  --test <xml-file>               run unit test using xml-file\n"
#endif
    );
}

int CMain::exec()
{
    ASSERT_THROW(m_parameters, ENOT_INITIALIZED);
    int ret = 0;

    switch (m_cl_parse_result) {
        case Parse_none_found:
            processArgs();
            break;
        case Parse_unknown_command:
            wrongUsage("Unknown command: %s", m_parameters->getUnknownCommand().c_str());
            ret = -1;
            break;
        case Parse_success:
            if (m_parameters->getSwitch("help")) {
                printHelp();
            } else if (m_parameters->getSwitch("version")) {
                printVersion();
            } else {
                ret = processArgs();
            }
            break;
    }
    return ret;
}

void CMain::printVersion()
{
    printf("%s\n", getAppVersion().toStr().c_str());
}

void CMain::wrongUsage(const char* fmt, ...)
{
    printHelp();

    printf("\n ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

void CMain::loadTranslation()
{
    QLocale locale = QLocale::system();

    QString bin_path = qApp->applicationDirPath();
    QString src_app_trans_path = bin_path + QLatin1String("/translations");
    // installed translation path
    QString app_trans_path = bin_path + "/../share/" APP_NAME "/translations";
    QString qt_trans_path =
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#else
        QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#endif

    // Try to load it first from app path (in case there's an updated
    // translation), if it fails it will try then from the Qt path.
    if (!m_qt_trans.load(locale, "qt", "_", app_trans_path))
        (void)m_qt_trans.load(locale, "qt", "_", qt_trans_path);
    if (!m_qtbase_trans.load(locale, "qtbase", "_", app_trans_path))
        (void)m_qtbase_trans.load(locale, "qtbase", "_", qt_trans_path);

    if (!m_app_trans.load(locale, "translation", "_", src_app_trans_path)) {
        if (!m_app_trans.load(locale, "translation", "_", app_trans_path)) {
            if (!locale.name().startsWith("en"))
                LOG(LogLevel::Warn, "Failed to load translation %s",
                    locale.name().toUtf8().constData());
        } else {
            LOG(LogLevel::Debug, "Using translation from %s", app_trans_path.toUtf8().constData());
        }
    } else {
        LOG(LogLevel::Debug, "Using translation from %s", src_app_trans_path.toUtf8().constData());
    }

    qApp->installTranslator(&m_app_trans);
    qApp->installTranslator(&m_qt_trans);
    qApp->installTranslator(&m_qtbase_trans);
}

int CMain::processArgs()
{
    // set console log level
    string level;
    if (m_parameters->getSwitch("verbose"))
        CLog::getInstance().setConsoleLevel(LogLevel::Debug);
    else if (m_parameters->getSwitch("no-log"))
        CLog::getInstance().setConsoleLevel(LogLevel::None);
    else if (m_parameters->getParam("log", level)) {
        LogLevel log_level;
        if (CLog::parseLevel(level, log_level)) CLog::getInstance().setConsoleLevel(log_level);
    }
    // set file log level
    if (m_parameters->getSwitch("no-file-log"))
        CLog::getInstance().setFileLevel(LogLevel::None);
    else if (m_parameters->getParam("file-log", level)) {
        LogLevel log_level;
        if (CLog::parseLevel(level, log_level)) CLog::getInstance().setFileLevel(log_level);
    }

#ifdef TESTING_SUPPORT
    string test_file;
    if (m_parameters->getParam("test", test_file)) {
        LOG(LogLevel::Debug, "Running tests on file %s", test_file.c_str());
        UnitTests test(test_file);
        return QTest::qExec(&test);
    }
#endif

    bool start_minimized = m_parameters->getSwitch("start-minimized");

    QApplication app(m_argc, m_argv);
    loadTranslation();

    MainWindow main_window;
    if (!start_minimized || !main_window.trayIconEnabled()) main_window.show();
    return app.exec();
}
