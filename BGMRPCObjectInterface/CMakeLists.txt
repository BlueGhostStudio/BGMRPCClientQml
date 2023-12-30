cmake_minimum_required(VERSION 3.18)

include(../cmake/BGMRPC.cmake)

project(
  BGMRPCObjectInterface
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network
  REQUIRED)

if(QT_VERSION_MAJOR EQUAL 5)
  set(LIB_OUTPUT_NAME "BGMRPCObjectInterface2-qt5")
else()
  set(LIB_OUTPUT_NAME "BGMRPCObjectInterface2")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${BGMRPCCommon_includeDir})

add_library(BGMRPCObjectInterface SHARED "")
target_sources(
  BGMRPCObjectInterface
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/ObjectInterface_global.h
          ${CMAKE_CURRENT_LIST_DIR}/caller.cpp
          ${CMAKE_CURRENT_LIST_DIR}/objectinterface.cpp
  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/caller.h
         ${CMAKE_CURRENT_LIST_DIR}/objectinterface.h)

set_target_properties(
  BGMRPCObjectInterface
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME ${LIB_OUTPUT_NAME}
             PUBLIC_HEADER "caller.h;objectinterface.h"
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  BGMRPCObjectInterface
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
          BGMRPCCommon.${BGMRPC_VERSION_MAJOR})

install(TARGETS BGMRPCObjectInterface
        LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
                COMPONENT BGMRPC_remote)

install(
  TARGETS BGMRPCObjectInterface
  LIBRARY DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}
          COMPONENT BGMRPC_remote_dev
          EXCLUDE_FROM_ALL
  PUBLIC_HEADER
    DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT BGMRPC_remote_dev
    EXCLUDE_FROM_ALL)
