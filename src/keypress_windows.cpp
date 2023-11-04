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

#include "KeyboardSimulator.h"
#include "WindowsInputDeviceStateAdaptor.h"

#include <type_traits>
#include <string>


#if defined(_WIN32)

Keypress::Keypress() {
	m_simulator = new CKeyboardSimulator();
}

Keypress::~Keypress() {
	if (m_simulator) delete ((CKeyboardSimulator*)m_simulator);
}

static VirtualKeyCode modifier_keys[] = {
	VirtualKeyCode::SHIFT,
	VirtualKeyCode::CONTROL,
	VirtualKeyCode::MENU,
	VirtualKeyCode::LWIN,
	VirtualKeyCode::RWIN
};


void Keypress::releaseModifiers() {
	constexpr int num_keys = sizeof(modifier_keys) / sizeof(VirtualKeyCode);
	static_assert(num_keys == sizeof(m_keys_pressed) / sizeof(m_keys_pressed[0]),
			"array size missmatch");

	//FIXME: handle numlock?

	CKeyboardSimulator* sim = (CKeyboardSimulator*)m_simulator;
	CWindowsInputDeviceStateAdaptor adaptor;
	for (int i = 0; i < num_keys; ++i) {
		m_keys_pressed[i] = adaptor.IsHardwareKeyDown(modifier_keys[i]);
		if (m_keys_pressed[i]) {
			sim->KeyUp(modifier_keys[i]);
		}
	}
}

void Keypress::restoreModifiers() {
	constexpr int num_keys = sizeof(modifier_keys) / sizeof(VirtualKeyCode);
	CKeyboardSimulator* sim = (CKeyboardSimulator*)m_simulator;
	CWindowsInputDeviceStateAdaptor adaptor;
	for (int i = 0; i < num_keys; ++i) {
		if (m_keys_pressed[i])
			sim->KeyDown(modifier_keys[i]);
	}
}

void Keypress::altTab() {
	CKeyboardSimulator* sim = (CKeyboardSimulator*)m_simulator;
	sim->KeyDown(VirtualKeyCode::LMENU);
	sim->KeyDown(VirtualKeyCode::TAB);
	Sleep(100);
	sim->KeyUp(VirtualKeyCode::TAB);
	sim->KeyUp(VirtualKeyCode::LMENU);
}

void Keypress::type(const char* str) {

	CKeyboardSimulator* sim = (CKeyboardSimulator*)m_simulator;

	//we need to convert str from UTF8 to UTF16
	/*
	//with c++11:
#include <locale>
#include <codecvt>
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(str);
	sim->TextEntry(wide.c_str());
	*/

	//Win32 API
	std::string sstr = str;
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &sstr[0], (int)sstr.size(), NULL, 0);
	std::wstring wstrTo( size_needed, 0 );
	MultiByteToWideChar(CP_UTF8, 0, &sstr[0], (int)sstr.size(), &wstrTo[0], size_needed);
	sim->TextEntry(wstrTo.c_str());
}


#endif /* defined(__linux__) */

