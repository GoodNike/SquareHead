cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(squarehead)

set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 3)
set(APP_VERSION_PATCH 0)

# Директория для поиска сценариев CMake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    include(${PROJECT_SOURCE_DIR}/cmake/CMakeOptions.txt)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core Gui Widgets)

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
    "src/Player/PlayerHuman.cpp"
    "src/Player/PlayerHuman.h"
)

set(VERSION_H_IN  "${PROJECT_SOURCE_DIR}/cmake/Version.h.in")
set(VERSION_H     "${PROJECT_BINARY_DIR}/Version.h")
configure_file(${VERSION_H_IN} ${VERSION_H})

source_group("Sources" FILES ${SOURCES} ${VERSION_H})
source_group("UI" FILES ${MAINWINDOW} ${OPTIONSDIALOG})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})
add_executable(${PROJECT_NAME} WIN32 ${MAINWINDOW} ${OPTIONSDIALOG} ${SOURCES} ${VERSION_H})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

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
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Short discription: Game")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Full discription: Game")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5widgets5 (>= 5.6.3)")
    set(CPACK_PACKAGE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/games)

    include(CPack)
endif()

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION games
)

if (UNIX)
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/squarehead
        DESTINATION share/menu
    )

    install(
        FILES ${PROJECT_SOURCE_DIR}/data/squarehead.desktop
        DESTINATION share/applications
    )

    install(
        FILES ${PROJECT_SOURCE_DIR}/data/pixmaps/squarehead.xpm
        DESTINATION share/pixmaps
    )

    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/16x16/squarehead.png
        DESTINATION share/icons/hicolor/16x16/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/32x32/squarehead.png
        DESTINATION share/icons/hicolor/32x32/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/64x64/squarehead.png
        DESTINATION share/icons/hicolor/64x64/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/128x128/squarehead.png
        DESTINATION share/icons/hicolor/128x128/apps
    )
    install(
        FILES ${PROJECT_SOURCE_DIR}/data/icons/256x256/squarehead.png
        DESTINATION share/icons/hicolor/256x256/apps
    )
endif()

if(WIN32)
    include(QtAdditional)
    QtDeploy(TARGET_EXE ${CMAKE_INSTALL_PREFIX}/games/${PROJECT_NAME}.exe FORCE)
    QtEnvironment()
endif()
