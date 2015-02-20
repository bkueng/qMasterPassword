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
		QMap<QString, UiUser>& users, QMap<CategoryId, QString> categories,
		DataImportExport& import_export, QWidget *parent) :
	QDialog(parent), ui(new Ui::SettingsWidget),
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
	int clipboard_pw_timeout = m_settings.clipboard_pw_timeout;
	if (clipboard_pw_timeout > 0) {
		ui->chkClipboardTimeout->setChecked(true);
		ui->spnClipboardTimeout->setValue(clipboard_pw_timeout);
	} else {
		ui->chkClipboardTimeout->setChecked(false);
	}

	for (const auto& category : categories.keys()) {
		if (category != 0)
			ui->cmbCategories->addItem(categories[category]);
	}

	updateUi();
	categoryNameChanged();
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

void SettingsWidget::clipboardTimeoutChanged(int timeout) {
	if (ui->chkClipboardTimeout->isChecked()) {
		m_settings.clipboard_pw_timeout = timeout;
	}
}

void SettingsWidget::clipboardTimeoutClicked(bool activated) {
	if (activated) {
		m_settings.clipboard_pw_timeout = ui->spnClipboardTimeout->value();
	} else {
		m_settings.clipboard_pw_timeout = 0;
	}
}

void SettingsWidget::updateUi() {
	ui->btnRemoveCategory->setEnabled(ui->cmbCategories->count() != 0);
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

void SettingsWidget::removeSelectedCategoryClicked() {
	int current_index = ui->cmbCategories->currentIndex();
	if (current_index < 0) return;
	ui->cmbCategories->removeItem(current_index);
	updateUi();
}

void SettingsWidget::addNewCategoryClicked() {
	QString category_name = ui->txtCategory->text();
	if (!canAddCategory(category_name)) return;
	ui->cmbCategories->addItem(category_name);
	ui->cmbCategories->setCurrentIndex(ui->cmbCategories->count()-1);
	ui->txtCategory->setText("");
	updateUi();
}

void SettingsWidget::categoryNameChanged() {
	ui->btnAddCategory->setEnabled(canAddCategory(ui->txtCategory->text()));
}

bool SettingsWidget::canAddCategory(const QString& category_name) {
	bool can_add = !category_name.isEmpty();
	for (int i = 0; i < ui->cmbCategories->count(); ++i) {
		if (ui->cmbCategories->itemText(i) == category_name)
			can_add = false;
	}
	return can_add;
}

QList<QString> SettingsWidget::defaultCategories() {
	QList<QString> ret;
	ret.push_back(tr("Personal"));
	ret.push_back(tr("Work"));
	ret.push_back(tr("eShopping"));
	ret.push_back(tr("Social Networks"));
	ret.push_back(tr("Bank"));
	ret.push_back(tr("Forum"));
	ret.push_back(tr("eMail"));
	return ret;
}

QList<QString> SettingsWidget::categories() {
	QList<QString> ret;
	for (int i = 0; i < ui->cmbCategories->count(); ++i) {
		ret.push_back(ui->cmbCategories->itemText(i));
	}
	return ret;
}

void SettingsWidget::restoreDefaultCategories() {
	while (ui->cmbCategories->count() > 0)
		ui->cmbCategories->removeItem(0);
	QList<QString> categories = SettingsWidget::defaultCategories();
	for (const auto& category : categories) {
		ui->cmbCategories->addItem(category);
	}
	updateUi();
}
