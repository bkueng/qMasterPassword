#include "InputBuilder.h"

#include <Windef.h>

#include "stdafx.h"

/* Copyright(c) 1998-2012, Arnaud Colin
 * All rights reserved.
 *
 * Licence GNU GPL version 3
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   -> Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   -> Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  Create from Original project Windows Input Simulator (C# SendInput Wrapper - Simulate Keyboard
 * and Mouse) from michaelnoonan, you can find http://inputsimulator.codeplex.com/
 *
 */

CInputBuilder::CInputBuilder(void)
{
    _inputList.clear();  // = new List<INPUT>();
}

CInputBuilder::~CInputBuilder(void)
{
}

INPUT* CInputBuilder::ToArray()
{
    return &(_inputList.front());
}

INPUT CInputBuilder::operator[](int position)
{
    return _inputList[position];
}

CInputBuilder& CInputBuilder::AddKeyDown(VirtualKeyCode keyCode)
{
    INPUT down;
    down.type = INPUT_KEYBOARD;
    down.ki.wVk = LOBYTE(keyCode);
    down.ki.wScan = 0;
    down.ki.dwFlags = 0;
    down.ki.time = 0;
    down.ki.dwExtraInfo = NULL;

    _inputList.insert(_inputList.end(), down);
    return *this;
}

CInputBuilder& CInputBuilder::AddKeyUp(VirtualKeyCode keyCode)
{
    INPUT up;
    up.type = INPUT_KEYBOARD;
    up.ki.wVk = LOBYTE(keyCode);
    up.ki.wScan = 0;
    up.ki.dwFlags = KEYEVENTF_KEYUP;
    up.ki.time = 0;
    up.ki.dwExtraInfo = NULL;

    _inputList.insert(_inputList.end(), up);
    return *this;
}

CInputBuilder& CInputBuilder::AddKeyPress(VirtualKeyCode keyCode)
{
    AddKeyDown(keyCode);
    AddKeyUp(keyCode);
    return *this;
}

CInputBuilder& CInputBuilder::AddKeyDown(std::vector<VirtualKeyCode>* pmodifierKeyCodes)
{
    if (pmodifierKeyCodes != NULL) {
        for (std::vector<VirtualKeyCode>::iterator character = pmodifierKeyCodes->begin();
             character != pmodifierKeyCodes->end(); ++character) {
            AddKeyDown(*character);
        }
    }
    return *this;
}

CInputBuilder& CInputBuilder::AddKeyUp(std::vector<VirtualKeyCode>* pmodifierKeyCodes)
{
    if (pmodifierKeyCodes != NULL) {
        for (std::vector<VirtualKeyCode>::iterator character = pmodifierKeyCodes->end();
             character != pmodifierKeyCodes->begin(); --character) {
            AddKeyUp(*character);
        }
    }
    return *this;
}

CInputBuilder& CInputBuilder::AddKeyPress(std::vector<VirtualKeyCode>* pkeyCodes)
{
    if (pkeyCodes != NULL) {
        for (std::vector<VirtualKeyCode>::iterator character = pkeyCodes->begin();
             character != pkeyCodes->end(); ++character) {
            AddKeyPress(*character);
        }
    }
    return *this;
}

CInputBuilder& CInputBuilder::AddCharacter(wchar_t character)
{
    UINT16 scanCode = character;
    INPUT down;
    down.type = INPUT_KEYBOARD;
    down.ki.wVk = 0;
    down.ki.wScan = scanCode;
    down.ki.dwFlags = KEYEVENTF_UNICODE;
    down.ki.time = 0;
    down.ki.dwExtraInfo = NULL;

    INPUT up;
    up.type = INPUT_KEYBOARD;
    up.ki.wVk = 0;
    up.ki.wScan = scanCode;
    up.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_UNICODE;
    up.ki.time = 0;
    up.ki.dwExtraInfo = NULL;

    // Handle extended keys:
    // If the scan code is preceded by a prefix byte that has the value 0xE0 (224),
    // we need to include the KEYEVENTF_EXTENDEDKEY flag in the Flags property.
    if ((scanCode & 0xFF00) == 0xE000) {
        down.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        up.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }

    _inputList.insert(_inputList.end(), down);
    _inputList.insert(_inputList.end(), up);
    return *this;
}

CInputBuilder& CInputBuilder::AddCharacters(std::vector<wchar_t> characters)
{
    for (std::vector<wchar_t>::iterator character = characters.begin();
         character != characters.end(); ++character) {
        AddCharacter(*character);
    }

    return *this;
}

