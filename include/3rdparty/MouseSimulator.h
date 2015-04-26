#pragma once
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

#include "IInputSimulator.h"
#include "InputBuilder.h"
#include <vector>

class CMouseSimulator :
	public IMouseSimulator
{
 private:
	static const int MouseWheelClickSize = 120;
	IInputMessageDispatcher* _pmessageDispatcher;
    bool bNeedDelete;
    int SendSimulatedInput(INPUT& input);
	int SendSimulatedInput( CInputBuilder& inputbuilder );


public:
    CMouseSimulator(IInputMessageDispatcher* messageDispatcher);
    CMouseSimulator();
	~CMouseSimulator();
        
    void MoveMouseBy(int pixelDeltaX, int pixelDeltaY);
    void MoveMouseTo(double absoluteX, double absoluteY);
    void MoveMouseToPositionOnVirtualDesktop(double absoluteX, double absoluteY);
    void LeftButtonDown();
    void LeftButtonUp();
    void LeftButtonClick();
    void LeftButtonDoubleClick();
    void RightButtonDown();
    void RightButtonUp();
    void RightButtonClick();
    void RightButtonDoubleClick();
    void XButtonDown(int buttonId);
    void XButtonUp(int buttonId);
    void XButtonClick(int buttonId);
    void XButtonDoubleClick(int buttonId);
    void VerticalScroll(int scrollAmountInClicks);
    void HorizontalScroll(int scrollAmountInClicks);
    };