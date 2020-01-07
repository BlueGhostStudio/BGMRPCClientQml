#!/bin/bash

OLD_IFS=$IFS
IFS=$(echo -en "\n\b")

PWD=$(cd "`dirname BGMRPC-Desktop\ Qt\ 5.13.2\ GCC\ 64bit/makepkg.sh`";pwd)
qt_ver=5.13.2
qt_path=/opt/Qt/${qt_ver}/gcc_64
BGMRPC_ver=1.0.0
BGMRPC_buildPath=$PWD
fakeroot=$PWD/fakeroot

qt5libs=(Core Network Qml Sql WebSockets)
otherlibs=(libicui18n libicuuc libicudata)
bgmrpclibs=(BGMRPCCommon/libBGMRPCCommon server/BGMRPC/libBGMRPC)
jsModules=(jsDB jsTimer)
autoloadPlugins=(JsPlugin2)

mkdir -p ${fakeroot}/lib
mkdir -p ${fakeroot}/BGMRPC
mkdir -p ${fakeroot}/BGMRPC/JsModules
mkdir -p ${fakeroot}/BGMRPC/JSObjs
mkdir -p ${fakeroot}/BGMRPC/plugins
mkdir -p ${fakeroot}/BGMRPC/plugins/autoload
mkdir -p ${fakeroot}/BGMRPC/sqldrivers

cp ${BGMRPC_buildPath}/initialLib.sh ${fakeroot}/lib

cp ${BGMRPC_buildPath}/server/BGMRServer/BGMRServer ${fakeroot}/BGMRPC


# Qt及相关的链接库
for lib in ${qt5libs[*]}
do
    cp ${qt_path}/lib/libQt5${lib}.so.${qt_ver} ${fakeroot}/lib
done

for lib in ${otherlibs[*]}
do
    cp ${qt_path}/lib/${lib}.so.*.* ${fakeroot}/lib
done

# BGMRPC链接库
for lib in ${bgmrpclibs[*]}
do
    cp ${BGMRPC_buildPath}/${lib}.so.${BGMRPC_ver} ${fakeroot}/lib
done

# jsModule
for lib in ${jsModules[*]}
do
    cp ${BGMRPC_buildPath}/server/JsPlugin2/JsModule/${lib}/lib${lib}.so ${fakeroot}/BGMRPC/JsModules
done

# plugins
for lib in ${autoloadPlugins[*]}
do
    cp ${BGMRPC_buildPath}/server/${lib}/lib${lib}.so ${fakeroot}/BGMRPC/plugins/autoload
done

# sqldriver
cp ${qt_path}/plugins/sqldrivers/* ${fakeroot}/BGMRPC/sqldrivers


IFS=$OLD_IFS
