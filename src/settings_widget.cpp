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
#include <QFile>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

SettingsWidget::SettingsWidget(ApplicationSettings& settings,
		const QMap<QString, UiUser>& users, const QMap<CategoryId, QString>& categories,
		QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWidget),
	m_settings(settings), m_users(users), m_categories(categories)
{
	ui->setupUi(this);
	for (const auto& user : m_users.keys()) {
		ui->cmbExportedUser->addItem(user);
	}
	if (m_users.isEmpty()) ui->btnExportJson->setEnabled(false);

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
	QString user_name = ui->cmbExportedUser->currentText();
	auto iter = m_users.find(user_name);
	if (iter == m_users.end()) return;
	const UiUser& user = *iter;
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
		exportJson(user, file_name);
	}
}

void SettingsWidget::exportJson(const UiUser& user, const QString& file_name) {
	QFile save_file(file_name);

	if (!save_file.open(QIODevice::WriteOnly)) {
		qWarning("Couldn't open save file.");
		QMessageBox::warning(this, tr("Error Opening File"),
			tr("Could not open file %1").arg(file_name));
	}

	QJsonArray sites_array;
	for (const auto& site : user.getSites()) {
		QJsonObject site_object;
		UiSite s;
		if (!site->category_ids.isEmpty()) {
			const auto& category = m_categories.find(site->category_ids[0]);
			if (category != m_categories.end()) {
				site_object["category"] = category.value();
			}
		}
		site_object["passwordType"] = "Generated"+QString::fromUtf8(
				MPSiteTypeToString(site->site.getType()).c_str());
		site_object["siteName"] = QString::fromUtf8(site->site.getName().c_str());
		site_object["userName"] = site->user_name;
		site_object["siteCounter"] = QString::number(site->site.getCounter());
		site_object["comment"] = site->comment;
		sites_array.append(site_object);
	}
	QJsonDocument save_doc(sites_array);
	save_file.write(save_doc.toJson());
}
