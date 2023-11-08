cmake_minimum_required(VERSION 3.18)
set(TARGET qMasterPassword)
project(${TARGET}
    LANGUAGES C CXX
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_SKIP_RPATH ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus Test)
find_package(OpenSSL REQUIRED)

# exposes ${TS_FILES}
add_subdirectory(data/translations)
set_source_files_properties(${TS_FILES}
    PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations"
)

if(QT_VERSION LESS 6.3.0)
    # manual project setup for older Qt versions
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(USE_FOLDERS ON)
    set(QT_TARGET_FOLDER QtInternalTargets)
    include(GNUInstallDirs)

    if(QT_VERSION LESS 6.0.0)
        qt5_add_translation(QM_FILES ${TS_FILES})
    else()
        qt6_add_translation(QM_FILES ${TS_FILES})
    endif()

    add_executable(${TARGET} WIN32
        ${QM_FILES}
    )
else()
    # only available in qt >= 6.3.0
    qt_standard_project_setup()

    qt_add_executable(${TARGET} WIN32)

    qt_add_lrelease(${TARGET}
	TS_FILES ${TS_FILES}
    )
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
    OpenSSL::Crypto
)
target_include_directories(${TARGET} PRIVATE ${OPENSSL_INCLUDE_DIR})

if(UNIX)
    target_link_libraries(${TARGET} PRIVATE
        Qt::DBus
        X11
        Xtst
        scrypt
    )
endif()

if(WIN32)
    target_link_libraries(${TARGET} PRIVATE
        ws2_32
    )
endif()

install(
    TARGETS ${TARGET}
)
