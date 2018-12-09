# Библиотека дополнительных функций CMake для более эффективного использования связки Qt + CMake.
# Включает в себя:
# - функцию QtDeploy как обертку вызова windeployqt для облегчения формирования дистрибутива ПО под Windows;
# - функцию QtEnvironment для донастройки проектов Visual Studio, использующих Qt.

if (CMAKE_VERSION VERSION_LESS 2.8.11)
    message(FATAL_ERROR "CMake version < 2.8.11")
endif()

# Обертка для работы с windeployqt.
#
# На этапе INSTALL копирует в заданный каталог установки необходимые для запускаемого файла проекта зависимости (*.dll библиотеки).
# Опции функции соотвествует аргументам утилиты windeployqt.
# Поддерживается возмоность насильного добавления зависимости (с помощью опции INCLUDE_MODULES, например, "INCLUDE_MODULES Qt5::Gui"),
# так и насильного исключения зависисти (с помощью опции EXCLUDE_MODULES, например, "EXCLUDE_MODULES Qt5::Gui").
# Присутствует дополнительный параметр DEFAULT_OPTIONS, который задает наиболее оптимальные настройки по умолчанию (подробнее о
# выбранных настройках см. в коде).
# Пример использования:
# QtDeploy(TARGET_EXE ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe DEFAULT_OPTIONS)
function(QtDeploy)
    cmake_parse_arguments(_deploy
        "COMPILER_RUNTIME;FORCE;NO_TRANSLATIONS;NO_OPENGLSW;NO_D3DCOMPILER;NO_ANGLE;DEFAULT_OPTIONS"
        "TARGET_EXE"
        "INCLUDE_MODULES;EXCLUDE_MODULES"
        ${ARGN}
    )

    if (NOT WIN32)
        message(FATAL_ERROR "Deploying only available for Windows")
    endif()
    if (NOT _deploy_TARGET_EXE)
        message(FATAL_ERROR "A TARGET_EXE must be specified")
    endif()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list (APPEND _ARGS --debug )
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        list (APPEND _ARGS --release-with-debug-info )
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        list (APPEND _ARGS --release)
    endif()
    if (_deploy_COMPILER_RUNTIME)
        list(APPEND _ARGS --compiler-runtime)
    endif()
    if (_deploy_FORCE)
        list(APPEND _ARGS --force)
    endif()
    if (_deploy_NO_TRANSLATIONS)
        list(APPEND _ARGS --no-translations)
    endif()
    if (_deploy_NO_OPENGLSW)
        list(APPEND _ARGS --no-opengl-sw)
    endif()
    if (_deploy_NO_D3DCOMPILER)
        list(APPEND _ARGS --no-system-d3d-compiler)
    endif()
    if (_deploy_NO_ANGLE)
        list(APPEND _ARGS --no-angle)
    endif()

    foreach (mod ${_deploy_INCLUDE_MODULES})
        string(TOLOWER ${mod} mod)
        string(REPLACE "qt5::" "" mod ${mod})
        list(APPEND _ARGS "--${mod}")
    endforeach()
    foreach (mod ${_deploy_EXCLUDE_MODULES})
        string(TOLOWER ${mod} mod)
        string(REPLACE "qt5::" "" mod ${mod})
        list(APPEND _ARGS "--no-${mod}")
    endforeach()

    # Опции по умолчанию.
    if (_deploy_DEFAULT_OPTIONS)
        set(_ARGS "")
        list(APPEND _ARGS --force)
        list(APPEND _ARGS --no-compiler-runtime)
        list(APPEND _ARGS --no-translations)
        list(APPEND _ARGS --no-opengl-sw)
        list(APPEND _ARGS --no-system-d3d-compiler)
        list(APPEND _ARGS --no-angle)
        list(APPEND _ARGS --no-svg)
    endif()

    find_program(_deploy_PROGRAM windeployqt PATHS $ENV{QTDIR}/bin/)
    if (_deploy_PROGRAM)
        message(STATUS "Found ${_deploy_PROGRAM}")
    else()
        message(FATAL_ERROR "Unable to find windeployqt")
    endif()

    # Самостоятельно установим VCINSTALLDIR для копирования MSVC redistributable, т.к. в PATH она может быть и не выставлена.
    if ($ENV{VCINSTALLDIR})
        set(VCINSTALLDIR "$ENV{VCINSTALLDIR}")
    elseif (MSVC14)
        set(VCINSTALLDIR "$ENV{VS140COMNTOOLS}/../../VC")
    elseif (MSVC12)
        set(VCINSTALLDIR "$ENV{VS120COMNTOOLS}/../../VC")
    else ()
        message(FATAL_ERROR "Unsupported MSVC version ${MSVC_VERSION}")
    endif()
    file(TO_CMAKE_PATH ${VCINSTALLDIR} VCINSTALLDIR)

    install (CODE "
        message(STATUS \"Running Qt Deploy Tool...\")

        execute_process(COMMAND \"${CMAKE_COMMAND}\" -E env \"VCINSTALLDIR=${VCINSTALLDIR}\"
                                \"${_deploy_PROGRAM}\" ${_ARGS} \"${_deploy_TARGET_EXE}\")
    ")
endfunction()

# Функция дополнительной настройки путей к Qt в файле .vcxproj.user (только для Visual Studio).
function(QtEnvironment)
    if(MSVC)
        # Имя файла с шаблоном для файла .vcxproj.user.
        set(QT_TEMPLATE QtAdditional_template.in)

        # Найдем файл в одном из каталогов дополнительных модулей.
        foreach(PATH ${CMAKE_MODULE_PATH})
            if(EXISTS ${PATH}/${QT_TEMPLATE})
                set(TEMPLATE_PATH ${PATH})
                break()
            endif()
        endforeach()

        # Настроим путь до библиотек Qt (файлов .dll).
        set(USERFILE_QTPATH ${CMAKE_PREFIX_PATH}\\bin)

        # Настроим файл с шаблоном пути до Qt.
        set(USER_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user)
        configure_file(${TEMPLATE_PATH}/${QT_TEMPLATE} ${USER_FILE} @ONLY)
    endif()
endfunction()
