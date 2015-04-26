#include "stdafx.h"
#include "MouseSimulator.h"
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

       CMouseSimulator::CMouseSimulator(IInputMessageDispatcher* pmessageDispatcher)
        {
          if (pmessageDispatcher == NULL)
            throw "";
            
		     bNeedDelete = false;
            _pmessageDispatcher = pmessageDispatcher;
        }

        CMouseSimulator::CMouseSimulator()
        {
            _pmessageDispatcher = new WindowsInputMessageDispatcher();
			bNeedDelete = true; 
        }



	 CMouseSimulator::~CMouseSimulator(void)
    {
		if(bNeedDelete && _pmessageDispatcher != NULL)
		{
			delete _pmessageDispatcher;
			_pmessageDispatcher = NULL;
		}
			
		
    }


     int CMouseSimulator::SendSimulatedInput(INPUT& input )
    {		
        return (int)_pmessageDispatcher->DispatchInput(input);
    }

    int CMouseSimulator::SendSimulatedInput( CInputBuilder& inputbuilder )
    {
		if ( (inputbuilder.Size() <=0)) return -1;
        return (int)_pmessageDispatcher->DispatchInput(inputbuilder);
    }

        void CMouseSimulator::MoveMouseBy(int pixelDeltaX, int pixelDeltaY)
        {
			 CInputBuilder inputList;
			SendSimulatedInput(inputList.AddRelativeMouseMovement(pixelDeltaX, pixelDeltaY));
        }

        void CMouseSimulator::MoveMouseTo(double absoluteX, double absoluteY)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddAbsoluteMouseMovement(  LOWORD(absoluteX), LOWORD(absoluteY)));
        }

         void CMouseSimulator::MoveMouseToPositionOnVirtualDesktop(double absoluteX, double absoluteY)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddAbsoluteMouseMovementOnVirtualDesktop(LOWORD(absoluteX), LOWORD(absoluteY)));
        }

        void  CMouseSimulator::LeftButtonDown()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonDown(MouseButton::LeftButton));
        }

         void CMouseSimulator::LeftButtonUp()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonUp(MouseButton::LeftButton));
        }

         void CMouseSimulator::LeftButtonClick()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonClick(MouseButton::LeftButton));
        }

         void CMouseSimulator::LeftButtonDoubleClick()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonDoubleClick(MouseButton::LeftButton));
        }

         void CMouseSimulator::RightButtonDown()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonDown(MouseButton::RightButton));
        }

         void CMouseSimulator::RightButtonUp()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonUp(MouseButton::RightButton));
        }

         void CMouseSimulator::RightButtonClick()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonClick(MouseButton::RightButton));
        }

         void CMouseSimulator::RightButtonDoubleClick()
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseButtonDoubleClick(MouseButton::RightButton));
       }

         void CMouseSimulator::XButtonDown(int buttonId)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseXButtonDown(buttonId));
        }

         void CMouseSimulator::XButtonUp(int buttonId)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseXButtonUp(buttonId));
        }

         void CMouseSimulator::XButtonClick(int buttonId)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseXButtonClick(buttonId));
        }

         void CMouseSimulator::XButtonDoubleClick(int buttonId)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseXButtonDoubleClick(buttonId));
        }

         void CMouseSimulator::VerticalScroll(int scrollAmountInClicks)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseVerticalWheelScroll(scrollAmountInClicks * MouseWheelClickSize));
        }

         void CMouseSimulator::HorizontalScroll(int scrollAmountInClicks)
        {
			CInputBuilder inputList;
			SendSimulatedInput(inputList.AddMouseHorizontalWheelScroll(scrollAmountInClicks * MouseWheelClickSize));
        }