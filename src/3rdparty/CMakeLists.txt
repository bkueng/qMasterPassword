if(WIN32)
    target_sources(${TARGET} PRIVATE
        InputBuilder.cpp
        InputSimulator.cpp
        KeyboardSimulator.cpp
        MouseSimulator.cpp
        WindowsInputDeviceStateAdaptor.cpp
    )
endif()
