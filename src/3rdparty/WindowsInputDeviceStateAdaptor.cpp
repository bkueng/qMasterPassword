#include "stdafx.h"
#include "WindowsInputDeviceStateAdaptor.h"
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

bool CWindowsInputDeviceStateAdaptor::IsKeyDown(VirtualKeyCode keyCode)
        {
			UINT16 result = GetKeyState( (UINT16) keyCode);
            return (result < 0);
        }


        bool CWindowsInputDeviceStateAdaptor::IsKeyUp(VirtualKeyCode keyCode)
        {
            return !IsKeyDown(keyCode);
        }

        bool CWindowsInputDeviceStateAdaptor::IsHardwareKeyDown(VirtualKeyCode keyCode)
        {
            UINT16 result = GetAsyncKeyState((UINT16) keyCode);
            return (result < 0);
        }

        bool CWindowsInputDeviceStateAdaptor::IsHardwareKeyUp(VirtualKeyCode keyCode)
        {
            return !IsHardwareKeyDown(keyCode);
        }

		bool CWindowsInputDeviceStateAdaptor::IsTogglingKeyInEffect(VirtualKeyCode keyCode)
        {
            UINT16 result = GetKeyState((UINT16) keyCode);
            return (result & 0x01) == 0x01;
        }


		