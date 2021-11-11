project(
  BGMRPCCtrl
  LANGUAGES CXX
  VERSION 2.0.0)

include(../cmake/BGMRPC.cmake)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Network
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_executable(BGMRPCCtrl main.cpp)

set_target_properties(
  BGMRPCCtrl
  PROPERTIES OUTPUT_NAME BGMRPCCtrl
             INSTALL_RPATH_USE_LINK_PATH ON
             INSTALL_RPATH "${BGMRPC_REMOTE}/${CMAKE_INSTALL_LIBDIR}")

message(">>>>>${BGMRPCCommon_includeDir}")
include_directories(${BGMRPCCommon_includeDir})

target_link_libraries(
  BGMRPCCtrl PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network
                     BGMRPCCommon)

install(TARGETS BGMRPCCtrl
        RUNTIME DESTINATION ${BGMRPC_REMOTE}/${CMAKE_INSTALL_BINDIR}
                COMPONENT BGMRPC_remote)
