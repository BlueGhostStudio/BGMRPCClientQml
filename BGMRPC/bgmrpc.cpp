#include "bgmrpc.h"
#include "client.h"
#include <QDebug>
#include <QLocalSocket>
#include <bgmrpccommon.h>

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
        qInfo().noquote() << "BGMRPC Started";
        QObject::connect(m_ctrlServer, &QLocalServer::newConnection, [=]() {
            ObjectCtrl* objCtrl = new ObjectCtrl(
                this, m_ctrlServer->nextPendingConnection(), this);

            QObject::connect(objCtrl, &ObjectCtrl::registerObject, this,
                             &BGMRPC::newObject);

            QObject::connect(
                objCtrl, &ObjectCtrl::removeObject, [&](const QString& name) {
                    qInfo().noquote()
                        << "Object[" + name + "] has ben disconnected";
                    //                    QLocalServer::removeServer(objCtrl->dataSocketName());
                    m_objects.remove(name);
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

ObjectCtrl* BGMRPC::objectCtrl(const QString& name)
{
    return m_objects[name];
}

void BGMRPC::newObject(const QString& name)
{
    qInfo().noquote() << "Register new Object, Name: " << name;
    m_objects[name] = qobject_cast<ObjectCtrl*>(sender());
    test_addedObject(name);
}

void BGMRPC::newClient()
{
    qInfo().noquote() << "New client connect to Server";

    new Client(this, m_BGMRPCServer->nextPendingConnection());
}
