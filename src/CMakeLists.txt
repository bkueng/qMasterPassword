target_sources(${TARGET} PRIVATE
    command_line.cpp
    crypto.cpp
    edit_site_widget.cpp
    exception.cpp
    global.cpp
    identicon.cpp
    import_export.cpp
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

# OS specific keypress handling implementation
if (LINUX OR (UNIX AND NOT APPLE))
    target_sources(${TARGET} PRIVATE
        keypress_linux.cpp
    )
elseif(WIN32)
    target_sources(${TARGET} PRIVATE
        keypress_windows.cpp
    )
else()
    target_sources(${TARGET} PRIVATE
        keypress.cpp
    )
endif()

add_subdirectory(3rdparty)
