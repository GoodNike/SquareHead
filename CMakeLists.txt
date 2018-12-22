cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(squarehead)

# Опции проекта.
option(SNAP_BUILD "Snap package build" OFF)

# Директория для поиска сценариев CMake.
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    include(${PROJECT_SOURCE_DIR}/cmake/CMakeOptions.txt)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Gui Widgets)

if(UNIX)
    include(GNUInstallDirs)
endif()

set(MAINWINDOW
    "src/UI/MainWindow.cpp"
    "src/UI/MainWindow.h"
    "src/UI/MainWindow.ui"
)

set(OPTIONSDIALOG
    "src/UI/OptionsDialog.cpp"
    "src/UI/OptionsDialog.h"
    "src/UI/OptionsDialog.ui"
)

set(SOURCES
    "src/main.cpp"
    "src/GlobalOptions.cpp"
    "src/GlobalOptions.h"
    "src/Field.cpp"
    "src/Field.h"
    "src/FieldView.cpp"
    "src/FieldView.h"
    "src/Game.cpp"
    "src/Game.h"
    "src/GameLogger.cpp"
    "src/GameLogger.h"
    "src/Player/Player.cpp"
    "src/Player/Player.h"
    "src/Player/PlayerAIVeryEasy.cpp"
    "src/Player/PlayerAIVeryEasy.h"
    "src/Player/PlayerAIEasy.cpp"
    "src/Player/PlayerAIEasy.h"
    "src/Player/PlayerAIMedium.cpp"
    "src/Player/PlayerAIMedium.h"
    "src/Player/PlayerAIHard.cpp"
    "src/Player/PlayerAIHard.h"
    "src/Player/PlayerAIVeryHard.cpp"
    "src/Player/PlayerAIVeryHard.h"
    "src/Player/PlayerHuman.cpp"
    "src/Player/PlayerHuman.h"
)

find_package(Git)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    RESULT_VARIABLE git_result
                    OUTPUT_VARIABLE git_output
                    ERROR_VARIABLE  git_error
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT ${git_result} EQUAL 0)
        message("Command \"${GIT_EXECUTABLE} describe in ${PROJECT_SOURCE_DIR}\" failed with output:\n${git_error}")
    else()
        set(APP_VERSION_STRING ${git_output})
    endif()
endif()

# Разбор строки с версией следующего вида: "v1.0.0".
if(${APP_VERSION_STRING} MATCHES "v([0-9]+)\\.([0-9]+).([0-9]*)")
    set(APP_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(APP_VERSION_MINOR ${CMAKE_MATCH_2})
    set(APP_VERSION_PATCH ${CMAKE_MATCH_3})
else()
    set(APP_VERSION_MAJOR "0")
    set(APP_VERSION_MINOR "0")
    set(APP_VERSION_PATCH "0")
endif()
message(STATUS "Building SquareHead v${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")

set(VERSION_H_IN  "${PROJECT_SOURCE_DIR}/cmake/Version.h.in")
set(VERSION_H     "${PROJECT_BINARY_DIR}/Version.h")
configure_file(${VERSION_H_IN} ${VERSION_H})

source_group("Sources" FILES ${SOURCES} ${VERSION_H})
source_group("UI" FILES ${MAINWINDOW} ${OPTIONSDIALOG})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${MAINWINDOW} ${OPTIONSDIALOG} ${SOURCES} ${VERSION_H})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

set(TS_FILES
    "data/${PROJECT_NAME}_en_US.ts"
    "data/${PROJECT_NAME}_ru_RU.ts"
)
find_package(Qt5LinguistTools)
qt5_create_translation(QM_FILES ${PROJECT_SOURCE_DIR}/src ${TS_FILES})
add_custom_target(translations ALL DEPENDS ${QM_FILES})

# Для Linux добавим библиотеку многопоточности, т.к. без нее std::thread не работает.
if (UNIX)
    get_target_property(LIBRARIES ${PROJECT_NAME} LINK_LIBRARIES)
    target_link_libraries(${PROJECT_NAME} ${LIBRARIES} pthread)
endif()

if (UNIX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nikolay Krasheninnikov")
    set(CPACK_PACKAGE_VENDOR "No company")
    set(CPACK_PACKAGE_CONTACT "nkrasheninnikov@yandex.ru")
    set(CPACK_PACKAGE_VERSION_MAJOR "${APP_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${APP_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${APP_VERSION_PATCH}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Short discription: SquareHead classic game")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Full discription: SquareHead classic game")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.6.3)")

    include(CPack)
endif()

if (SNAP_BUILD)
    message(STATUS "Performing SNAP build")
    add_definitions(-DSNAP_BUILD)
endif()

if (UNIX)
    if (SNAP_BUILD)
        set(INSTALL_PREFIX /usr/${CMAKE_INSTALL_DATAROOTDIR})
    else()
        set(INSTALL_PREFIX ${CMAKE_INSTALL_DATAROOTDIR})
    endif()

    # Программа и переводы.
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION games
    )
    install(
        FILES ${QM_FILES}
        DESTINATION ${INSTALL_PREFIX}/qt5/translations
    )

    # Меню.
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/squarehead
        DESTINATION ${INSTALL_PREFIX}/menu
    )

    # desktop-файл.
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/squarehead.desktop
        DESTINATION ${INSTALL_PREFIX}/applications
    )

    # Иконки.
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/pixmaps/squarehead.xpm
        DESTINATION ${INSTALL_PREFIX}/pixmaps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/16x16/squarehead.png
        DESTINATION ${INSTALL_PREFIX}/icons/hicolor/16x16/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/32x32/squarehead.png
        DESTINATION ${INSTALL_PREFIX}/icons/hicolor/32x32/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/64x64/squarehead.png
        DESTINATION ${INSTALL_PREFIX}/icons/hicolor/64x64/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/128x128/squarehead.png
        DESTINATION ${INSTALL_PREFIX}/icons/hicolor/128x128/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/256x256/squarehead.png
        DESTINATION ${INSTALL_PREFIX}/icons/hicolor/256x256/apps
    )
else()
    install(
        TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION games
    )
endif()

if(WIN32)
    include(QtAdditional)
    QtDeploy(TARGET_EXE ${CMAKE_INSTALL_PREFIX}/games/${PROJECT_NAME}.exe FORCE)
    QtEnvironment()
endif()
