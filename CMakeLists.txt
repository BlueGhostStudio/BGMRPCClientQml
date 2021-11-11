cmake_minimum_required(VERSION 3.18)

project(BGMRPC VERSION 2.0.0)

add_subdirectory(BGMRPCCommon)
add_subdirectory(BGMRPCd)
add_subdirectory(BGMRPCObjectInterface)
add_subdirectory(BGMRPCCtrl)
add_subdirectory(BGMRPCObjectLoader)

add_subdirectory(interfaces)

add_subdirectory(BGMRPCClient)

add_subdirectory(BGMRPCClientQml)
