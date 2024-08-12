cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0048 NEW)

project(artplot VERSION 1.4.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("$ENV{ARTF_INSTALL_DIR}" STREQUAL "")
    message(FATAL_ERROR "ARTF_INSTALL_DIR is not set.")
else()
    message(STATUS "ARTF_INSTALL_DIR: $ENV{ARTF_INSTALL_DIR}")
endif()
set(CMAKE_PREFIX_PATH "$ENV{ARTF_INSTALL_DIR}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 3rd-party libs
find_package(absl REQUIRED CONFIG)
# find_package (glog 0.6.0 REQUIRED)
set(Protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Set build options
set(ROBOT_TYPE "LQ" CACHE STRING "select robot. (QI: QuadIP, LQ: LeoQuad, WL: Woly)")
set(TRANSPORT_TYPE "GRPC" CACHE STRING "select transport library. (GRPC: gRPC, ECAL: eCAL)")

if(ROBOT_TYPE STREQUAL "LQ") 
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_LEOQUAD)
    set(conf_artplot_robot_type "LEOQUAD")
    set(conf_artplot_robot_name "LeoQuad")
elseif(ROBOT_TYPE STREQUAL "QI")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_QUADIP)
    set(conf_artplot_robot_type "QUADIP")
    set(conf_artplot_robot_name "Quad-IP")
elseif(ROBOT_TYPE STREQUAL "WL") 
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_WOLY)
    set(conf_artplot_robot_type "WOLY")
    set(conf_artplot_robot_name "Woly")
else()
    message(FATAL_ERROR "ROBOT_TYPE must be set.")
endif()

if(TRANSPORT_TYPE STREQUAL "GRPC")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} USE_TRANSPORT_GRPC;)
    set(conf_artplot_transport_type "GRPC")
elseif(TRANSPORT_TYPE STREQUAL "ECAL") 
    message(FATAL_ERROR "TRANSPORT_TYPE must be set as GRPC. GRPC is the only trasport layer supported currenty.")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} USE_TRANSPORT_ECAL;)
    set(conf_artplot_transport_type "ECAL")
else()
    message(FATAL_ERROR "TRANSPORT_TYPE must be set as GRPC. GRPC is the only trasport layer supported currenty.")
endif()

# message(STATUS "Compiler preprocessor definitions: " ${PROJECT_COMPILE_DEFINITIONS})
# add_compile_definitions(${PROJECT_COMPILE_DEFINITIONS})

# Generate config.h
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h" @ONLY)

# Prepare source files
file(GLOB PROJECT_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "src/*.ui"
    "src/*.h"
    "src/*.qrc"
    "src/*.cc"
)
#set(PROJECT_SOURCES
#    main.cpp
#    mainwindow.h
#    mainwindow.cpp
#    mainwindow.ui
#)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${CMAKE_PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${CMAKE_PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${CMAKE_PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE $ENV{ARTF_INSTALL_DIR}/include)
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE $ENV{ARTF_INSTALL_DIR}/lib)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE protobuf::libprotobuf)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE gRPC::grpc++)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE gRPC::grpc++_reflection)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dtproto_grpc)
# target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ecal_core)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE yaml-cpp)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${CMAKE_PROJECT_NAME})
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
include(CPack)