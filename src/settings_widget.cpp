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
	emit showTrayIconChanged(show);
}
