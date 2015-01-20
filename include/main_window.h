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
#include <QPushButton>
#include <QItemSelection>
#include <QSortFilterProxyModel>

#include "crypto.h"
#include "user.h"
#include "ui_helpers.h"

namespace Ui {
class MainWindow;
}
class MainWindow;


class MySortFilterProxyModel : public QSortFilterProxyModel
{
public:
	MySortFilterProxyModel(MainWindow& main_window, QObject *parent = 0)
		: QSortFilterProxyModel(parent), m_main_window(main_window) {}

protected:
	bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
private:
	MainWindow& m_main_window;
};



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	CategoryId selectedCategory() const { return m_selected_category; }
	QStandardItemModel* model() const { return m_sites_model; }
private:
	Ui::MainWindow* m_ui;
	void login();
	void logout();
	void initSitesView();
	void addSiteToUI(UiSite& site);
	void clearSitesUI();
	TableItem* getSelectedItem();
	void updateModelItem(int row, const UiSite& site);
	void addCategory(const QString& name, CategoryId id=-1);
	void selectCategory(CategoryId category);
	void uiSitesTableChanged();

	MasterPassword m_master_password;
	QMap<QString, UiUser> m_users;
	QMap<CategoryId, QString> m_categories;
	CategoryId m_next_category_id = 0;
	CategoryId m_selected_category = -1;
	UiUser* m_current_user = nullptr; /** current logged in user */

	QStandardItemModel* m_sites_model;
	MySortFilterProxyModel* m_proxy_model;

	int m_copy_column_idx;

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
	void categoryButtonPressed();
	void filterTextChanged(QString filter_text);
public slots:
	void copyPWToClipboardClicked();

protected:
	void closeEvent(QCloseEvent *event);
};

#endif // MAIN_WINDOW_H
