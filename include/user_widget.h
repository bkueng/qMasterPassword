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

#ifndef HEADER_USER_WIDGET_H
#define HEADER_USER_WIDGET_H

#include <QDialog>
#include "user.h"
#include "identicon.h"

namespace Ui {
class UserWidget;
}

class UserWidget : public QDialog
{
	Q_OBJECT

public:
	enum Type {
		Type_edit,
		Type_new
	};
	explicit UserWidget(Type type, QWidget *parent = 0);
	~UserWidget();

	QString password() const;
	QString userName() const;

	void setData(const UiUser& user);
	void applyData(UiUser& user);

	bool checkPasswordOnLogin() const;

private:
	Ui::UserWidget *ui;
	Identicon m_identicon;
private slots:
	void checkInputValidity();
	void identiconHelp();
	void generatePasswordClicked();
};

#endif // HEADER_USER_WIDGET_H
