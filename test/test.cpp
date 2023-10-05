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


#include "test.h"
#include "crypto.h"
#include <QXmlStreamReader>

UnitTests::UnitTests(const std::string& file_name)
	: m_file_name(file_name) {
}

void UnitTests::runTests_data() {
	QTest::addColumn<QString>("user_name");
	QTest::addColumn<QString>("master_password");
	QTest::addColumn<QString>("site_name");
	QTest::addColumn<QString>("site_type");
	QTest::addColumn<uint32_t>("site_counter");
	QTest::addColumn<QString>("site_variant");
	QTest::addColumn<QString>("site_context");
	QTest::addColumn<QString>("result");

	QString test_id;
	QString user_name;
	QString master_password;
	QString site_name;
	QString site_type;
	uint32_t site_counter;
	QString site_variant;
	QString site_context;
	QString result;

	QString file_name = QString::fromStdString(m_file_name);
	QFile file(file_name);
	QVERIFY2(file.open(QIODevice::ReadOnly), "Error opening file");
	QXmlStreamReader xml(&file);
	DataTree test_cases;

	QVERIFY(xml.readNextStartElement());
	QVERIFY(xml.name().toString() == "tests");

	while (xml.readNextStartElement()) {
		if (xml.name().toString() == "case") {
			TestNode node;
			QString id = xml.attributes().value("id").toString();
			node.parent = xml.attributes().value("parent").toString();
			while (xml.readNextStartElement()) {
				QString attr_name = xml.name().toString();
				xml.readNext();
				node.attributes[attr_name] = xml.text().toString();
				xml.skipCurrentElement();
			}
			test_cases[id] = node;
		} else {
			xml.skipCurrentElement();
		}
	}
	QVERIFY2(!xml.hasError(), "XML file contains errors");

	for (auto iter = test_cases.constBegin(); iter != test_cases.constEnd(); ++iter) {
		test_id = iter.key();
		QVERIFY(getAttribute(test_cases, iter, "fullName", user_name));
		QVERIFY(getAttribute(test_cases, iter, "masterPassword", master_password));
		QVERIFY(getAttribute(test_cases, iter, "siteName", site_name));
		QVERIFY(getAttribute(test_cases, iter, "siteType", site_type));
		QString site_counter_s;
		QVERIFY(getAttribute(test_cases, iter, "siteCounter", site_counter_s));
		site_counter = site_counter_s.toUInt();
		QVERIFY(getAttribute(test_cases, iter, "siteVariant", site_variant));
		getAttribute(test_cases, iter, "siteContext", site_context);
		QVERIFY(getAttribute(test_cases, iter, "result", result));

		QTest::newRow(test_id.toUtf8().constData()) << user_name
				<< master_password << site_name << site_type << site_counter
				<< site_variant << site_context << result;
	}
}
bool UnitTests::getAttribute(const DataTree& data, DataTree::ConstIterator iter,
		QString name, QString& ret) {
	const QMap<QString, QString>& attributes = iter.value().attributes;
	auto data_iter = attributes.find(name);
	if (data_iter == attributes.end()) {
		//search parent
		auto parent_iter = data.find(iter.value().parent);
		if (parent_iter == data.end()) {
			ret = "";
			return false;
		}
		return getAttribute(data, parent_iter, name, ret);
	}
	ret = data_iter.value();
	return true;
}

void UnitTests::runTests() {
	QFETCH(QString, user_name);
	QFETCH(QString, master_password);
	QFETCH(QString, site_name);
	QFETCH(QString, site_type);
	QFETCH(uint32_t, site_counter);
	QFETCH(QString, site_variant);
	QFETCH(QString, site_context);
	QFETCH(QString, result);

	MasterPassword mp;
	Site site;
	User user_data(user_name.toUtf8().constData());
	try {
		mp.login(user_data, master_password.toUtf8().constData());
		site.setName(site_name.toUtf8().constData());
		site.setCounter(site_counter);
		site.setVariant(site_variant.toUtf8().constData());
		site.setType(site_type.toUtf8().constData());
		site.setContext(site_context.toUtf8().constData());
	} catch (Exception& e) {
		e.log();
		QFAIL("Exception while generating site");
	}

	string password = mp.sitePassword(site);
	QCOMPARE(QString::fromUtf8(password.c_str()), result);
}

