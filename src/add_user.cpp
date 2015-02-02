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

#include "add_user.h"
#include "ui_add_user.h"

#include <QPushButton>

AddUser::AddUser(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddUser)
{
	ui->setupUi(this);
	setFixedSize(size());
	checkInputValidity();
}

AddUser::~AddUser()
{
	delete ui;
}

QString AddUser::password() const {
	return ui->txtPassword->text();
}
QString AddUser::userName() const {
	return ui->txtUserName->text();
}

void AddUser::checkInputValidity() {
	bool is_valid = ui->txtUserName->text().length() > 0
			&& ui->txtPassword->text().length() > 0
			&& ui->txtPassword->text() == ui->txtPasswordRepeat->text();
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(is_valid);
}

bool AddUser::checkPasswordOnLogin() const {
	return ui->chkCheckPassword->isChecked();
}
