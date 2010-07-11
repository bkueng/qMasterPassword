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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "config.h"

#include <cstdio>
#include <string>
#include <cstdarg>
using namespace std;

enum EnErrors {
	SUCCESS = 0,
	EGENERAL,
	EASSERT,
	EPOOL,
	ELIST,
	EOUT_OF_MEMORY,
	ETIMEOUT,
	EUNABLE_TO_OPEN_FILE,
	EINVALID_PARAMETER,
	EDEVICE,
	ENOTHING_TO_ABORT,
	EDEVICE_BUSY,
	ECANNOT_DELETE,
	EBUFFER_TOO_SMALL,
	EFILE_ERROR,
	ECANNOT_UNLOAD,
	ENR_OF_INSTANCES_EXHAUSTED,
	EFILE_PARSING_ERROR,
	EALREADY_INITIALIZED,
	ENOT_INITIALIZED,
	ENO_SUCH_DEVICE,
	EUNABLE_TO_READ,
	ETRY_AGAIN,
	EINTERRUPTED,
	EUNSUPPORTED,
	EFAILED_TO_LOAD
};



/*********************************************************************//*!
 * @class Exception
 * 
 * @brief general exception class with logging functionality
 *//*********************************************************************/

#define EXCEPTION(err) \
	Exception(err, __FUNCTION__, __FILE__, __LINE__)


class Exception {
public:
	Exception(EnErrors err, const char* func, const char* file, int line);
	virtual ~Exception();
	
	virtual EnErrors getError() { return(m_err); }
	virtual string getErrorStr(); //description of the error number
	
protected:
	Exception(EnErrors err); //this constructor does not log anything
private:
	EnErrors m_err;
};



/*********************************************************************//*!
 * @class ExceptionString
 * 
 * @brief exception class with a user defined error description
 *//*********************************************************************/

#define EXCEPTION_s(err, err_fmt, ...) \
	ExceptionString(err, __FUNCTION__, __FILE__, __LINE__, err_fmt, ## __VA_ARGS__)


class ExceptionString : public Exception {
public:
	ExceptionString(EnErrors err, const char* func
			, const char* file, int line, const char* fmt, ...);
	virtual ~ExceptionString();
	
	virtual string getErrorStr() { return(m_err_desc); }
private:
	string m_err_desc;
};



#define THROW(err) throw(EXCEPTION(err))
#define THROW_s(err, err_fmt, ...) throw(EXCEPTION_s(err, err_fmt, ## __VA_ARGS__)) 

#define ASSERT_THROW(exp, err) if(!(exp)) throw(EXCEPTION(err))
#define ASSERT_THROW_s(exp, err_fmt, ...) \
	if(!(exp)) throw(EXCEPTION_s(EASSERT, err_fmt, ## __VA_ARGS__)) 
#define ASSERT_THROW_e(exp, err, err_fmt, ...) \
	if(!(exp)) throw(EXCEPTION_s(err, err_fmt, ## __VA_ARGS__)) 

#endif /* EXCEPTION_H_ */


