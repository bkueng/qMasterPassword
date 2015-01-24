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


#ifndef _HEADER_IMPORT_EXPORT_H_
#define _HEADER_IMPORT_EXPORT_H_

#include <QString>
#include <QMap>
#include "user.h"

/**
 ** class DataImportExport
 *  data import & export. import does not delete existing sites, only overwrites
 *  existing sites with the same name.
 */
class DataImportExport {
public:
	DataImportExport(const QMap<CategoryId, QString>& categories);

	void exportJson(const UiUser& user, const QString& file_name);
	void importJson(UiUser& user, const QString& file_name);

private:
	const QMap<CategoryId, QString>& m_categories;
};

#endif /* _HEADER_IMPORT_EXPORT_H_ */
