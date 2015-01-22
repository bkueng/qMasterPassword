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

SettingsWidget::SettingsWidget(ApplicationSettings& settings, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWidget),
	m_settings(settings)
{
	ui->setupUi(this);

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
