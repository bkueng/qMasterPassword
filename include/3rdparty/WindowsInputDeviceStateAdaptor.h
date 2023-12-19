#pragma once
#include "IInputSimulator.h"
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

class CWindowsInputDeviceStateAdaptor : public IInputDeviceStateAdaptor {
   public:
    bool IsKeyDown(VirtualKeyCode keyCode);
    bool IsKeyUp(VirtualKeyCode keyCode);
    bool IsHardwareKeyDown(VirtualKeyCode keyCode);
    bool IsHardwareKeyUp(VirtualKeyCode keyCode);
    bool IsTogglingKeyInEffect(VirtualKeyCode keyCode);
};
