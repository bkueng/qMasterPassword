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


#include "import_export.h"
#include "exception.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

DataImportExport::DataImportExport(
		const QMap<CategoryId, QString>& categories)
	: m_categories(categories) {
}

void DataImportExport::exportJson(const UiUser& user,
		const QString& file_name) {
	QFile save_file(file_name);

	if (!save_file.open(QIODevice::WriteOnly)) {
		THROW(EFILE_ERROR);
	}

	QJsonArray sites_array;
	for (const auto& site : user.getSites()) {
		QJsonObject site_object;
		UiSite s;
		if (!site->category_ids.isEmpty()) {
			//Note: for compatibility reasons we only write the first category
			const auto& category = m_categories.find(site->category_ids[0]);
			if (category != m_categories.end()) {
				site_object["category"] = category.value();
			}
		}
		site_object["passwordType"] = "Generated"+QString::fromUtf8(
				MPSiteTypeToString(site->site.getType()).c_str());
		site_object["siteName"] = QString::fromUtf8(site->site.getName().c_str());
		site_object["userName"] = site->user_name;
		site_object["siteCounter"] = QString::number(site->site.getCounter());
		site_object["comment"] = site->comment;
		sites_array.append(site_object);
	}
	QJsonDocument save_doc(sites_array);
	if (save_file.write(save_doc.toJson()) == -1)
		THROW(EFILE_ERROR);
}

void DataImportExport::importJson(UiUser& user, const QString& file_name) {
	//TODO
}
