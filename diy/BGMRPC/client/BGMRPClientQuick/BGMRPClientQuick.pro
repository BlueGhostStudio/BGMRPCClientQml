# Add more folders to ship with the application, here
folder_01.source = qml/BGMRPClientQuick
folder_01.target = qml
folder_02.source = qml/clientComponents
folder_02.target = qml
DEPLOYMENTFOLDERS = folder_01 folder_02

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = .
QML2_IMPORT_PATH = .

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    clifile.cpp \
    clifiles.cpp \
    proc.cpp \
    step.cpp \
    bgmrpc.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

QT       += network
COMMONPATH=../../BGMRPCCommon
include (../../base.pri)

HEADERS += \
    clifile.h \
    clifiles.h \
    proc.h \
    step.h \
    bgmrpc.h

OTHER_FILES += \
    qml/test/call.qml \
    qml/clientComponents/CliButton.qml \
    qml/clientComponents/FlickPanel.qml \
    qml/clientComponents/RPC.js \
    qml/clientComponents/SeqButton.qml \
    qml/clientComponents/fileTransfer.js
