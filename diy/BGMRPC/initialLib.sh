#!/bin/bash

qt_ver=(5.13.2 5)
BGMRPC_ver=(1.0.0 1)

qt5libs=(libQt5Core libQt5Network libQt5Qml libQt5Sql libQt5WebSockets)
otherlibs=(libicui18n libicuuc libicudata)
bgmrpclibs=(libBGMRPCCommon libBGMRPC)

for lib in ${qt5libs[*]}
do
    ln -f -s ${lib}.so.${qt_ver[0]} ${lib}.so.${qt_ver[1]}
done

for lib in ${bgmrpclibs[*]}
do
    ln -f -s ${lib}.so.${BGMRPC_ver[0]} ${lib}.so.${BGMRPC_ver[1]}
done

for lib in ${otherlibs[*]}
do
    libname=`ls ${lib}.so.*.*`
    ln -f -s $libname `echo $libname | sed 's/\.[^.]*$//'`
done
