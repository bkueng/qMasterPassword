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

#ifndef MAIN_CLASS_H_
#define MAIN_CLASS_H_

#include "global.h"
#include "command_line.h"


/*////////////////////////////////////////////////////////////////////////////////////////////////
 ** class CMain
 * main class with the main task and command line parser
/*////////////////////////////////////////////////////////////////////////////////////////////////


class CMain {
public:
	CMain();
	~CMain();
	
	/* parse the command line parameters */
	void init(int argc, char *argv[]);
	
	void exec();
	
private:
	void parseCommandLine(int argc, char *argv[]);
	void printHelp();
	void wrongUsage(const char* fmt, ...);
	void printVersion();
	
	void processArgs();
	
	
	CCommandLineParser* m_parameters;
	ECLParsingResult m_cl_parse_result;
};



#endif /* MAIN_CLASS_H_ */
