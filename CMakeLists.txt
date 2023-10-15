cmake_minimum_required(VERSION 3.18)
set(TARGET qMasterPassword)
project(${TARGET}
   LANGUAGES CXX
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_SKIP_RPATH ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus Test)

if(QT_VERSION LESS 6.3.0)
    # manual project setup for older Qt versions
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(USE_FOLDERS ON)
    set(QT_TARGET_FOLDER QtInternalTargets)
    include(GNUInstallDirs)

    add_executable(${TARGET} WIN32)

else()
    # only available in qt >= 6.3.0
    qt_standard_project_setup()

    qt_add_executable(${TARGET} WIN32)
endif()

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(ui)

if(LINUX)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET} PRIVATE
            -Wno-unused-parameter
        )
    endif()
endif()

target_compile_definitions(${TARGET} PRIVATE
    APP_NAME="${TARGET}"
)

target_link_libraries(${TARGET} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

if(UNIX)
    target_link_libraries(${TARGET} PRIVATE
        Qt::DBus
        X11
        Xtst
        crypto
        scrypt
    )
endif()

if(WIN32)
    target_link_libraries(${TARGET} PRIVATE
        libeay32
        scrypt
        ws2_32
    )
endif()

install(
    TARGETS ${TARGET}
)
