/*
 * Copyright (C) 2014 Beat Küng <beat-kueng@gmx.net>
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


#include "user.h"
#include <QDataStream>
using namespace std;

QDataStream& operator <<(QDataStream& stream, const UiSite& site) {
	stream << QString::fromStdString(site.site.getComment());
	stream << QString::fromStdString(site.site.getContext());
	stream << (qint32)site.site.getCounter();
	stream << QString::fromStdString(site.site.getName());
	stream << (qint32)site.site.getType();
	stream << QString::fromStdString(site.site.getUserName());
	stream << (qint32)site.site.getVariant();
	return stream;
}

QDataStream& operator >>(QDataStream& stream, UiSite& site) {
	QString comment, context, name, user_name;
	qint32 counter, type, variant;
	stream >> comment >> context >> counter >> name >> type >> user_name
			>> variant;
	site.site.setComment(comment.toStdString());
	site.site.setContext(context.toStdString());
	site.site.setCounter((uint32_t)counter);
	site.site.setName(name.toStdString());
	site.site.setType((MPSiteType)type);
	site.site.setUserName(user_name.toStdString());
	site.site.setVariant((MPSiteVariant)variant);
	return stream;
}

QDataStream& operator <<(QDataStream& stream, const UiUser& user) {
	stream << user.getUserName();
	stream << user.getSites();
	return stream;
}

QDataStream& operator >>(QDataStream& stream, UiUser& user) {
	QString user_name;
	stream >> user_name >> user.getSites();
	user.setUserName(user_name);
	return stream;
}
