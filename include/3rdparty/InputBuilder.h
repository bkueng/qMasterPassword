#pragma once
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

#include "VirtualKeyCode.h"
#include <vector>


class  CInputBuilder
{
private:
	std::vector<INPUT> _inputList;


public:
	CInputBuilder(void);
	~CInputBuilder(void);

	INPUT* ToArray();
	inline unsigned int Size()
	{
		return _inputList.size();
	}

	INPUT operator[](int position);

    CInputBuilder& AddKeyDown(VirtualKeyCode keyCode);
    CInputBuilder& AddKeyUp(VirtualKeyCode keyCode);
    CInputBuilder& AddKeyPress(VirtualKeyCode keyCode);

	CInputBuilder& AddKeyUp( std::vector<VirtualKeyCode>* pmodifierKeyCodes);
	CInputBuilder& AddKeyDown( std::vector<VirtualKeyCode>* pmodifierKeyCodes);
	CInputBuilder& AddKeyPress( std::vector<VirtualKeyCode>* pkeyCodes);

    CInputBuilder& AddCharacter(wchar_t character);
    CInputBuilder& AddCharacters(std::vector<wchar_t> characters);
	CInputBuilder& AddCharacters(LPCWSTR characters);


    CInputBuilder& AddRelativeMouseMovement(int x, int y);
    CInputBuilder& AddAbsoluteMouseMovement(int absoluteX, int absoluteY);
    CInputBuilder& AddAbsoluteMouseMovementOnVirtualDesktop(int absoluteX, int absoluteY);
    CInputBuilder& AddMouseButtonDown(MouseButton button);
    CInputBuilder& AddMouseXButtonDown(int xButtonId);
    CInputBuilder& AddMouseButtonUp(MouseButton button);
    CInputBuilder& AddMouseXButtonUp(int xButtonId);
    CInputBuilder& AddMouseButtonClick(MouseButton button);
    CInputBuilder& AddMouseXButtonClick(int xButtonId);
    CInputBuilder& AddMouseButtonDoubleClick(MouseButton button);
    CInputBuilder& AddMouseXButtonDoubleClick(int xButtonId);
    CInputBuilder& AddMouseVerticalWheelScroll(int scrollAmount);


	CInputBuilder& AddHardware( DWORD msg, DWORD paramh, DWORD paraml);
 
};

  
  
