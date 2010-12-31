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

#ifndef CONFIG_H_
#define CONFIG_H_


//#define APP_NAME ""

#ifndef APP_NAME
#error "APP_NAME must be defined. either in config.h or as a compiler define"
#endif


//this file contains defines to change the behaviour of the application
//most of the defines in here are project/OS specific


#if defined(WIN32) || defined(WIN64)
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif


#define LOG_FILE "."PATH_SEP""APP_NAME".log"







#endif /* CONFIG_H_ */
