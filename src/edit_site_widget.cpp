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

#include "edit_site_widget.h"
#include "ui_edit_site_widget.h"
#include <cstdlib>
#include <QPushButton>

EditSiteWidget::EditSiteWidget(const QMap<CategoryId, QString>& categories,
		UiSite& site, Type type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditSiteWidget), m_site(site), m_type(type),
	m_categories(categories)
{
	ui->setupUi(this);
	m_sample_password.login("test", "topsecret");

	for (int i = 0; i < MPSiteTypeCount(); ++i) {
		std::string str_type = MPSiteTypeToString(MPSiteTypeFromIdx(i));
		ui->cmbPasswordType->addItem(QString::fromUtf8(str_type.c_str()));
	}

	for (auto iter = m_categories.begin(); iter != m_categories.end(); ++iter) {
		if (iter.key() != 0) {
			CategoryCheckbox* checkbox = new CategoryCheckbox(iter.key(), iter.value());
			checkbox->setChecked(m_site.category_ids.contains(iter.key()));
			ui->layoutCategories->addWidget(checkbox);
		}
	}
	setFixedSize(sizeHint());

	switch(type) {
	case Type_edit:
		setWindowTitle(tr("Edit Site"));
		ui->txtSiteName->setText(QString::fromUtf8(m_site.site.getName().c_str()));
		ui->txtUserName->setText(m_site.user_name);
		ui->txtComment->setText(m_site.comment);
		ui->spnSiteCounter->setValue(m_site.site.getCounter());
		break;
	case Type_new:
		setWindowTitle(tr("New Site"));
		break;
	}
	ui->cmbPasswordType->setCurrentIndex(MPSiteTypeToIdx(m_site.site.getType()));
	passwordTypeChanged(ui->cmbPasswordType->currentIndex());
	checkInputValidity();
}

EditSiteWidget::~EditSiteWidget()
{
	delete ui;
}

void EditSiteWidget::applyData() {
	m_site.site.setName(ui->txtSiteName->text().toUtf8().constData());
	m_site.user_name = ui->txtUserName->text();
	m_site.comment = ui->txtComment->text();
	m_site.site.setType(MPSiteTypeFromIdx(ui->cmbPasswordType->currentIndex()));
	m_site.category_ids.clear();
	for (int i = 0; i < ui->layoutCategories->count(); ++i) {
		CategoryCheckbox* checkbox = dynamic_cast<CategoryCheckbox*>(
				ui->layoutCategories->itemAt(i)->widget());
		if (checkbox && checkbox->isChecked()) {
			m_site.category_ids.push_back(checkbox->category_id);
		}
	}

	m_site.site.setCounter(ui->spnSiteCounter->value());

	switch(m_type) {
	case Type_edit:
		m_site.time_edited = QDateTime::currentDateTime();
		break;
	case Type_new:
		m_site.time_edited = QDateTime::currentDateTime();
		m_site.time_created = m_site.time_edited;
		break;
	}
}

void EditSiteWidget::passwordTypeChanged(int new_type) {
	Site test_site;
	test_site.setName("test");
	test_site.setType(MPSiteTypeFromIdx(new_type));
	test_site.setCounter(rand() % 9999 + 1);
	std::string password = m_sample_password.sitePassword(test_site);
	ui->lblSamplePassword->setText(QString::fromUtf8(password.c_str()));
}
void EditSiteWidget::checkInputValidity() {
	bool is_valid = ui->txtSiteName->text().length() > 0;
	ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(is_valid);
}
