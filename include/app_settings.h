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

/** @file application settings */

#ifndef _HEADER_APP_SETTINGS_H_
#define _HEADER_APP_SETTINGS_H_

#include <QSettings>

/**
 ** class ApplicationSettings
 */
class ApplicationSettings {
public:

	bool show_systray_icon = false;
	bool show_pw_after_login = true;

	bool auto_logout_when_hidden = false;
	int auto_logout_timeout = 0; //[min]

	void saveSettings(QSettings& settings) {
		settings.setValue("app_settings/show_systray_icon", show_systray_icon);
		settings.setValue("app_settings/show_password_after_login", show_pw_after_login);
		settings.setValue("app_settings/auto_logout_when_hidden", auto_logout_when_hidden);
		settings.setValue("app_settings/auto_logout_timeout", auto_logout_timeout);
	}
	void restoreSettings(QSettings& settings) {
		show_systray_icon = settings.value("app_settings/show_systray_icon",
				show_systray_icon).toBool();
		show_pw_after_login = settings.value("app_settings/show_password_after_login",
				show_pw_after_login).toBool();
		auto_logout_when_hidden = settings.value("app_settings/auto_logout_when_hidden",
				auto_logout_when_hidden).toBool();
		auto_logout_timeout = settings.value("app_settings/auto_logout_timeout",
				auto_logout_timeout).toInt();
	}

private:
};

#endif /* _HEADER_APP_SETTINGS_H_ */
