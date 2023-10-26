target_sources(${TARGET} PRIVATE
    command_line.cpp
    crypto.cpp
    edit_site_widget.cpp
    exception.cpp
    global.cpp
    identicon.cpp
    import_export.cpp
    keypress.cpp
    keypress_linux.cpp
    logging.cpp
    main.cpp
    main_class.cpp
    main_window.cpp
    password_generator_widget.cpp
    pushbutton_delegate.cpp
    settings_widget.cpp
    shortcuts_widget.cpp
    user.cpp
    user_widget.cpp
)

if(WIN32)
    target_sources(${TARGET} PRIVATE
        keypress_windows.cpp
    )
endif()

add_subdirectory(3rdparty)
