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

	QPushButton* btn_copy = new UserPushButton(table_item->site(), "", parent);
	btn_copy->setIcon(QIcon(":/copy.png"));
	btn_copy->setFixedWidth(btn_copy->sizeHint().width());
	connect(btn_copy, SIGNAL(clicked()), &m_main_window,
			SLOT(copyPWToClipboardClicked()));

	QPushButton* btn_show = new UserPushButton(table_item->site(), "", parent);
	btn_show->setIcon(QIcon(table_item->site().password_visible ?
			":/hidden.png" : ":/shown.png"));
	btn_show->setFixedWidth(btn_show->sizeHint().width());
	connect(btn_show, SIGNAL(clicked()), &m_main_window,
			SLOT(showHidePWClicked()));

	QHBoxLayout* layout = new QHBoxLayout(parent);
	layout->setMargin(0);
	layout->addWidget(btn_show);
	layout->addWidget(btn_copy);
	QWidget* widget = new QWidget(parent);
	widget->setLayout(layout);
	widget->setFixedWidth(widget->sizeHint().width());

	return widget;
}

void PushButtonDelegate::updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option, const QModelIndex& index) const {
	editor->setGeometry(option.rect);
}
