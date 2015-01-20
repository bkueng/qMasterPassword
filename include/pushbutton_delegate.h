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

#ifndef _HEADER_PUSHBUTTON_DELEGATE_H
#define _HEADER_PUSHBUTTON_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPushButton>
class MainWindow;

/**
 ** class PushButtonDelegate
 * Delegate class to draw a button in a table view
 */
class PushButtonDelegate: public QStyledItemDelegate {
	Q_OBJECT
public:
	PushButtonDelegate(MainWindow& main_window, QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;

	/*
	//no editing needed
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
			const QModelIndex &index) const;
	*/

	void updateEditorGeometry(QWidget *editor,
			const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
	MainWindow& m_main_window;
};


#endif // _HEADER_PUSHBUTTON_DELEGATE_H
