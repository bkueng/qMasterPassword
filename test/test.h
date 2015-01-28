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

/** @file unit tests based on Qt Test */

#ifndef _HEADER_TEST_H_
#define _HEADER_TEST_H_

#include <QtTest/QtTest>
#include <QObject>
#include <QMap>
#include <string>

struct TestNode {
	QString parent;
	QMap<QString, QString> attributes;
};

class UnitTests : public QObject {
Q_OBJECT
public:
	UnitTests(const std::string& file_name);

	typedef QMap<QString, TestNode> DataTree;
private slots:
	void runTests_data();
	void runTests();
private:
	bool getAttribute(const DataTree& data, DataTree::ConstIterator iter,
			QString name, QString& ret);
	std::string m_file_name;
};

#endif /* _HEADER_TEST_H_ */
