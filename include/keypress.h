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

/** @file keyboard input simulation */

#ifndef _HEADER_KEYBOARD_H_
#define _HEADER_KEYBOARD_H_

struct xdo;
typedef xdo xdo_t;

/**
 ** class Keypress
 */
class Keypress {
   public:
    Keypress();
    ~Keypress();

    /** saves the current modifiers (alt, ...) & releases them */
    void releaseModifiers();
    /** restores previously saved modifiers */
    void restoreModifiers();

    void altTab();
    void type(const char* str);

   private:
#if defined(__linux__)
    void* m_display = nullptr;
    xdo_t* m_xdo = nullptr;
    void* m_active_mods = nullptr;
#elif defined(_WIN32)
    void* m_simulator = nullptr;
    bool m_keys_pressed[5];
#endif /* defined(__linux__) */
};

#endif /* _HEADER_KEYBOARD_H_ */
