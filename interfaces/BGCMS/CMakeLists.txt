cmake_minimum_required(VERSION 3.18)

include(../../cmake/BGMRPC.cmake)

project(
  BGCMSApp
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Qml
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network Qml
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${BGMRPCCommon_includeDir}
                    ${BGMRPCObjectInterface_includeDir})

add_library(BGCMSApp SHARED "" BGCMS_global.h)
target_sources(BGCMSApp PRIVATE ${CMAKE_CURRENT_LIST_DIR}/bgcms.h
                                ${CMAKE_CURRENT_LIST_DIR}/bgcms.cpp)

set_target_properties(
  BGCMSApp
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME BGCMSApp
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")
target_link_libraries(
  BGCMSApp
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
          Qt${QT_VERSION_MAJOR}::Network
          Qt${QT_VERSION_MAJOR}::Qml
          BGMRPCObjectInterface.${BGMRPC_VERSION_MAJOR}
          BGMRPCCommon.${BGMRPC_VERSION_MAJOR})

install(TARGETS BGCMSApp LIBRARY DESTINATION ${BGMRPC_REMOTE}/interfaces
                                 COMPONENT BGMRPC_remote_interface_BGCMSApp)
