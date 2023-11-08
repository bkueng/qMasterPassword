if(WIN32)
    add_subdirectory(scrypt)

    target_sources(${TARGET} PRIVATE
        InputBuilder.cpp
        InputSimulator.cpp
        KeyboardSimulator.cpp
        MouseSimulator.cpp
        WindowsInputDeviceStateAdaptor.cpp
    )
endif()
