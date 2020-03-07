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

#ifndef COMMAND_LINE_H_
#define COMMAND_LINE_H_

#include <map>
#include <string>
#include <vector>
#include <queue>
using namespace std;

/*********************************************************************//*
 * class CCommandLineParser
 *
 * generic class to parse the command line arguments
 *
 * argument format:
 * --<name> <value> or -<short_name> <value>
 * --<name>=<value> or -<short_name>=<value>
 * switches can be connected:
 * -<short_name switch1><short_name switch2><short_name param1> <value of param1>
 *
 * the arguments are double-staged:
 * there are global parameter and switches, but also task specific params and switches
 * so a command line call has the following form:
 * <binary> <global switches/params> <task> <task specific switches/params> or <global switches/params> <task> ...
 *
 * usage:
 * - init the object with the command line arguments
 * - set the known arguments and swiches
 * - parse command line and evaluate the return value
 *
 *
 * Known Issues:
 * - it is not checked whether a parameter/switch/task name is used twice
 *//*********************************************************************/

enum ECLParsingResult {
	Parse_success = 0,
	Parse_none_found,
	Parse_unknown_command
};

struct SCLSwitch {
	SCLSwitch() : bGiven(false) {}
	
	char short_name;
	
	bool bGiven;
};

struct SCLParam {
	SCLParam() {}
	
	char short_name;
	
	queue<string> values; //can be multiple
	string def_val;
};

struct SCLTask {
	SCLTask() : bGiven(false) {}
	
	char short_name;
	
	bool bGiven;
	map<string, SCLParam> params;
	map<string, SCLSwitch> switches;
	
	SCLParam* findParam(const string& name);
	SCLParam* findParam(char short_name);
	
	SCLSwitch* findSwitch(const string& name);
	SCLSwitch* findSwitch(char short_name);
};

class CCommandLineParser
{
public:
	CCommandLineParser(int argc, char* argv[]);
	~CCommandLineParser();
	
	//argument initialization
	void addTask(const string& name, char short_name = ' ');
	//if task_name is "" then it is global
	void addParam(const string& name, char short_name = ' ',
				  const string& default_val = "", const string& task_name = "");
	void addSwitch(const string& name, char short_name = ' ',
				   const string& task_name = "");
				   
	//parse the arguments
	//bAllow_files: if true: arguments without -- or - are interpreted as files
	//  and can be queried with getFiles()
	ECLParsingResult parse(bool bAllow_files = false);
	
	//set the current task and return it if found, NULL otherwise
	//if name is "", task will be unset
	const SCLTask* setTask(const string& name);
	//searches globally and also in current task if set
	bool getSwitch(const string& name);
	//sets val to first element on queue and pops it. next call returns next element,
	//until queue is empty, then false is returned and val is set to default value
	bool getParam(const string& name, string& val);
	const string& getParamDefault(const string& name);
	//todo: int/float overloading?
	
	
	const vector<string>& getFiles() const { return m_files; }
	
	const string& getUnknownCommand() const { return m_unknown_command; }
private:
	ECLParsingResult unknownCommand(const string& command);
	
	SCLTask* findTask(const string& name);
	SCLTask* findTask(char short_name);
	
	map<string, SCLTask> m_tasks;
	SCLTask m_global;
	
	vector<string> m_files;
	
	string m_unknown_command;
	SCLTask* m_cur_task;
	
	/* contains an empty element at the end to avoid index checks */
	vector<string> m_args;
	
	int m_argc;
	ECLParsingResult m_parse_result;
};





#endif /* COMMAND_LINE_H_ */
