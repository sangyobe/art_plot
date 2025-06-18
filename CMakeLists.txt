cmake_minimum_required(VERSION 3.5)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0071 NEW)

project(artplot VERSION 1.4.6 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("$ENV{ARTF_INSTALL_DIR}" STREQUAL "")
    set(ARTF_INSTALL_DIR "$ENV{HOME}/.local/ART_Framework")
else()
    file(TO_CMAKE_PATH "$ENV{ARTF_INSTALL_DIR}" ARTF_INSTALL_DIR)
endif()

if("${ARTF_INSTALL_DIR}" STREQUAL "")
    message(FATAL_ERROR "ARTF_INSTALL_DIR is not set.")
else()
    message(STATUS "ARTF_INSTALL_DIR: ${ARTF_INSTALL_DIR}")
endif()

set(CMAKE_INSTALL_PREFIX "${ARTF_INSTALL_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${ARTF_INSTALL_DIR}")

# list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.5.1/gcc_64/") # set Qt install prefix path

# Find packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)
find_package(artf CONFIG REQUIRED)

# --------------------------------------------------------
# Set build options
# --------------------------------------------------------
set(ROBOT_TYPE "LQ" CACHE STRING "select robot. (QI: QuadIP, LQ: LeoQuad, LQW: LeoQuad-Wheel, WL: Woly, DA: Dual Arm, MA: Motor Aging)")
set(TRANSPORT_TYPE "GRPC" CACHE STRING "select transport library. (GRPC: gRPC, ECAL: eCAL)")

if(ROBOT_TYPE STREQUAL "LQ")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_LEOQUAD)
    set(conf_artplot_robot_type "LEOQUAD")
    set(conf_artplot_robot_name "LeoQuad")
elseif(ROBOT_TYPE STREQUAL "LQW")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_LEOQUADWHEEL)
    set(conf_artplot_robot_type "LEOQUADWHEEL")
    set(conf_artplot_robot_name "LeoQuad-Wheel")
elseif(ROBOT_TYPE STREQUAL "QI")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_QUADIP)
    set(conf_artplot_robot_type "QUADIP")
    set(conf_artplot_robot_name "Quad-IP")
elseif(ROBOT_TYPE STREQUAL "WL")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_WOLY)
    set(conf_artplot_robot_type "WOLY")
    set(conf_artplot_robot_name "Woly")
elseif(ROBOT_TYPE STREQUAL "DA")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_DUALARM)
    set(conf_artplot_robot_type "DUALARM")
    set(conf_artplot_robot_name "DualArm")
elseif(ROBOT_TYPE STREQUAL "MA")
    set(PROJECT_COMPILE_DEFINITIONS ${PROJECT_COMPILE_DEFINITIONS} ROBOT_MOTORAGING)
    set(conf_artplot_robot_type "MOTORAGING")
    set(conf_artplot_robot_name "MotorAging")
else()
    message(FATAL_ERROR "ROBOT_TYPE must be set.")
endif()

message(STATUS "************************************************************************************************************")
message(STATUS "supported robot type: (QI: QuadIP, LQ: LeoQuad, LQW: LeoQuad-Wheel, WL: Woly, DA: Dual Arm, MA: Motor Aging)")
message(STATUS "selected ROBOT_TYPE: ${ROBOT_TYPE}")
message(STATUS "************************************************************************************************************")

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

# Compile proto files
file(GLOB_RECURSE PB_SRCS "src/proto/*.proto")
add_library(proto-objects OBJECT ${PB_SRCS})
target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/src/proto")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/src/proto")

target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

protobuf_generate(
    TARGET proto-objects
    IMPORT_DIRS ${PROTO_IMPORT_DIRS}
    PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

# Generate config.h
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h" @ONLY)

# --------------------------------------------------------
# Build
# --------------------------------------------------------

# Prepare source files
file(GLOB PROJECT_SOURCES
    "src/*.cpp"
    "src/*.hpp"
    "src/*.ui"
    "src/*.h"
    "src/*.qrc"
    "src/*.cc"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    message("Set Qt6!")
    qt_add_executable(${CMAKE_PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

# Define target properties for Android with Qt 6 as:
# set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${CMAKE_PROJECT_NAME} SHARED
            ${PROJECT_SOURCES}
        )

    # Define properties for Android with Qt 5 after find_package() calls as:
    # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        message("Set Qt5")
        add_executable(${CMAKE_PROJECT_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/build/src/proto)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE proto-objects)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE artf::dtproto_grpc)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --------------------------------------------------------
# Install
# --------------------------------------------------------
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