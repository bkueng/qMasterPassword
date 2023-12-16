#include "KeyboardSimulator.h"

#include "InputBuilder.h"
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
 *  Create from  Windows Input Simulator (C# SendInput Wrapper - Simulate Keyboard and Mouse)
 *  a project of michaelnoonan, you can find http://inputsimulator.codeplex.com/
 *
 */

CKeyboardSimulator::CKeyboardSimulator(IInputMessageDispatcher* pmessageDispatcher)
{
    if (pmessageDispatcher == NULL) throw "";

    _pmessageDispatcher = pmessageDispatcher;
    bNeedDelete = false;
}

CKeyboardSimulator::CKeyboardSimulator(void)
{
    _pmessageDispatcher = new WindowsInputMessageDispatcher();
    bNeedDelete = true;
}

CKeyboardSimulator::~CKeyboardSimulator(void)
{
    if (bNeedDelete && _pmessageDispatcher != NULL) {
        delete _pmessageDispatcher;
        _pmessageDispatcher = NULL;
    }
}

int CKeyboardSimulator::SendSimulatedInput(INPUT& input)
{
    return (int)_pmessageDispatcher->DispatchInput(input);
}

int CKeyboardSimulator::SendSimulatedInput(CInputBuilder& inputbuilder)
{
    if ((inputbuilder.Size() <= 0)) return -1;
    return (int)_pmessageDispatcher->DispatchInput(inputbuilder);
}

void CKeyboardSimulator::KeyDown(VirtualKeyCode keyCode)
{
    CInputBuilder inputList;
    SendSimulatedInput(inputList.AddKeyDown(keyCode));
}

void CKeyboardSimulator::KeyUp(VirtualKeyCode keyCode)
{
    CInputBuilder inputList;
    SendSimulatedInput(inputList.AddKeyUp(keyCode));
}

void CKeyboardSimulator::KeyPress(VirtualKeyCode keyCode)
{
    CInputBuilder inputList;
    inputList.AddKeyDown(keyCode).AddKeyUp(keyCode);

    SendSimulatedInput(inputList);
}

void CKeyboardSimulator::KeyPress(VirtualKeyCode keyCode, const unsigned int count /*= 1*/)
{
    CInputBuilder inputList;
    for (unsigned int i = 0; i < count; i++) {
        inputList.AddKeyDown(keyCode).AddKeyUp(keyCode);
    }

    SendSimulatedInput(inputList);
}

void CKeyboardSimulator::ModifiedKeyStroke(VirtualKeyCode modifierKeyCode, VirtualKeyCode keyCode)
{
    CInputBuilder inputList;
    inputList.AddKeyDown(modifierKeyCode).AddKeyPress(keyCode).AddKeyUp(modifierKeyCode);

    SendSimulatedInput(inputList);
}

void CKeyboardSimulator::ModifiedKeyStroke(std::vector<VirtualKeyCode>* modifierKeyCodes,
                                           VirtualKeyCode keyCode)
{
    CInputBuilder builder;
    if (modifierKeyCodes != NULL) builder.AddKeyDown(modifierKeyCodes);
    builder.AddKeyPress(keyCode);
    if (modifierKeyCodes != NULL) builder.AddKeyUp(modifierKeyCodes);

    SendSimulatedInput(builder);
}

void CKeyboardSimulator::ModifiedKeyStroke(VirtualKeyCode modifierKey,
                                           std::vector<VirtualKeyCode>* keyCodes)
{
    CInputBuilder builder;
    builder.AddKeyDown(modifierKey);
    if (keyCodes != NULL) builder.AddKeyPress(keyCodes);
    builder.AddKeyUp(modifierKey);

    SendSimulatedInput(builder);
}

void CKeyboardSimulator::ModifiedKeyStroke(std::vector<VirtualKeyCode>* modifierKeyCodes,
                                           std::vector<VirtualKeyCode>* keyCodes)
{
    CInputBuilder builder;
    if (modifierKeyCodes != NULL) builder.AddKeyDown(modifierKeyCodes);
    if (keyCodes != NULL) builder.AddKeyPress(keyCodes);
    if (modifierKeyCodes != NULL) builder.AddKeyUp(modifierKeyCodes);

    SendSimulatedInput(builder);
}

void CKeyboardSimulator::TextEntry(LPCWSTR text)
{
    CInputBuilder inputList;
    SendSimulatedInput(inputList.AddCharacters(text));
}
