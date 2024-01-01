cmake_minimum_required(VERSION 3.14)

include(../../cmake/VERSION.cmake)

project(
  Graphviz
  LANGUAGES CXX
  VERSION ${BGMRPC_VERSION})

include(../../cmake/BGMRPC.cmake)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON) set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${BGMRPCCommon_includeDir}
                    ${BGMRPCObjectInterface_includeDir})

add_library(Graphviz SHARED Graphviz_global.h graphviz.cpp graphviz.h)
set_target_properties(
  Graphviz
  PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
             VERSION ${PROJECT_VERSION}
             OUTPUT_NAME Graphviz
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

target_link_libraries(
  Graphviz PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
                   BGMRPCObjectInterface BGMRPCCommon)

install(TARGETS Graphviz LIBRARY DESTINATION ${BGMRPC_REMOTE}/interfaces
                                 COMPONENT BGMRPC_remote_interface_Graphviz)

target_compile_definitions(Graphviz PRIVATE GRAPHVIZ_LIBRARY)
