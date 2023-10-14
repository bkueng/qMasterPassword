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

/** @file helper classes for that extend ui classes to contain additional data */

#ifndef _HEADER_UITEMPLATE_HELPERS_H
#define _HEADER_UITEMPLATE_HELPERS_H

#include "user.h"
#include <QStandardItem>
#include <QPushButton>
#include <QCheckBox>


template<typename T>
class UserUITemplate : public T {
public:
	UserUITemplate(UiSite& site, const QString& item_text)
		: T(item_text), m_site(site) {}
	UserUITemplate(UiSite& site, const QString& item_text, QWidget* parent)
		: T(item_text, parent), m_site(site) {}

	UiSite& site() { return m_site; }
private:
	UiSite& m_site;
};
typedef UserUITemplate<QStandardItem> TableItem;
typedef UserUITemplate<QPushButton> UserPushButton;



template<typename T>
class CategoryUiTemplate : public T
{
public:
	CategoryUiTemplate(CategoryId id, const QString& text)
		: T(text), category_id(id) {}

	const CategoryId category_id;
private:
};

typedef CategoryUiTemplate<QCheckBox> CategoryCheckbox;
typedef CategoryUiTemplate<QPushButton> CategoryButton;



#endif // _HEADER_UITEMPLATE_HELPERS_H
