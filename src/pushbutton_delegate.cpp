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

#include "pushbutton_delegate.h"
#include "ui_helpers.h"
#include "main_window.h"

#include <QIcon>
#include <QStandardItemModel>
#include <QHBoxLayout>

PushButtonDelegate::PushButtonDelegate(MainWindow& main_window, QObject* parent)
	: QStyledItemDelegate(parent), m_main_window(main_window) {
}

QWidget* PushButtonDelegate::createEditor(QWidget* parent,
		const QStyleOptionViewItem& option, const QModelIndex& index) const {
	const MySortFilterProxyModel* item_proxy_model =
			dynamic_cast<const MySortFilterProxyModel*>(index.model());
	if (!item_proxy_model) return nullptr;

	const QStandardItemModel* item_model =
			dynamic_cast<QStandardItemModel*>(item_proxy_model->sourceModel());
	DEBUG_ASSERT1(item_model);
	TableItem* table_item = dynamic_cast<TableItem*>(item_model->itemFromIndex(
			item_proxy_model->mapToSource(index)));
	DEBUG_ASSERT1(table_item);

	auto createButton = [&parent] (UiSite& site, QIcon icon) -> QPushButton* {
		UserPushButton* button = new UserPushButton(site, "", parent);
		button->setIcon(icon);
		button->setStyleSheet("padding: 1px");
		button->setFixedWidth(button->sizeHint().width());
		button->setFocusPolicy(Qt::NoFocus);
		button->setFlat(true);
		return button;
	};

	QPushButton* btn_copy = createButton(table_item->site(), QIcon(":/copy.png"));
	btn_copy->setToolTip(tr("Copy password to clipboard"));
	connect(btn_copy, SIGNAL(clicked()), &m_main_window,
			SLOT(copyPWToClipboardClicked()));

	QPushButton* btn_show = createButton(table_item->site(),
		QIcon(table_item->site().password_visible ? ":/hidden.png" : ":/shown.png"));
	btn_show->setToolTip(tr("Show/hide password"));
	connect(btn_show, SIGNAL(clicked()), &m_main_window,
			SLOT(showHidePWClicked()));

	QPushButton* btn_url = createButton(table_item->site(), QIcon(":/internet.png"));
	connect(btn_url, SIGNAL(clicked()), &m_main_window, SLOT(openUrlClicked()));
	btn_url->setToolTip(tr("Open URL"));
	btn_url->setEnabled(table_item->site().url != "");

	QHBoxLayout* layout = new QHBoxLayout(parent);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(btn_show);
	layout->addWidget(btn_copy);
	layout->addWidget(btn_url);
	QWidget* widget = new QWidget(parent);
	widget->setLayout(layout);
	widget->setFixedWidth(widget->sizeHint().width());

	return widget;
}

void PushButtonDelegate::updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option, const QModelIndex& index) const {
	editor->setGeometry(option.rect);
}
