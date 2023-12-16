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

#include "command_line.h"

#include "exception.h"

/*********************************************************************//*
 * class Exception
 *//*********************************************************************/

SCLParam* SCLTask::findParam(const string& name)
{
    map<string, SCLParam>::iterator iter = params.find(name);
    return iter == params.end() ? NULL : &iter->second;
}

SCLParam* SCLTask::findParam(char short_name)
{
    for (map<string, SCLParam>::iterator iter = params.begin(); iter != params.end(); ++iter) {
        if (iter->second.short_name == short_name) return &iter->second;
    }
    return NULL;
}

SCLSwitch* SCLTask::findSwitch(const string& name)
{
    map<string, SCLSwitch>::iterator iter = switches.find(name);
    return iter == switches.end() ? NULL : &iter->second;
}

SCLSwitch* SCLTask::findSwitch(char short_name)
{
    for (map<string, SCLSwitch>::iterator iter = switches.begin(); iter != switches.end(); ++iter) {
        if (iter->second.short_name == short_name) return &iter->second;
    }
    return NULL;
}

CCommandLineParser::CCommandLineParser(int argc, char* argv[])
    : m_unknown_command(""), m_cur_task(NULL), m_argc(argc - 1), m_parse_result(Parse_none_found)
{
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        size_t pos = arg.find('=');
        if (pos == string::npos || arg[0] != '-') {
            m_args.push_back(arg);
        } else {
            if (pos == 0 || pos == arg.length() - 1) {
                unknownCommand(arg);
                i = argc;
            } else {
                m_args.push_back(arg.substr(0, pos));
                m_args.push_back(arg.substr(pos + 1));
                ++m_argc;
            }
        }
    }
    m_args.push_back("");
}

CCommandLineParser::~CCommandLineParser()
{
}

void CCommandLineParser::addTask(const string& name, char short_name)
{
    if (name.length() > 0) {
        m_tasks[name].short_name = short_name;
    }
}

void CCommandLineParser::addParam(const string& name, char short_name, const string& default_val,
                                  const string& task_name)
{
    map<string, SCLParam>* param = &m_global.params;
    if (task_name.length() > 0) {
        map<string, SCLTask>::iterator iter = m_tasks.find(task_name);
        ASSERT_THROW_e(iter != m_tasks.end(), EINVALID_PARAMETER, "Task name %s not found",
                       task_name.c_str());
        param = &iter->second.params;
    }

    // insert in param
    SCLParam& p = (*param)[name];
    p.short_name = short_name;
    p.def_val = default_val;
}

void CCommandLineParser::addSwitch(const string& name, char short_name, const string& task_name)
{
    map<string, SCLSwitch>* s = &m_global.switches;
    if (task_name.length() > 0) {
        map<string, SCLTask>::iterator iter = m_tasks.find(task_name);
        ASSERT_THROW_e(iter != m_tasks.end(), EINVALID_PARAMETER, "Task name %s not found",
                       task_name.c_str());
        s = &iter->second.switches;
    }
    SCLSwitch nswitch;
    nswitch.short_name = short_name;
    s->insert(make_pair(name, nswitch));
}

ECLParsingResult CCommandLineParser::parse(bool bAllow_files)
{
    if (m_parse_result != Parse_none_found) return m_parse_result;
    if (m_args.size() < 2) return Parse_none_found;

    m_cur_task = NULL;
    SCLTask* task;

    for (int i = 0; i < m_argc; ++i) {
        const string& arg = m_args[i];
        if (arg.length() > 1 && arg[0] == '-') {
            if (arg[1] == '-') {  // name
                string arg_name = arg.substr(2);
                if ((task = findTask(arg_name))) {
                    m_cur_task = task;
                    task->bGiven = true;
                } else {
                    SCLSwitch* s = NULL;
                    SCLParam* p = NULL;
                    if (m_cur_task) {
                        s = m_cur_task->findSwitch(arg_name);
                        p = m_cur_task->findParam(arg_name);
                    }
                    if (!s) s = m_global.findSwitch(arg_name);
                    if (!p) p = m_global.findParam(arg_name);

                    if (s) {
                        s->bGiven = true;
                    } else if (p) {
                        p->values.push(m_args[i + 1]);
                        ++i;
                    } else {
                        return unknownCommand(arg);
                    }
                }
            } else {  // short name
                for (size_t k = 1; k < arg.length(); ++k) {
                    char arg_name = arg[k];
                    if ((task = findTask(arg_name))) {
                        m_cur_task = task;
                        task->bGiven = true;
                    } else {
                        SCLSwitch* s = NULL;
                        SCLParam* p = NULL;
                        if (m_cur_task) {
                            s = m_cur_task->findSwitch(arg_name);
                            p = m_cur_task->findParam(arg_name);
                        }
                        if (!s) s = m_global.findSwitch(arg_name);
                        if (!p) p = m_global.findParam(arg_name);

                        if (s) {
                            s->bGiven = true;
                        } else if (p && k == arg.length() - 1) {
                            p->values.push(m_args[i + 1]);
                            ++i;
                        } else {
                            return unknownCommand(string("") + arg_name);
                        }
                    }
                }
            }
        } else {
            if (!bAllow_files) return unknownCommand(arg);
            m_files.push_back(arg);
        }
    }

    m_cur_task = NULL;

    return m_parse_result = Parse_success;
}

ECLParsingResult CCommandLineParser::unknownCommand(const string& command)
{
    m_unknown_command = command;
    return m_parse_result = Parse_unknown_command;
}

SCLTask* CCommandLineParser::findTask(const string& name)
{
    map<string, SCLTask>::iterator iter = m_tasks.find(name);
    return iter == m_tasks.end() ? NULL : &iter->second;
}

SCLTask* CCommandLineParser::findTask(char short_name)
{
    for (map<string, SCLTask>::iterator iter = m_tasks.begin(); iter != m_tasks.end(); ++iter) {
        if (iter->second.short_name == short_name) return &iter->second;
    }
    return NULL;
}

const SCLTask* CCommandLineParser::setTask(const string& name)
{
    m_cur_task = findTask(name);
    if (name.length() > 0)
        ASSERT_THROW_e(m_cur_task, EINVALID_PARAMETER, "Parameter %s not found", name.c_str());
    return m_cur_task;
}

bool CCommandLineParser::getSwitch(const string& name)
{
    SCLSwitch* s = NULL;
    if (m_cur_task) s = m_cur_task->findSwitch(name);
    if (!s) s = m_global.findSwitch(name);
    ASSERT_THROW_e(s, EINVALID_PARAMETER, "switch %s not found", name.c_str());
    return s->bGiven;
}

bool CCommandLineParser::getParam(const string& name, string& val)
{
    SCLParam* p = NULL;
    if (m_cur_task) p = m_cur_task->findParam(name);
    if (!p) p = m_global.findParam(name);
    ASSERT_THROW(p, EINVALID_PARAMETER);
    if (p->values.empty()) {
        val = p->def_val;
        return false;
    }
    val = p->values.front();
    p->values.pop();
    return true;
}

const string& CCommandLineParser::getParamDefault(const string& name)
{
    SCLParam* p = NULL;
    if (m_cur_task) p = m_cur_task->findParam(name);
    if (!p) p = m_global.findParam(name);
    ASSERT_THROW(p, EINVALID_PARAMETER);
    return p->def_val;
}
