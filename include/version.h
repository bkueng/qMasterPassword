/*
 * Copyright (C) 2010-2020 Beat Küng <beat-kueng@gmx.net>
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

/*! @file
 * @brief Software versioning of source code tree.
 *
 * This file is automatically maintained by script.
 */
#ifndef VERSION_H_
#define VERSION_H_

/* stl */
#include <sstream>
#include <string>
using namespace std;

#define VERSION_MAJOR 2 /*<! @brief Major version number. */
#define VERSION_MINOR 0 /*<! @brief Minor version number. */
#define VERSION_PATCH 3 /*<! @brief patch number. */

struct VERSION {
    VERSION() : bSet(false) {}
    VERSION(int vmajor, int vminor, int vpatch) : bSet(true)
    {
        this->major = vmajor;
        this->minor = vminor;
        this->patch = vpatch;
    }

    bool bSet;

    int major;
    int minor;
    int patch;

    string toStr()
    {
        // version has the format: v<major>.<minor>[-p<patch>]
        if (!bSet) return "";
        ostringstream stream;
        stream << "v" << major << "." << minor;
        if (patch != 0) stream << "." << patch;
        return stream.str();
    }
};

VERSION getAppVersion();

#endif /* VERSION_H_ */
