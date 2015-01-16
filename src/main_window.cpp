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

#include "main_window.h"
#include "ui_main_window.h"
#include "add_user.h"
#include "logging.h"

#include <iostream>
using namespace std;

#include <QMessageBox>
#include <QSettings>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent), m_ui(new Ui::MainWindow) {
	m_ui->setupUi(this);
	readSettings();
	m_ui->btnDeleteUser->setEnabled(m_ui->cmbUserName->count() > 0);
}

MainWindow::~MainWindow() {
	delete m_ui;
}

void MainWindow::loginLogoutClicked() {
	if (m_master_password.isLoggedIn())
		logout();
	else
		login();
}

void MainWindow::login() {
	QString user_name = m_ui->cmbUserName->currentText();
	QString password = m_ui->txtPassword->text();
	auto iter_user = m_users.find(user_name);
	if (user_name.length() == 0 || password.length() == 0
			|| iter_user == m_users.end())
		return;

	try {
		m_current_user = &iter_user.value();
		m_master_password.logout();
		m_master_password.login(user_name.toStdString(), password.toStdString());
		m_ui->btnLoginLogout->setText(tr("Logout"));
		m_ui->cmbUserName->setEnabled(false);
		m_ui->txtPassword->setEnabled(false);
		//TODO: load sites

		//clear the password: no need to store it anymore
		m_ui->txtPassword->setText("");

	} catch(CryptoException& e) {
		QString error_msg;
		switch(e.type) {
		case CryptoException::Type_HMAC_SHA256_failed:
			error_msg = tr("HMAC SHA256 function failed");
			break;
		case CryptoException::Type_scrypt_failed:
			error_msg = tr("scrypt function failed");
			break;
		case CryptoException::Type_not_logged_in:
			error_msg = tr("not logged in");
			break;
		}
		QMessageBox::warning(this, tr("Cryptographic exception"),
			tr("Error: %1.\n"
			"Should it happen again, then something is seriously wrong with the program installation.").arg(error_msg));
	}
}

void MainWindow::logout() {
	m_master_password.logout();
	m_current_user = nullptr;
	m_ui->btnLoginLogout->setText(tr("Login"));
	m_ui->txtPassword->setText("");
	m_ui->cmbUserName->setEnabled(true);
	m_ui->txtPassword->setEnabled(true);
	//TODO: clear sites
}

void MainWindow::addUser() {
	AddUser add_user(this);
	if (add_user.exec() == 1) { //accepted
		QString user_name = add_user.userName();
		if (m_users.find(user_name) != m_users.end()) {
			QMessageBox::warning(this, tr("User exists"),
					tr("Error: user already exists."));
			return;
		}
		UiUser user(user_name);
		m_users.insert(user_name, user);
		m_ui->txtPassword->setText(add_user.password());
		m_ui->cmbUserName->addItem(user_name);
		m_ui->cmbUserName->setCurrentIndex(m_ui->cmbUserName->count() - 1);
		m_ui->btnDeleteUser->setEnabled(true);
		login();
	}
}

void MainWindow::deleteUser() {
	int selected = m_ui->cmbUserName->currentIndex();
	if (selected >= 0) {
		QString user_name = m_ui->cmbUserName->itemText(selected);
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, tr("Delete User"),
				tr("Do you really want to delete the user %1?").arg(user_name),
				QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			logout();
			m_ui->cmbUserName->removeItem(selected);
			m_users.remove(user_name);
		}
	}
	if (m_ui->cmbUserName->count() == 0)
		m_ui->btnDeleteUser->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	saveSettings();
    QMainWindow::closeEvent(event);
}
void MainWindow::saveSettings() {
    QSettings settings("qMasterPassword", "qMasterPassword");
    settings.setValue("main_window/geometry", saveGeometry());
    settings.setValue("main_window/window_state", saveState());
    settings.setValue("main_window/selected_user", m_ui->cmbUserName->currentText());
    QByteArray user_data;
    QDataStream stream(&user_data, QIODevice::ReadWrite);
    stream << m_users;
    settings.setValue("data/users", user_data);
}
void MainWindow::readSettings() {
    QSettings settings("qMasterPassword", "qMasterPassword");
    restoreGeometry(settings.value("main_window/geometry").toByteArray());
    restoreState(settings.value("main_window/window_state").toByteArray());
    QDataStream stream(settings.value("data/users").toByteArray());
    QString selected_user = settings.value("main_window/selected_user").toString();
    stream >> m_users;
    int selected_index = -1;
	for (auto& user : m_users) {
		LOG(DEBUG, "Read user: %s (%i sites)",
			user.getUserName().toStdString().c_str(), user.getSites().count());

		m_ui->cmbUserName->addItem(user.getUserName());
		if (user.getUserName() == selected_user)
			selected_index = m_ui->cmbUserName->count() - 1;
    }
	if (selected_index != -1)
		m_ui->cmbUserName->setCurrentIndex(selected_index);
	if (!m_users.isEmpty())
		m_ui->txtPassword->setFocus();
}
