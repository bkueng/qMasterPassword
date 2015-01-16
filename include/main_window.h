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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMap>
#include <vector>

#include "crypto.h"
#include "user.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow* m_ui;
	void login();
	void logout();

	MasterPassword m_master_password;
	QMap<QString, UiUser> m_users;
	UiUser* m_current_user = nullptr; /** current logged in user */

private slots:
	void loginLogoutClicked();
	void addUser();
	void deleteUser();
	void saveSettings();
	void readSettings();

protected:
	void closeEvent(QCloseEvent *event);
};

#endif // MAIN_WINDOW_H
