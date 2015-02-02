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

#ifndef ADD_USER_H
#define ADD_USER_H

#include <QDialog>
#include "user.h"

namespace Ui {
class AddUser;
}

class AddUser : public QDialog
{
	Q_OBJECT

public:
	enum Type {
		Type_edit,
		Type_new
	};
	explicit AddUser(Type type, QWidget *parent = 0);
	~AddUser();

	QString password() const;
	QString userName() const;

	void setData(const UiUser& user);
	void applyData(UiUser& user);

	bool checkPasswordOnLogin() const;

private:
	Ui::AddUser *ui;
private slots:
	void checkInputValidity();
};

#endif // ADD_USER_H
