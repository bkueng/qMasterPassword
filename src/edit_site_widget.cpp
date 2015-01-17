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

EditSiteWidget::EditSiteWidget(UiSite& site, Type type, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::EditSiteWidget), m_site(site), m_type(type)
{
	ui->setupUi(this);
	setFixedSize(size());
	m_sample_password.login("test", "topsecret");

	for (int i = 0; i < MPSiteTypeCount(); ++i) {
		std::string str_type = MPSiteTypeToString(MPSiteTypeFromIdx(i));
		ui->cmbPasswordType->addItem(QString::fromStdString(str_type));
	}

	switch(type) {
	case Type_edit:
		setWindowTitle(tr("Edit Site"));
		ui->txtSiteName->setText(QString::fromStdString(m_site.site.getName()));
		ui->txtUserName->setText(m_site.user_name);
		ui->txtComment->setText(m_site.comment);
		//TODO: categories

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
	m_site.site.setName(ui->txtSiteName->text().toStdString());
	m_site.user_name = ui->txtUserName->text();
	m_site.comment = ui->txtComment->text();
	m_site.site.setType(MPSiteTypeFromIdx(ui->cmbPasswordType->currentIndex()));
	m_site.category_ids.clear();
	//TODO: categories

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
	ui->lblSamplePassword->setText(QString::fromStdString(password));
}
void EditSiteWidget::checkInputValidity() {
	bool is_valid = ui->txtSiteName->text().length() > 0;
	ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(is_valid);
}
