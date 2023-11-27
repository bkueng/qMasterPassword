if(WIN32)
    add_subdirectory(scrypt)

    if (NOT DISABLE_FILL_FORM_SHORTCUTS)
        target_sources(${TARGET} PRIVATE
            InputBuilder.cpp
            InputSimulator.cpp
            KeyboardSimulator.cpp
            MouseSimulator.cpp
            WindowsInputDeviceStateAdaptor.cpp
	)
    endif()
endif()
