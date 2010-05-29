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

#include "global.h"


#include "version.h"

VERSION getAppVersion() {
	return(VERSION((int)VERSION_MAJOR, (int)VERSION_MINOR, (int)VERSION_PATCH));
}


string getDate() {
	char   timestr[20];
	time_t seconds= time(0);
	struct tm *ptm= localtime(&seconds);
	sprintf(timestr,"%02i.%02i.%02i",
	(int)ptm->tm_mday,	
	(int)ptm->tm_mon+1,
	(int)ptm->tm_year-100);
	return(timestr);

}
string getTime() {
	char   timestr[20];
	time_t seconds= time(0);
	struct tm *ptm= localtime(&seconds);
	sprintf(timestr,"%02i:%02i:%02i",
	(int)ptm->tm_hour,
	(int)ptm->tm_min,
	(int)ptm->tm_sec);
	return(timestr);
}


string toStr(int val) {
	char b[20];
	sprintf(b, "%i", val);
	return(b);
}


bool cmpInsensitive(const string& str1, const string& str2) {
	if(str1.length() != str2.length()) return(false);
	
	for(size_t i=0; i<str1.length(); ++i) {
		if(tolower(str1[i])!=tolower(str2[i])) return(false);
	}
	return(true);
}


string& toLower(string& str) {
	for(size_t i=0; i<str.length(); ++i) {
		if(str[i]>='A' && str[i]<='Z') str[i]+=32;
	}
	return(str);
}

string toLower(const string& str) {
	string ret=str;
	for(size_t i=0; i<str.length(); ++i) {
		if(ret[i]>='A' && ret[i]<='Z') ret[i]+=32;
	}
	return(ret);
}

string trim(const string& str) {
	size_t start_pos = str.find_first_not_of(" \t");
	if(start_pos==string::npos) start_pos=0;
	
	size_t end_pos=str.find_last_not_of(" \t");
	if(end_pos==string::npos) end_pos=str.length()-1;
	
	return(str.substr(start_pos, end_pos-start_pos+1));
}


string& replace(string& str, const string& find, const string& replace) {
	size_t pos=0;
	while((pos = str.find(find, pos)) != string::npos ) {
		str.replace(pos, find.length(), replace);
		pos+=replace.length();
	}
	return(str);
}


