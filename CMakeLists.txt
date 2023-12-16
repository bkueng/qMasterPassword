cmake_minimum_required(VERSION 3.18)
set(TARGET qMasterPassword)
project(${TARGET} LANGUAGES C CXX)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_SKIP_RPATH ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets
                                                       LinguistTools)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
endif()
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# exposes ${TS_FILES}
add_subdirectory(data/translations)
# cmake-format: off
# cmake-lint: disable=C0301
set_source_files_properties(
    ${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations")
# cmake-format: on

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

    add_executable(${TARGET} WIN32 ${QM_FILES})
else()
    # only available in qt >= 6.3.0
    qt_standard_project_setup()

    qt_add_executable(${TARGET} WIN32)

    qt_add_lrelease(${TARGET} TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE
                    QM_FILES)
endif()

option(DISABLE_FILL_FORM_SHORTCUTS "Build without fill form support" OFF)
if(DISABLE_FILL_FORM_SHORTCUTS)
    message(STATUS "Fill form shortcuts are disabled")
    target_compile_definitions(${TARGET} PRIVATE DISABLE_FILL_FORM_SHORTCUTS)
else()
    message(STATUS "Fill form shortcuts are enabled")
endif()

# LINUX requires CMake >= 3.25.0
if(LINUX OR (UNIX AND NOT APPLE))
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET} PRIVATE -Wno-unused-parameter)
    endif()
endif()

target_compile_definitions(${TARGET} PRIVATE APP_NAME="${TARGET}")

target_link_libraries(${TARGET} PRIVATE Qt::Core Qt::Gui Qt::Widgets
                                        OpenSSL::Crypto Threads::Threads)
target_include_directories(${TARGET} PRIVATE ${OPENSSL_INCLUDE_DIR})

if(UNIX)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus)
    target_link_libraries(${TARGET} PRIVATE Qt::DBus scrypt)

    if(NOT DISABLE_FILL_FORM_SHORTCUTS)
        # this defines X11_FOUND
        find_package(X11 REQUIRED COMPONENTS Xtst xkbcommon_X11)
    endif()
endif()

if(WIN32)
    target_link_libraries(${TARGET} PRIVATE ws2_32)

    # find path to Qt binaries from qmake location
    get_target_property(QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
    # cmake-format: off
    add_custom_target(
        windeployqt
        COMMAND
            ${QT_BIN_DIR}/windeployqt --release --translations ${TRANSLATIONS}
            --verbose 2
            \"$ENV{WINDEPLOY_DIR}/bin/${CMAKE_PROJECT_NAME}$<TARGET_FILE_SUFFIX:${TARGET}>\"
        COMMENT "Running windeployqt...")
    # cmake-format: on

    add_custom_target(
        makensis
        COMMAND
            makensis -DWINDEPLOY_DIR="$ENV{WINDEPLOY_DIR}"
            -DNSIS_DIR="$ENV{NSIS_DIR}" -v4 -wx
            ${CMAKE_CURRENT_SOURCE_DIR}/data/windows/installer.nsi
        COMMENT "Running makensis...")
endif()

# subdirectories can rely on variables set by find_package() calls
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(ui)

install(TARGETS ${TARGET})
install(FILES ${QM_FILES}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${CMAKE_PROJECT_NAME}/translations)
if(LINUX OR (UNIX AND NOT APPLE))
    install(FILES data/${CMAKE_PROJECT_NAME}.desktop
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES data/${CMAKE_PROJECT_NAME}.appdata.xml
            DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    install(
        FILES data/icons/app_icon.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
        RENAME qmasterpassword.png)
endif()
