/*
 * Copyright (C) 2010-2011 Beat Küng <beat-kueng@gmx.net>
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


#include "exception.h"
#include "config.h"
#include "logging.h"

#include <cstdarg>
#include <cstdio>


/*********************************************************************//*
 * class Exception
 *//*********************************************************************/

bool Exception::m_bLog_exceptions = true;

Exception::Exception(EnErrors err) : m_bLogged(false), m_func(NULL)
	, m_file(NULL), m_line(0), m_err(err) {
	
}

Exception::Exception(EnErrors err, const char* func, const char* file, int line)
	: m_bLogged(false), m_func(func), m_file(file), m_line(line), m_err(err) {
	
}

Exception::~Exception() {
	if(m_bLog_exceptions) log();
}

void Exception::log() {
	if(!m_bLogged) {
		CLog::getInstance().Log(ERROR, m_file, m_func, m_line, "Exception: %s", getErrorStr().c_str());
		m_bLogged=true;
	}
}



string Exception::getErrorStr() const {
	switch(m_err) {
	case EGENERAL: return("EGENERAL");
	case EASSERT: return("EASSERT");
	case EPOOL: return("EPOOL");
	case ELIST: return("ELIST");
	case EOUT_OF_MEMORY: return("EOUT_OF_MEMORY");
	case ETIMEOUT: return("ETIMEOUT");
	case EUNABLE_TO_OPEN_FILE: return("EUNABLE_TO_OPEN_FILE");
	case EINVALID_PARAMETER: return("EINVALID_PARAMETER");
	case EDEVICE: return("EDEVICE");
	case ENOTHING_TO_ABORT: return("ENOTHING_TO_ABORT");
	case EDEVICE_BUSY: return("EDEVICE_BUSY");
	case ECANNOT_DELETE: return("ECANNOT_DELETE");
	case EBUFFER_TOO_SMALL: return("EBUFFER_TOO_SMALL");
	case EFILE_ERROR: return("EFILE_ERROR");
	case ECANNOT_UNLOAD: return("ECANNOT_UNLOAD");
	case ENR_OF_INSTANCES_EXHAUSTED: return("ENR_OF_INSTANCES_EXHAUSTED");
	case EFILE_PARSING_ERROR: return("EFILE_PARSING_ERROR");
	case EALREADY_INITIALIZED: return("EALREADY_INITIALIZED");
	case ENOT_INITIALIZED: return("ENOT_INITIALIZED");
	case ENO_SUCH_DEVICE: return("ENO_SUCH_DEVICE");
	case EUNABLE_TO_READ: return("EUNABLE_TO_READ");
	case ETRY_AGAIN: return("ETRY_AGAIN");
	case EINTERRUPTED: return("EINTERRUPTED");
	case EUNSUPPORTED: return("EUNSUPPORTED");
	case EWRONG_STATE: return("EWRONG_STATE");
	case EFILE_EXISTS: return("EFILE_EXISTS");
	default:;
	}
	return("(unknown Error)");
}


Exception* Exception::copy() const {
	return(new Exception(*this));
}


/*********************************************************************//*
 * class ExceptionString
 *//*********************************************************************/


ExceptionString::ExceptionString(EnErrors err, const char* func
		, const char* file, int line, const char* fmt, ...)
	: Exception(err, func, file, line) {
	
	va_list va;
	char buf[2048];
	va_start(va, fmt);
	vsprintf(buf, fmt, va);
	va_end(va);
	m_err_desc=buf;
	
}

void ExceptionString::log() {
	if(!m_bLogged) {
		CLog::getInstance().Log(ERROR, m_file, m_func, m_line, "%s", m_err_desc.c_str());
		m_bLogged=true;
	}
}

ExceptionString::~ExceptionString() {
	if(m_bLog_exceptions) log();
}

Exception* ExceptionString::copy() const {
	return(new ExceptionString(*this));
}

