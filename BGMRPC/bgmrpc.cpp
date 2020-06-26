#include "bgmrpc.h"
#include "client.h"
#include <QDebug>
#include <QLocalSocket>
#include <QProcess>
#include <bgmrpccommon.h>
#include <stdlib.h>

using namespace NS_BGMRPC;

BGMRPC::BGMRPC(QObject* parent) : QObject(parent)
{
    m_ctrlServer = new QLocalServer(this);
    m_BGMRPCServer = new QWebSocketServer(
        QString(), QWebSocketServer::NonSecureMode, parent);
    m_port = 8000;
    m_address = QHostAddress::Any;
}

BGMRPC::~BGMRPC()
{
    m_ctrlServer->close();
}

bool BGMRPC::start()
{
    if (m_ctrlServer->listen(/*NS_BGMRPC::*/ BGMRPCCtrlSocket) &&
        m_BGMRPCServer->listen(m_address, m_port)) {
        qInfo().noquote() << "BGMRPC,start,Started";

        QObject::connect(m_ctrlServer, &QLocalServer::newConnection, [=]() {
            ObjectCtrl* objCtrl = new ObjectCtrl(
                /*this, */ m_ctrlServer->nextPendingConnection(), this);

            QObject::connect(objCtrl, &ObjectCtrl::registerObject, this,
                             &BGMRPC::ctrl_registerObject);

            QObject::connect(objCtrl, &ObjectCtrl::checkObject, this,
                             &BGMRPC::ctrl_checkObject);

            QObject::connect(objCtrl, &ObjectCtrl::getConfig, this,
                             &BGMRPC::ctrl_getConfig);

            QObject::connect(objCtrl, &ObjectCtrl::detachObject, this,
                             &BGMRPC::ctrl_detachObject);
            QObject::connect(objCtrl, &ObjectCtrl::listObjects, this,
                             &BGMRPC::ctrl_listObjects);

            QObject::connect(
                objCtrl, &ObjectCtrl::removeObject, [&](const QString& name) {
                    qInfo().noquote()
                        << QString("Object(%1),removeObject,Disconnected")
                               .arg(name);
                    m_objects.remove(name);
                });

            QObject::connect(objCtrl, &ObjectCtrl::stopServer, []() {
                qInfo() << "BGMRPC,stopServer,Stoped";
                qApp->quit();
            });
        });

        QObject::connect(m_BGMRPCServer, &QWebSocketServer::newConnection, this,
                         &BGMRPC::newClient);

        return true;
    } else
        return false;
}

void BGMRPC::setAddress(const QHostAddress& address)
{
    m_address = address;
}

void BGMRPC::setPort(quint16 port)
{
    m_port = port;
}

ObjectCtrl* BGMRPC::objectCtrl(const QString& name)
{
    return m_objects[name];
}

void BGMRPC::initial(const QString& file)
{
    if (file.isEmpty())
        m_settings = new QSettings();
    else
        m_settings = new QSettings(file, QSettings::IniFormat);

    QString ip = m_settings->value("server/ip").toString();
    m_address = !ip.isEmpty() ? QHostAddress(ip) : QHostAddress::Any;
    m_port = m_settings->value("server/port", 8000).toInt();
}

void BGMRPC::ctrl_registerObject(const QString& name)
{
    qInfo().noquote() << "BGMRPC,Register Object," << name;
    m_objects[name] = qobject_cast<ObjectCtrl*>(sender());
    test_addedObject(name);
}

void BGMRPC::ctrl_checkObject(const QString& name)
{
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    if (m_objects.contains(name))
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x1", 1));
    else
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x0", 1));
}

void BGMRPC::ctrl_getConfig(quint8 cnf)
{
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    QByteArray rootPath = m_settings->value("path/root")
                              .toString()
                              .replace(QRegExp("^~"), QDir::homePath())
                              .toUtf8();
    switch (cnf) {
    case CNF_PATH_ROOT:
        theObjCtrl->sendCtrlData(rootPath);
        break;
    case CNF_PATH_BIN:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/bin", rootPath + "/bin")
                .toString()
                .replace(QRegExp("^~"), QDir::homePath())
                .toUtf8());
        break;
    case CNF_PATH_INTERFACES:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/interfaces", rootPath + "/interfaces")
                .toString()
                .replace(QRegExp("^~"), QDir::homePath())
                .toUtf8());
        break;
    case CNF_PATH_LOGS:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/logs", rootPath + "/logs")
                .toString()
                .replace(QRegExp("^~"), QDir::homePath())
                .toUtf8());
        break;
    }
}

void BGMRPC::ctrl_detachObject(const QString& name)
{
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    if (m_objects.contains(name)) {
        ObjectCtrl* target = m_objects[name];
        target->m_ctrlStroke->disconnectFromServer();
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x1", 1));
    } else {
        qWarning().noquote()
            << "Detach Object fail: Not existed object" << objectName();
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x0", 1));
    }
}

void BGMRPC::ctrl_listObjects()
{
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());

    QByteArray listData;
    bool begin = true;
    foreach (QString objName, m_objects.keys()) {
        if (begin)
            begin = false;
        else
            listData += ',';

        listData += objName;
    }
    if (begin)
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x0", 1));
    else
        theObjCtrl->sendCtrlData(listData);
}

/*void BGMRPC::createObject(const QString& name, const QString& oif,
                          const QString& crArgs)
{
    QString rootPath = m_settings->value("path/root").toString();
    QDir::setCurrent(rootPath);
    QString interfacesPath = m_settings->value("path/interfaces").toString();
    QProcess::startDetached(rootPath + "/bin/BGMRPCObjectLoader" + " -n " +
                            name + " -I " + interfacesPath + '/' + oif + ' ' +
                            crArgs);
}*/

void BGMRPC::newClient()
{
    qInfo().noquote() << "BGMRPC,newClient,New client connected";

    new Client(this, m_BGMRPCServer->nextPendingConnection());
}
