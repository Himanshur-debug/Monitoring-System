cmake_minimum_required(VERSION 3.5)

project(ClientUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ClientUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ClientUI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ClientUI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ClientUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ClientUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(ClientUI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ClientUI)
endif()



set(CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)
configure_file(${CONFIG_DIR}/config.json ${CMAKE_CURRENT_BINARY_DIR}/config.json COPYONLY)

set(CERTIFICATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../Certificates)
configure_file(${CERTIFICATE_DIR}/server.crt ${CMAKE_CURRENT_BINARY_DIR}/server.crt COPYONLY)

set(CLIENT_EXECUTABLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../build/Client)
configure_file(${CLIENT_EXECUTABLE_DIR}/Client ${CMAKE_CURRENT_BINARY_DIR}/Client COPYONLY)
