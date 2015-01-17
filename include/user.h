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

/** @file user & site classes with streaming operators */

#ifndef _HEADER_USER_H_
#define _HEADER_USER_H_

#include "crypto.h"

#include <QString>
#include <QList>
#include <QMetaType>
#include <QDateTime>

typedef qint32 CategoryId;

/**
 ** class UiSite
 *
 */
class UiSite {
public:
	Site site;

	QList<CategoryId> category_ids;
	QString user_name;
	QString comment;

	QDateTime time_created;
	QDateTime time_edited;
private:
};

Q_DECLARE_METATYPE(UiSite)
QDataStream &operator<<(QDataStream &, const UiSite &);
QDataStream &operator>>(QDataStream &, UiSite &);


/**
 ** class UiUser
 */
class UiUser {
public:
	UiUser(const QString& user_name="") : m_user_name(user_name) {}

	const QList<UiSite*>& getSites() const { return m_sites; }
	QList<UiSite*>& getSites() { return m_sites; }

	const QString& getUserName() const { return m_user_name; }
	void setUserName(const QString& userName) { m_user_name = userName; }
private:
	QString m_user_name;
	QList<UiSite*> m_sites;
};

Q_DECLARE_METATYPE(UiUser)
QDataStream &operator<<(QDataStream &, const UiUser &);
QDataStream &operator>>(QDataStream &, UiUser &);

#endif /* _HEADER_USER_H_ */
