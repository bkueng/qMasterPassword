/*
 * Copyright (C) 2015 Beat Küng <beat-kueng@gmx.net>
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


#include "identicon.h"
#include "crypto_functions.h"
#include "crypto.h"

constexpr const char* Identicon::left_arm[];
constexpr const char* Identicon::right_arm[];
constexpr const char* Identicon::body[];
constexpr const char* Identicon::accessory[];

const QColor Identicon::colors_dark[] = {
		QColor("#dc322f"), QColor("#859900"),
		QColor("#b58900"), QColor("#268bd2"),
		QColor("#d33682"), QColor("#2aa198"),
		QColor("#586e75")
};
const QColor Identicon::colors_light[] = {
		QColor("#dc322f"), QColor("#859900"),
		QColor("#b58900"), QColor("#268bd2"),
		QColor("#d33682"), QColor("#2aa198"),
		QColor("#93a1a1")
};

void Identicon::getIdenticon(const QString& master_password, QString& identicon,
		QColor& color) {

	identicon = "";
	QByteArray master_password_array = master_password.toUtf8();
	const char* pw = master_password_array.constData();
	QByteArray user_name_array = m_user_name.toUtf8();
	const int seed_len = 32;
	unsigned char seed_bytes[seed_len];
	if (!HMAC_SHA256((uint8_t*)pw, master_password_array.size(),
			(unsigned char*)user_name_array.constData(),
			user_name_array.size(), seed_bytes))
		throw CryptoException(CryptoException::Type_HMAC_SHA256_failed);

	identicon += left_arm[seed_bytes[0] % arraySize(left_arm)];
	identicon += body[seed_bytes[1] % arraySize(body)];
	identicon += right_arm[seed_bytes[2] % arraySize(right_arm)];
	identicon += accessory[seed_bytes[3] % arraySize(accessory)];
	color = colors_dark[seed_bytes[4] % arraySize(colors_dark)];
}
