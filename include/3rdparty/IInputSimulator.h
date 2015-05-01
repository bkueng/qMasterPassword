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


#include "VirtualKeyCode.h"
#include "InputBuilder.h"
#include <vector>


class  IInputMessageDispatcher
    {
public:
		virtual unsigned int DispatchInput(INPUT& input) = 0;
		virtual unsigned int DispatchInput( CInputBuilder& inputbuilder ) =0;
    };

class  IKeyboardSimulator
{
public:
   virtual void KeyDown(VirtualKeyCode keyCode) = 0;
   virtual void KeyUp(VirtualKeyCode keyCode) = 0;
   virtual void KeyPress(VirtualKeyCode keyCode) = 0;   
   virtual void KeyPress(VirtualKeyCode keyCode, const unsigned int count /*= 1*/ ) = 0;
   virtual void ModifiedKeyStroke(std::vector<VirtualKeyCode>* modifierKeyCodes, std::vector<VirtualKeyCode>* keyCodes) = 0;
   virtual void ModifiedKeyStroke(std::vector<VirtualKeyCode>* modifierKeyCodes, VirtualKeyCode keyCode) = 0;
   virtual void ModifiedKeyStroke(VirtualKeyCode modifierKey, std::vector<VirtualKeyCode>* keyCodes) = 0;
   virtual void ModifiedKeyStroke(VirtualKeyCode modifierKeyCode, VirtualKeyCode keyCode) = 0;
   virtual void TextEntry(LPCTSTR text) = 0;
};

class  IMouseSimulator
    {
	public:
		virtual void MoveMouseBy(int pixelDeltaX, int pixelDeltaY) = 0;
		virtual void MoveMouseTo(double absoluteX, double absoluteY)= 0;
		virtual void MoveMouseToPositionOnVirtualDesktop(double absoluteX, double absoluteY)= 0;
		virtual void LeftButtonDown()= 0;
		virtual void LeftButtonUp()= 0;
		virtual void LeftButtonClick()= 0;
		virtual void LeftButtonDoubleClick()= 0;
		virtual void RightButtonDown()= 0;
		virtual void RightButtonUp()= 0;
		virtual void RightButtonClick()= 0;
		virtual void RightButtonDoubleClick()= 0;
		virtual void XButtonDown(int buttonId) = 0;
		virtual void XButtonUp(int buttonId)= 0;
		virtual void XButtonClick(int buttonId)= 0;
		virtual void XButtonDoubleClick(int buttonId)= 0;
		virtual void VerticalScroll(int scrollAmountInClicks)= 0;
    };


  class  IInputDeviceStateAdaptor
    {
	public:
       virtual bool IsKeyDown(VirtualKeyCode keyCode) =0;
       virtual bool IsKeyUp(VirtualKeyCode keyCode)=0;
       virtual bool IsHardwareKeyDown(VirtualKeyCode keyCode)=0;
       virtual bool IsHardwareKeyUp(VirtualKeyCode keyCode)=0;
       virtual bool IsTogglingKeyInEffect(VirtualKeyCode keyCode)=0;
    };

class  IInputSimulator
    {
	public:
       virtual IKeyboardSimulator* GetKeyboard() = 0;
       virtual IMouseSimulator* GetMouse() = 0;
       virtual IInputDeviceStateAdaptor* GetInputDeviceState() = 0;

		virtual unsigned int SendInput(INPUT& input) = 0;
		virtual unsigned int SendInput( CInputBuilder& inputbuilder ) =0;
    };



class  WindowsInputMessageDispatcher : public IInputMessageDispatcher
    {
	public:
         unsigned int DispatchInput(INPUT& input)
        {  
            return ::SendInput( 1  , &input, sizeof(INPUT));
        }

         unsigned int DispatchInput( CInputBuilder& inputbuilder )
        {  
            return ::SendInput( inputbuilder.Size() , inputbuilder.ToArray(), sizeof(INPUT));
        }
	};
