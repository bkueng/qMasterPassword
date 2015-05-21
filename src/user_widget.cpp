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

#include "user_widget.h"
#include "ui_user_widget.h"
#include "password_generator_widget.h"

#include <QPushButton>
#include <QMessageBox>
#include <QWhatsThis>
#include <QCursor>

UserWidget::UserWidget(Type type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UserWidget)
{
	ui->setupUi(this);
	setFixedSize(size());
	checkInputValidity();

	QFont label_font = ui->txtIdenticon->font();
	label_font.setPointSizeF(label_font.pointSizeF()*1.3f);
	ui->txtIdenticon->setFont(label_font);

	if (type == Type_edit) {
		ui->txtUserName->setEnabled(false);
		setWindowTitle(tr("Edit User"));
	}
}

UserWidget::~UserWidget()
{
	delete ui;
}

QString UserWidget::password() const {
	return ui->txtPassword->text();
}
QString UserWidget::userName() const {
	return ui->txtUserName->text();
}

void UserWidget::applyData(UiUser& user) {
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

void UserWidget::checkInputValidity() {
	QString password = ui->txtPassword->text();
	bool is_valid = ui->txtUserName->text().length() > 0
			&& password.length() > 0
			&& password == ui->txtPasswordRepeat->text();
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(is_valid);

	if (password.isEmpty()) {
		ui->txtIdenticon->setText("");
	} else {
		QColor identicon_color;
		QString identicon;
		m_identicon.setUserName(ui->txtUserName->text());
		m_identicon.getIdenticon(password, identicon, identicon_color);
		ui->txtIdenticon->setText(identicon);

		QPalette palette = ui->txtIdenticon->palette();
		palette.setColor(QPalette::Text, identicon_color);
		ui->txtIdenticon->setPalette(palette);
	}
}

bool UserWidget::checkPasswordOnLogin() const {
	return ui->chkCheckPassword->isChecked();
}

void UserWidget::setData(const UiUser& user) {
	ui->txtUserName->setText(user.getUserName());
	ui->chkCheckPassword->setChecked(user.userData().storePasswordHash());
}

void UserWidget::identiconHelp() {
	QWhatsThis::showText(QCursor::pos(),
		tr("<p>The identicon is a visual help calculated from the user name and the master password.</p>"
			"<p>Remember it and if it is the same when logging in, you most likely didn't make a typo.</p>"
			"<p>This avoids the need to explicitly store the hash of the password (password check option below).</p>"
			"<p>It can be disabled under Edit -> Settings.</p>"), this);
}

void UserWidget::generatePasswordClicked() {
	PasswordGeneratorWidget password_generator(this);
	password_generator.exec();
	QString password;
	password_generator.getPassword(password);
	if (!password.isEmpty()) {
		ui->txtPassword->setEchoMode(QLineEdit::Normal);
		ui->txtPassword->setText(password);
	}
}
