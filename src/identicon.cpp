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

#include "crypto.h"
#include "crypto_functions.h"

const char* const Identicon::left_arm[] = {
    reinterpret_cast<const char*>(u8"\u2554"),
    reinterpret_cast<const char*>(u8"\u255a"),
    reinterpret_cast<const char*>(u8"\u2570"),
    reinterpret_cast<const char*>(u8"\u2550"),
};
const char* const Identicon::right_arm[] = {
    reinterpret_cast<const char*>(u8"\u2557"),
    reinterpret_cast<const char*>(u8"\u255d"),
    reinterpret_cast<const char*>(u8"\u256f"),
    reinterpret_cast<const char*>(u8"\u2550"),
};
const char* const Identicon::body[] = {
    reinterpret_cast<const char*>(u8"\u2588"), reinterpret_cast<const char*>(u8"\u2591"),
    reinterpret_cast<const char*>(u8"\u2592"), reinterpret_cast<const char*>(u8"\u2593"),
    reinterpret_cast<const char*>(u8"\u263a"), reinterpret_cast<const char*>(u8"\u263b"),
};
const char* const Identicon::accessory[] = {
    reinterpret_cast<const char*>(u8"\u25c8"), reinterpret_cast<const char*>(u8"\u25ce"),
    reinterpret_cast<const char*>(u8"\u25d0"), reinterpret_cast<const char*>(u8"\u25d1"),
    reinterpret_cast<const char*>(u8"\u25d2"), reinterpret_cast<const char*>(u8"\u25d3"),
    reinterpret_cast<const char*>(u8"\u2600"), reinterpret_cast<const char*>(u8"\u2601"),
    reinterpret_cast<const char*>(u8"\u2602"), reinterpret_cast<const char*>(u8"\u2603"),
    reinterpret_cast<const char*>(u8"\u2604"), reinterpret_cast<const char*>(u8"\u2605"),
    reinterpret_cast<const char*>(u8"\u2606"), reinterpret_cast<const char*>(u8"\u260e"),
    reinterpret_cast<const char*>(u8"\u260f"), reinterpret_cast<const char*>(u8"\u2388"),
    reinterpret_cast<const char*>(u8"\u2302"), reinterpret_cast<const char*>(u8"\u2618"),
    reinterpret_cast<const char*>(u8"\u2622"), reinterpret_cast<const char*>(u8"\u2623"),
    reinterpret_cast<const char*>(u8"\u2615"), reinterpret_cast<const char*>(u8"\u231a"),
    reinterpret_cast<const char*>(u8"\u231b"), reinterpret_cast<const char*>(u8"\u23f0"),
    reinterpret_cast<const char*>(u8"\u26a1"), reinterpret_cast<const char*>(u8"\u26c4"),
    reinterpret_cast<const char*>(u8"\u26c5"), reinterpret_cast<const char*>(u8"\u2614"),
    reinterpret_cast<const char*>(u8"\u2654"), reinterpret_cast<const char*>(u8"\u2655"),
    reinterpret_cast<const char*>(u8"\u2656"), reinterpret_cast<const char*>(u8"\u2657"),
    reinterpret_cast<const char*>(u8"\u2658"), reinterpret_cast<const char*>(u8"\u2659"),
    reinterpret_cast<const char*>(u8"\u265a"), reinterpret_cast<const char*>(u8"\u265b"),
    reinterpret_cast<const char*>(u8"\u265c"), reinterpret_cast<const char*>(u8"\u265d"),
    reinterpret_cast<const char*>(u8"\u265e"), reinterpret_cast<const char*>(u8"\u265f"),
    reinterpret_cast<const char*>(u8"\u2668"), reinterpret_cast<const char*>(u8"\u2669"),
    reinterpret_cast<const char*>(u8"\u266a"), reinterpret_cast<const char*>(u8"\u266b"),
    reinterpret_cast<const char*>(u8"\u2690"), reinterpret_cast<const char*>(u8"\u2691"),
    reinterpret_cast<const char*>(u8"\u2694"), reinterpret_cast<const char*>(u8"\u2696"),
    reinterpret_cast<const char*>(u8"\u2699"), reinterpret_cast<const char*>(u8"\u26a0"),
    reinterpret_cast<const char*>(u8"\u2318"), reinterpret_cast<const char*>(u8"\u23ce"),
    reinterpret_cast<const char*>(u8"\u2704"), reinterpret_cast<const char*>(u8"\u2706"),
    reinterpret_cast<const char*>(u8"\u2708"), reinterpret_cast<const char*>(u8"\u2709"),
    reinterpret_cast<const char*>(u8"\u270c"),
};

const QColor Identicon::colors_dark[] = {QColor("#dc322f"), QColor("#859900"), QColor("#b58900"),
                                         QColor("#268bd2"), QColor("#d33682"), QColor("#2aa198"),
                                         QColor("#586e75")};
const QColor Identicon::colors_light[] = {QColor("#dc322f"), QColor("#859900"), QColor("#b58900"),
                                          QColor("#268bd2"), QColor("#d33682"), QColor("#2aa198"),
                                          QColor("#93a1a1")};

void Identicon::getIdenticon(const QString& master_password, QString& identicon, QColor& color)
{
    identicon = "";
    QByteArray master_password_array = master_password.toUtf8();
    const char* pw = master_password_array.constData();
    QByteArray user_name_array = m_user_name.toUtf8();
    const int seed_len = 32;
    unsigned char seed_bytes[seed_len];
    if (!HMAC_SHA256((uint8_t*)pw, master_password_array.size(),
                     (unsigned char*)user_name_array.constData(), user_name_array.size(),
                     seed_bytes))
        throw CryptoException(CryptoException::Type_HMAC_SHA256_failed);

    identicon += left_arm[seed_bytes[0] % arraySize(left_arm)];
    identicon += body[seed_bytes[1] % arraySize(body)];
    identicon += right_arm[seed_bytes[2] % arraySize(right_arm)];
    identicon += accessory[seed_bytes[3] % arraySize(accessory)];
    color = colors_dark[seed_bytes[4] % arraySize(colors_dark)];
}
