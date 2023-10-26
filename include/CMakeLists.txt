target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

target_sources(${TARGET} PRIVATE
    edit_site_widget.h
    main_window.h
    password_generator_widget.h
    pushbutton_delegate.h
    settings_widget.h
    shortcuts_widget.h
    user_widget.h
)

add_subdirectory(3rdparty)
