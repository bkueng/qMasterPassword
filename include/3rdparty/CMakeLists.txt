if(WIN32)
    target_include_directories(${TARGET} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
    )

    target_sources(${TARGET} PRIVATE
        IInputSimulator.h
        InputBuilder.h
        InputSimulator.h
        KeyboardSimulator.h
        MouseSimulator.h
        VirtualKeyCode.h
        WindowsInputDeviceStateAdaptor.h
        stdafx.h
    )
endif()
