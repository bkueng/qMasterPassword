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

#ifndef _HEADER_IDENTICON_H_
#define _HEADER_IDENTICON_H_

#include <QColor>
#include <QString>

/**
 ** class Identicon.
 *  identicon class for the user to check the correctness of the given
 *  masterpassword. this avoids the need to store the hash of the password.
 */
class Identicon {
   public:
    const QString& userName() const { return m_user_name; }
    void setUserName(const QString& userName) { m_user_name = userName; }

    /**
     * calculate the identicon
     * @param master_password input master password
     * @param identicon output identicon (unicode icons)
     * @param color output color
     */
    void getIdenticon(const QString& master_password, QString& identicon, QColor& color);

   private:
    QString m_user_name;

    /*
    static Color[] colors  = new Color[]{
            Color.RED, Color.GREEN, Color.YELLOW, Color.BLUE, Color.MAGENTA, Color.CYAN, Color.MONO
    };
            */

    template <class T>
    static int arraySize(const T& array)
    {
        return sizeof(array) / sizeof(array[0]);
    }

    static constexpr const char* left_arm[] = {u8"\u2554", u8"\u255a", u8"\u2570", u8"\u2550"};
    static constexpr const char* right_arm[] = {u8"\u2557", u8"\u255d", u8"\u256f", u8"\u2550"};
    static constexpr const char* body[] = {u8"\u2588", u8"\u2591", u8"\u2592",
                                           u8"\u2593", u8"\u263a", u8"\u263b"};
    static constexpr const char* accessory[] = {
        u8"\u25c8", u8"\u25ce", u8"\u25d0", u8"\u25d1", u8"\u25d2", u8"\u25d3", u8"\u2600",
        u8"\u2601", u8"\u2602", u8"\u2603", u8"\u2604", u8"\u2605", u8"\u2606", u8"\u260e",
        u8"\u260f", u8"\u2388", u8"\u2302", u8"\u2618", u8"\u2622", u8"\u2623", u8"\u2615",
        u8"\u231a", u8"\u231b", u8"\u23f0", u8"\u26a1", u8"\u26c4", u8"\u26c5", u8"\u2614",
        u8"\u2654", u8"\u2655", u8"\u2656", u8"\u2657", u8"\u2658", u8"\u2659", u8"\u265a",
        u8"\u265b", u8"\u265c", u8"\u265d", u8"\u265e", u8"\u265f", u8"\u2668", u8"\u2669",
        u8"\u266a", u8"\u266b", u8"\u2690", u8"\u2691", u8"\u2694", u8"\u2696", u8"\u2699",
        u8"\u26a0", u8"\u2318", u8"\u23ce", u8"\u2704", u8"\u2706", u8"\u2708", u8"\u2709",
        u8"\u270c"};

    static const QColor colors_dark[];
    static const QColor colors_light[];
};

#endif /* _HEADER_IDENTICON_H_ */
