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

#ifndef EDIT_SITE_WIDGET_H
#define EDIT_SITE_WIDGET_H

#include <QDialog>
#include "user.h"
#include "crypto.h"

namespace Ui {
class EditSiteWidget;
}

class EditSiteWidget : public QDialog
{
	Q_OBJECT

public:
	enum Type {
		Type_edit,
		Type_new
	};
	explicit EditSiteWidget(UiSite& site, Type type, QWidget *parent = 0);
	~EditSiteWidget();

	/** set the input data to the object */
	void applyData();

private:
	Ui::EditSiteWidget *ui;
	UiSite& m_site;
	const Type m_type;
	MasterPassword m_sample_password;

private slots:
	void passwordTypeChanged(int new_type);
	void checkInputValidity();
};

#endif // EDIT_SITE_WIDGET_H
