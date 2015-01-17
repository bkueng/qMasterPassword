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
	stream << (qint8)0; //stream version
	stream << site.comment;
	stream << QString::fromStdString(site.site.getContext());
	stream << (qint32)site.site.getCounter();
	stream << QString::fromStdString(site.site.getName());
	stream << (qint32)site.site.getType();
	stream << site.user_name;
	stream << (qint32)site.site.getVariant();
	stream << site.category_ids;
	stream << site.time_created;
	stream << site.time_edited;
	return stream;
}

QDataStream& operator >>(QDataStream& stream, UiSite& site) {
	QString context, name;
	qint32 counter, type, variant;
	qint8 version;
	stream >> version;
	DEBUG_ASSERT1(version == 0);
	stream >> site.comment >> context >> counter >> name >> type
			>> site.user_name >> variant >> site.category_ids
			>> site.time_created >> site.time_edited;
	site.site.setContext(context.toStdString());
	site.site.setCounter((uint32_t)counter);
	site.site.setName(name.toStdString());
	site.site.setType((MPSiteType)type);
	site.site.setVariant((MPSiteVariant)variant);
	return stream;
}

QDataStream& operator <<(QDataStream& stream, const UiUser& user) {
	stream << (qint8)0; //stream version
	stream << user.getUserName();
	stream << (qint32)user.getSites().count();
	for (const auto& site : user.getSites()) {
		stream << *site;
	}
	return stream;
}

QDataStream& operator >>(QDataStream& stream, UiUser& user) {
	qint8 version;
	qint32 site_count;
	QString user_name;
	stream >> version;
	DEBUG_ASSERT1(version == 0);
	stream >> user_name >> site_count;
	for (int i = 0; i < site_count; ++i) {
		UiSite* site = new UiSite();
		stream >> *site;
		user.getSites().push_back(site);
	}
	user.setUserName(user_name);
	return stream;
}
