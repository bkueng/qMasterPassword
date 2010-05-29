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

#include "main_class.h"
#include "version.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

/*////////////////////////////////////////////////////////////////////////////////////////////////
 ** class CMain
/*////////////////////////////////////////////////////////////////////////////////////////////////


CMain::CMain() : m_parameters(NULL), m_cl_parse_result(Parse_none_found) {
	
}

CMain::~CMain() {
	SAVE_DEL(m_parameters);
}



void CMain::init(int argc, char *argv[]) {
	parseCommandLine(argc, argv);
	
}

void CMain::parseCommandLine(int argc, char *argv[]) {
	
	SAVE_DEL(m_parameters);
	m_parameters=new CCommandLineParser(argc, argv);
	
	//init known arguments
	m_parameters->addSwitch("help", 'h');
	m_parameters->addSwitch("version");
	m_parameters->addSwitch("verbose", 'v');
	
	
	//m_parameters->addTask("main task", 't');
	//m_parameters->addParam("output", 'o', "", "main task");
	//m_parameters->addSwitch("new-only", 'n', "main task");
	
	
	m_cl_parse_result=m_parameters->parse();
	
}

void CMain::printHelp() {
	printf("Usage:\n"
		" "APP_NAME" [-v] \n"
		" "APP_NAME" --version\n"
		"  -v, --verbose                   print debug messages\n"
		"  -h, --help                      print this message\n"
		"  --version                       print the version\n"
		);
}


void CMain::exec() {
	
	ASSERT_THROW(m_parameters, ENOT_INITIALIZED);
	
	switch(m_cl_parse_result) {
	case Parse_none_found:
		printHelp();
		break;
	case Parse_unknown_command:
		wrongUsage("Unknown command: %s", m_parameters->getUnknownCommand().c_str());
		break;
	case Parse_success:
		if(m_parameters->getSwitch("help")) {
			printHelp();
		} else if(m_parameters->getSwitch("version")) {
			printVersion();
		} else {
			processArgs();
		}
		break;
	}
}

void CMain::printVersion() {
	printf("%s\n", getAppVersion().toStr().c_str());
}

void CMain::wrongUsage(const char* fmt, ...) {
	
	printHelp();
	
	printf("\n ");
	
	va_list args;
	va_start (args, fmt);
	vprintf(fmt, args);
	va_end (args);
	
	printf("\n");
	
}


void CMain::processArgs() {
	
	if(m_parameters->getSwitch("verbose")) CLog::getInstance().setConsoleLevel(DEBUG);
	
	
	string device;
	if(m_parameters->getParam("device", device)) {
		//device set
	} else {
		//device is default value
	}
	
	if(m_parameters->setTask("main task")->bGiven) {
		
	}
	
}










