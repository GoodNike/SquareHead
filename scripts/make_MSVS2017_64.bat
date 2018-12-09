:: Скрипт для генерации проекта Visual Studio 2017 64bit

@echo off

SET TOP_DIR="%~dp0\.."
SET SLN_DIR=%TOP_DIR%\msvc2017_64-solution
SET SRC_DIR=%TOP_DIR%
SET QT_DIR=C:\Qt\5.6.3\msvc2015_64
SET CMAKE_CMD=cmake

if exist %SLN_DIR% (rmdir /S /Q %SLN_DIR% || (pause & exit))
mkdir %SLN_DIR% || (pause & exit)
cd %SLN_DIR%

%CMAKE_CMD% %SRC_DIR% -G "Visual Studio 15 2017 Win64" -DCMAKE_CONFIGURATION_TYPES="Debug;Release" -DCMAKE_PREFIX_PATH=%QT_DIR%

pause
