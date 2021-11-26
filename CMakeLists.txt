cmake_minimum_required(VERSION 3.18)

project(BGMRPC2 VERSION 1.0.0)

if(NOT DEFINED QT_NAMES)
    SET(QT_NAMES "Qt6" "Qt5")
endif()

find_package(
  QT NAMES ${QT_NAMES}#Qt6 Qt5
  COMPONENTS Core Network Qml
  REQUIRED)

add_subdirectory(BGMRPCCommon)
add_subdirectory(BGMRPCObjectInterface)
add_subdirectory(BGMRPCObjectLoader)
add_subdirectory(BGMRPCCtrl)
add_subdirectory(BGMRPCConsole)

if(QT_VERSION_MAJOR EQUAL 6)
    add_subdirectory(BGMRPCd)
    add_subdirectory(BGMRPCClient)
    add_subdirectory(BGMRPCClientQml)
endif()

add_subdirectory(interfaces)
