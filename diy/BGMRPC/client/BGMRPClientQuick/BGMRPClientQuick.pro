# Add more folders to ship with the application, here
folder_01.source = qml/BGMRPCallQuick
folder_01.target = qml
folder_02.source = qml/test
folder_02.target = qml
folder_03.source = qml/clientComponents
folder_03.target = qml
DEPLOYMENTFOLDERS = folder_01 folder_02 folder_03

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH = .
QML2_IMPORT_PATH = .

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    bgmrpcc.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

QT       += network
include (../../base.pri)

HEADERS += \
    bgmrpcc.h

OTHER_FILES += \
    qml/test/call.qml \
    qml/clientComponents/CliButton.qml \
    qml/clientComponents/RPC.js
