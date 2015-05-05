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


#include "keypress.h"

#if !defined(__linux__) && !defined(_WIN32)

#warning keypress stuff not (yet) implemented for your OS


Keypress::Keypress() {
}

Keypress::~Keypress() {
}

void Keypress::releaseModifiers() {
}

void Keypress::restoreModifiers() {
}

void Keypress::altTab() {
}

void Keypress::type(const char* str) {
}


#endif /* defined(__linux__) */
