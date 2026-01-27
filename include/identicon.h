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

    static const char* const left_arm[];
    static const char* const right_arm[];
    static const char* const body[];
    static const char* const accessory[];
    static const QColor colors_dark[];
    static const QColor colors_light[];
};

#endif /* _HEADER_IDENTICON_H_ */
