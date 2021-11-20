cmake_minimum_required(VERSION 3.21)

project(
  PythonInterface
  LANGUAGES CXX
  VERSION 1.0.0)

include(../../cmake/BGMRPC.cmake)

find_package(
  QT NAMES Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt5
  COMPONENTS Core Network
  REQUIRED)

find_package(Python3 COMPONENTS Interpreter Development)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(
  /usr/include/PythonQt ${Python3_INCLUDE_DIRS} ${BGMRPCCommon_includeDir}
  ${BGMRPCObjectInterface_includeDir})
# link_directories(/usr/local/lib/PythonQt)

add_library(PythonInterface SHARED "")
target_sources(
  PythonInterface PRIVATE PythonInterface_global.h pyobj.cpp pyobj.h
                          pythoninterface.cpp pythoninterface.h)
set_target_properties(
  PythonInterface
  PROPERTIES # SOVERSION ${PROJECT_VERSION_MAJOR}
             # VERSION ${PROJECT_VERSION}
             OUTPUT_NAME PythonInterface
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  PythonInterface
  Qt5::Core
  Qt5::Network
  ${Python3_LIBRARIES}
  PythonQt-Qt5-Python3
  BGMRPCObjectInterface
  BGMRPCCommon)

install(TARGETS PythonInterface LIBRARY DESTINATION ${BGMRPC_REMOTE}/interfaces
    COMPONENT BGMRPC_remote_interface_PythonInterface)