CInputBuilder& CInputBuilder::AddCharacters(LPCWSTR wstr)
{
    int count = wcslen(wstr);
    wchar_t* pwchr = const_cast<wchar_t*>(&wstr[0]);
    for (int j = 0; j < count; ++j) {
        AddCharacter(*pwchr);
        pwchr++;
    }

    return *this;
}

CInputBuilder& CInputBuilder::AddRelativeMouseMovement(int x, int y)
{
    INPUT movement;
    movement.type = INPUT_MOUSE;
    movement.mi.dx = x;
    movement.mi.dy = y;
    movement.mi.dwFlags = MOUSEEVENTF_MOVE;
    // movement.mi.time = 0;
    // movement.mi.dwExtraInfo = NULL;

    _inputList.insert(_inputList.end(), movement);

    return *this;
}

CInputBuilder& CInputBuilder::AddAbsoluteMouseMovement(int absoluteX, int absoluteY)
{
    INPUT movement;
    movement.type = INPUT_MOUSE;
    movement.mi.dx = absoluteX;
    movement.mi.dy = absoluteY;
    movement.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    // movement.mi.time = 0;
    // movement.mi.dwExtraInfo = NULL;

    _inputList.insert(_inputList.end(), movement);

    return *this;
}

CInputBuilder& CInputBuilder::AddAbsoluteMouseMovementOnVirtualDesktop(int absoluteX, int absoluteY)
{
    INPUT movement;
    movement.type = INPUT_MOUSE;
    movement.mi.dx = absoluteX;
    movement.mi.dy = absoluteY;
    movement.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    // movement.mi.time = 0;
    // movement.mi.dwExtraInfo = NULL;

    _inputList.insert(_inputList.end(), movement);

    return *this;
}

CInputBuilder& CInputBuilder::AddMouseButtonDown(MouseButton button)
{
    INPUT buttonDown;
    buttonDown.type = INPUT_MOUSE;
    buttonDown.mi.dwFlags = MouseButtonExtensions::ToMouseButtonDownFlag(button);
    _inputList.insert(_inputList.end() + 1, buttonDown);

    return *this;
}

CInputBuilder& CInputBuilder::AddMouseXButtonDown(int xButtonId)
{
    INPUT buttonDown;
    buttonDown.type = INPUT_MOUSE;
    buttonDown.mi.dwFlags = MOUSEEVENTF_XDOWN;
    buttonDown.mi.mouseData = xButtonId;
    _inputList.insert(_inputList.end(), buttonDown);

    return *this;
}

CInputBuilder& CInputBuilder::AddMouseButtonUp(MouseButton button)
{
    INPUT buttonDown;
    buttonDown.type = INPUT_MOUSE;
    buttonDown.mi.dwFlags = MouseButtonExtensions::ToMouseButtonUpFlag(button);
    _inputList.insert(_inputList.end(), buttonDown);

    return *this;
}

CInputBuilder& CInputBuilder::AddMouseXButtonUp(int xButtonId)
{
    INPUT buttonUp;
    buttonUp.type = INPUT_MOUSE;
    buttonUp.mi.dwFlags = MOUSEEVENTF_XUP;
    buttonUp.mi.mouseData = xButtonId;
    _inputList.insert(_inputList.end(), buttonUp);

    return *this;
}

CInputBuilder& CInputBuilder::AddMouseButtonClick(MouseButton button)
{
    return AddMouseButtonDown(button).AddMouseButtonUp(button);
}

CInputBuilder& CInputBuilder::AddMouseXButtonClick(int xButtonId)
{
    return AddMouseXButtonDown(xButtonId).AddMouseXButtonUp(xButtonId);
}

CInputBuilder& CInputBuilder::AddMouseButtonDoubleClick(MouseButton button)
{
    return AddMouseButtonClick(button).AddMouseButtonClick(button);
}

CInputBuilder& CInputBuilder::AddMouseXButtonDoubleClick(int xButtonId)
{
    return AddMouseXButtonClick(xButtonId).AddMouseXButtonClick(xButtonId);
}

CInputBuilder& CInputBuilder::AddMouseVerticalWheelScroll(int scrollAmount)
{
    INPUT scroll;
    scroll.type = INPUT_MOUSE;
    scroll.mi.dwFlags = MOUSEEVENTF_WHEEL;
    scroll.mi.mouseData = scrollAmount;
    _inputList.insert(_inputList.end(), scroll);

    return *this;
}

CInputBuilder& CInputBuilder::AddHardware(DWORD msg, DWORD paramh, DWORD paraml)
{
    INPUT tesf;
    tesf.type = INPUT_HARDWARE;
    tesf.hi.uMsg = 0;
    tesf.hi.wParamH = 0;
    tesf.hi.wParamL = 0;
    _inputList.insert(_inputList.end(), tesf);

    return *this;
}

// WM_TOUCH
