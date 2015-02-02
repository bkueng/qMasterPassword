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
#include <QMessageBox>

AddUser::AddUser(Type type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddUser)
{
	ui->setupUi(this);
	setFixedSize(size());
	checkInputValidity();

	if (type == Type_edit) {
		ui->txtUserName->setEnabled(false);
		setWindowTitle(tr("Edit User"));
	}
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

void AddUser::applyData(UiUser& user) {
	if (ui->chkCheckPassword->isChecked()) {
		try {
			user.userData().setStorePasswordHash(
				ui->txtPassword->text().toUtf8().constData());
		} catch(CryptoException& e) {
			QMessageBox::critical(this, tr("Cryptographic exception"),
				tr("Failed to generate password hash. password check will be disabled."));
			user.userData().disableStorePasswordHash();
		}
	} else {
		user.userData().disableStorePasswordHash();
	}
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

void AddUser::setData(const UiUser& user) {
	ui->txtUserName->setText(user.getUserName());
	ui->chkCheckPassword->setChecked(user.userData().storePasswordHash());
}
