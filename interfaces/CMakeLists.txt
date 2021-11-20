cmake_minimum_required(VERSION 3.18)

project(Interfaces)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Network Qml
  REQUIRED)

if (QT_VERSION_MAJOR EQUAL 5)
    add_subdirectory(PythonInterface)
elseif(QT_VERSION_MAJOR EQUAL 6)
    add_subdirectory(JsPlugin2)
endif()
