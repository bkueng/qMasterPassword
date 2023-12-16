#include "InputSimulator.h"

#include "KeyboardSimulator.h"
#include "MouseSimulator.h"
#include "WindowsInputDeviceStateAdaptor.h"
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

CInputSimulator::CInputSimulator(IKeyboardSimulator* pkeyboardSimulator,
                                 IMouseSimulator* pmouseSimulator,
                                 IInputDeviceStateAdaptor* pinputDeviceStateAdaptor)
{
    _pkeyboardSimulator = pkeyboardSimulator;
    _pmouseSimulator = pmouseSimulator;
    _pinputDeviceState = pinputDeviceStateAdaptor;
    bNeedDelete = false;
}

CInputSimulator::CInputSimulator()
{
    _pkeyboardSimulator = new CKeyboardSimulator();
    _pmouseSimulator = new CMouseSimulator();
    _pinputDeviceState = new CWindowsInputDeviceStateAdaptor();
    bNeedDelete = true;
}

CInputSimulator::~CInputSimulator()
{
    if (bNeedDelete) delete _pkeyboardSimulator;
    delete _pmouseSimulator;
    delete _pinputDeviceState;
}
