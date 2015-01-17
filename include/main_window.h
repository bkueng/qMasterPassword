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
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMap>
#include <QList>
#include <QItemSelection>

#include "crypto.h"
#include "user.h"

namespace Ui {
class MainWindow;
}

class TableItem : public QStandardItem {
public:
	TableItem(UiSite& site, const QString& item_text)
		: QStandardItem(item_text), m_site(site) {}

	UiSite& site() { return m_site; }
private:
	UiSite& m_site;
};

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
	void initSitesView();
	void addSiteToUI(UiSite& site);
	void clearSitesUI();
	TableItem* getSelectedItem();
	void updateModelItem(int row, const UiSite& site);

	MasterPassword m_master_password;
	QMap<QString, UiUser> m_users;
	QMap<CategoryId, QString> m_categories;
	UiUser* m_current_user = nullptr; /** current logged in user */

	QStandardItemModel* m_sites_model;

private slots:
	void loginLogoutClicked();
	void addUser();
	void deleteUser();
	void addSite();
	void deleteSite();
	void editSite();
	void saveSettings();
	void readSettings();
	void enableUI(bool logged_in);
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

protected:
	void closeEvent(QCloseEvent *event);
};

#endif // MAIN_WINDOW_H
