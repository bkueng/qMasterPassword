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
#include <memory>

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
	QString url;
	QString comment;

	QDateTime time_created;
	QDateTime time_edited;

	bool password_visible = true;
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
	UiUser(const QString& user_name="")
		: m_user(std::string(user_name.toUtf8().constData())) {}

	const QList<std::shared_ptr<UiSite>>& getSites() const { return m_sites; }
	QList<std::shared_ptr<UiSite>>& getSites() { return m_sites; }

	QString getUserName() const {
		return QString::fromUtf8(m_user.getUserName().c_str()); }
	void setUserName(const QString& userName) {
		m_user.setUserName(userName.toUtf8().constData()); }

	const User& userData() const { return m_user; }
	User& userData() { return m_user; }
private:
	User m_user;
	QList<std::shared_ptr<UiSite>> m_sites;
};

Q_DECLARE_METATYPE(UiUser)
QDataStream &operator<<(QDataStream &, const UiUser &);
QDataStream &operator>>(QDataStream &, UiUser &);

#endif /* _HEADER_USER_H_ */
