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
#include "logging.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

using namespace std;

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
	QFile read_file(file_name);

	if (!read_file.open(QIODevice::ReadOnly)) {
		THROW(EFILE_ERROR);
	}

	QByteArray data_bytes = read_file.readAll();

	QJsonDocument load_doc(QJsonDocument::fromJson(data_bytes));
	QJsonArray json_sites_array = load_doc.array();
	for (const auto& json_site_iter : json_sites_array) {
		QJsonObject json_site = json_site_iter.toObject();
		QString category = json_site["category"].toString();
		QString password_type_str = json_site["passwordType"].toString();
		QString site_name = json_site["siteName"].toString();
		QString user_name = json_site["userName"].toString();
		bool ok;
		uint32_t site_counter = json_site["siteCounter"].toString().toInt(&ok);
		if (!ok) site_counter = 1;
		QString comment = json_site["comment"].toString();
		if (site_name != "") {
			string site_name_str = site_name.toUtf8().constData();
			//check if site exists
			shared_ptr<UiSite> new_site;
			for (auto& site : user.getSites()) {
				if (site->site.getName() == site_name_str) {
					new_site = site;
					LOG(DEBUG, "Import: found site %s", site_name_str.c_str());
					break;
				}
			}
			if (!new_site.get()) {
				LOG(DEBUG, "Import: new site %s", site_name_str.c_str());
				new_site = make_shared<UiSite>();
				user.getSites().push_back(new_site);
			}

			//apply data
			for (auto iter = m_categories.constBegin();
					iter != m_categories.constEnd(); ++iter) {
				if (iter.value() == category) {
					new_site->category_ids.clear();
					new_site->category_ids.push_back(iter.key());
					break;
				}
				//TODO (?): add new category if it isn't found?
			}

			try {
				new_site->site.setType(password_type_str.toUtf8().constData());
			} catch (Exception& e) {
				LOG(WARN, "Import: unknown password type %s",
						password_type_str.toUtf8().constData());
			}
			new_site->site.setName(site_name_str);
			new_site->user_name = user_name;
			new_site->site.setCounter(site_counter);
			new_site->comment = comment;
		}
	}

}
