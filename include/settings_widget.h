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

#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QDialog>
#include <QMap>
#include <QString>
#include "app_settings.h"
#include "user.h"
#include "import_export.h"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsWidget(ApplicationSettings& settings,
			QMap<QString, UiUser>& users,
			DataImportExport& import_export,
			QWidget *parent = 0);
	~SettingsWidget();

signals:
	void dataImported(QString user_name);

private:
	UiUser* selectedUser();
	void updateUi();
	Ui::SettingsWidget *ui;
	ApplicationSettings& m_settings;
	QMap<QString, UiUser>& m_users;
	DataImportExport& m_import_export;
private slots:
	void showPWAfterLogin(bool show);
	void showTrayIcon(bool show);
	void autoLogout(bool activated);
	void autoLogoutValueChanged(int value);
	void exportAsJsonClicked();
	void importFromJsonClicked();
	void clipboardTimeoutChanged(int timeout);
	void clipboardTimeoutClicked(bool activated);
signals:
	void showTrayIconChanged(bool visible);
};

#endif // SETTINGS_WIDGET_H
