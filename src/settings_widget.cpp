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

#include "settings_widget.h"
#include "ui_settings_widget.h"

#include <QFileDialog>
#include <QMessageBox>

SettingsWidget::SettingsWidget(ApplicationSettings& settings,
		QMap<QString, UiUser>& users, DataImportExport& import_export,
		QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWidget),
	m_settings(settings), m_users(users), m_import_export(import_export)
{
	ui->setupUi(this);
	for (const auto& user : m_users.keys()) {
		ui->cmbExportedUser->addItem(user);
	}
	if (m_users.isEmpty()) {
		ui->btnExportJson->setEnabled(false);
		ui->btnImportJson->setEnabled(false);
	}

	ui->chkShowPasswords->setChecked(m_settings.show_pw_after_login);
	ui->chkTrayIcon->setChecked(m_settings.show_systray_icon);
	ui->chkAutoLogout->setChecked(m_settings.auto_logout_when_hidden);
	ui->spnAutoLogoutTimeout->setValue(m_settings.auto_logout_timeout);
	updateUi();
}

SettingsWidget::~SettingsWidget()
{
	delete ui;
}

void SettingsWidget::showPWAfterLogin(bool show) {
	m_settings.show_pw_after_login = show;
}
void SettingsWidget::showTrayIcon(bool show) {
	m_settings.show_systray_icon = show;
	updateUi();
	emit showTrayIconChanged(show);
}

void SettingsWidget::autoLogout(bool activated) {
	m_settings.auto_logout_when_hidden = activated;
}

void SettingsWidget::autoLogoutValueChanged(int value) {
	m_settings.auto_logout_timeout = value;
}

void SettingsWidget::updateUi() {
	bool enabled = m_settings.show_systray_icon;
	ui->chkAutoLogout->setEnabled(enabled);
	ui->spnAutoLogoutTimeout->setEnabled(enabled);
	ui->lblTimeout->setEnabled(enabled);
	ui->lblMinutes->setEnabled(enabled);
	if (!enabled) ui->chkAutoLogout->setChecked(false);
}

void SettingsWidget::exportAsJsonClicked() {
	const UiUser* user = selectedUser();
	if (!user) return;

	//choose file name...
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("JSON (*.json)"));
	dialog.setViewMode(QFileDialog::Detail);
	QStringList file_names;
	if (dialog.exec()) {
		file_names = dialog.selectedFiles();
		if (file_names.isEmpty()) return;
		QString file_name = file_names[0];
		if (!file_name.endsWith(".json", Qt::CaseInsensitive))
			file_name += ".json";
		try {
			m_import_export.exportJson(*user, file_name);
		} catch(Exception& e) {
			switch (e.getError()) {
			case EFILE_ERROR:
				QMessageBox::critical(this, tr("Error Opening File"),
						tr("Could not write to file %1").arg(file_name));
				break;
			default:
				QMessageBox::critical(this, tr("Error"),
						tr("unknown error (%1) occurred").arg(e.getError()));
				break;
			}
		}
	}
}

void SettingsWidget::importFromJsonClicked() {
	UiUser* user = selectedUser();
	if (!user) return;

	//choose file name...
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("JSON (*.json)"));
	dialog.setViewMode(QFileDialog::Detail);
	QStringList file_names;
	if (dialog.exec()) {
		file_names = dialog.selectedFiles();
		if (file_names.isEmpty()) return;
		QString file_name = file_names[0];
		try {
			m_import_export.importJson(*user, file_name);
			emit dataImported(user->getUserName());
		} catch(Exception& e) {
			switch (e.getError()) {
			case EFILE_ERROR:
				QMessageBox::critical(this, tr("Error Opening File"),
						tr("Could not read from file %1").arg(file_name));
				break;
			default:
				QMessageBox::critical(this, tr("Error"),
						tr("unknown error (%1) occurred").arg(e.getError()));
				break;
			}
		}
	}
}

UiUser* SettingsWidget::selectedUser() {
	QString user_name = ui->cmbExportedUser->currentText();
	auto iter = m_users.find(user_name);
	if (iter == m_users.end())
		return nullptr;
	return &*iter;
}
