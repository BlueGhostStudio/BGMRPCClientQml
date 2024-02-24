#include "bgmrpc.h"

#include <bgmrpccommon.h>

#include <QDebug>
#include <QLocalSocket>
#include <QProcess>
#include <QSslKey>

#include "client.h"
#include "objectctrl.h"
#include "serverctrl.h"

using namespace NS_BGMRPC;

BGMRPC::BGMRPC(QObject* parent)
    : m_defaultSettings(defaultEtcDir + "/BGMRPC.conf", QSettings::IniFormat),
      QObject(parent) {
    m_serverCtrlServer = new QLocalServer(this);
    m_serverCtrlServer->setSocketOptions(QLocalServer::UserAccessOption);

    m_objectSocketServer = new QLocalServer(this);
    m_objectSocketServer->setSocketOptions(QLocalServer::WorldAccessOption);
}

BGMRPC::~BGMRPC() { m_serverCtrlServer->close(); }

bool
BGMRPC::start() {
    if (m_serverCtrlServer->listen(/*NS_BGMRPC::*/ BGMRPCServerCtrlSocket) &&
        // m_objectCtrlServer->listen(BGMRPCObjectCtrlSocket) &&
        m_objectSocketServer->listen(BGMRPCObjectSocket) &&
        m_BGMRPCServer->listen(m_address, m_port)) {
        qInfo().noquote() << "BGMRPC,start,Started";

        QObject::connect(
            m_serverCtrlServer, &QLocalServer::newConnection, this, [=]() {
                new ServerCtrl(this,
                               m_serverCtrlServer->nextPendingConnection());
            });

        QObject::connect(
            m_objectSocketServer, &QLocalServer::newConnection, this, [=]() {
                new ObjectCtrl(this,
                               m_objectSocketServer->nextPendingConnection());
            });

        QObject::connect(m_BGMRPCServer, &QWebSocketServer::newConnection, this,
                         &BGMRPC::newClient);

        return true;
    } else
        return false;
}

void
BGMRPC::setAddress(const QHostAddress& address) {
    m_address = address;
}

void
BGMRPC::setPort(quint16 port) {
    m_port = port;
}

/*ObjectPlug*
BGMRPC::objectCtrl(const QString& name) const {
    return m_objects[name];
}*/

bool
BGMRPC::checkObject(const QByteArray& name) const {
    return m_objects.contains(name);
}

void
BGMRPC::initial(const QString& file) {
    if (file.isEmpty())
        m_settings = new QSettings();
    else
        m_settings = new QSettings(file, QSettings::IniFormat);

    QString ip =
        m_settings->value("server/ip", m_defaultSettings.value("server/ip"))
            .toString();
    m_address = !ip.isEmpty() ? QHostAddress(ip) : QHostAddress::Any;
    m_port =
        m_settings
            ->value("server/port", m_defaultSettings.value("server/port", 8000))
            .toInt();

    bool ssl = false;
    QString ssl_certificate =
        m_settings
            ->value("server/ssl_certificate",
                    m_defaultSettings.value("server/ssl_certificate"))
            .toString();
    QString ssl_certificate_key =
        m_settings
            ->value("server/ssl_certificate_key",
                    m_defaultSettings.value("server/ssl_certificate_key"))
            .toString();
    if (!ssl_certificate.isEmpty() && !ssl_certificate_key.isEmpty()) {
        ssl_certificate =
            ssl_certificate.replace(QRegularExpression("^~"), QDir::homePath());
        ssl_certificate_key = ssl_certificate_key.replace(
            QRegularExpression("^~"), QDir::homePath());

        QString rootPath = getConfig(CNF_PATH_ROOT);

        if (QDir::isRelativePath(ssl_certificate))
            ssl_certificate = rootPath + '/' + ssl_certificate;
        if (QDir::isRelativePath(ssl_certificate_key))
            ssl_certificate_key = rootPath + '/' + ssl_certificate_key;

        QFile crtFile(ssl_certificate);
        QFile keyFile(ssl_certificate_key);
        if (crtFile.open(QIODevice::ReadOnly) &&
            keyFile.open(QIODevice::ReadOnly)) {
            m_BGMRPCServer =
                new QWebSocketServer(QString(), QWebSocketServer::SecureMode);
            ssl = true;

            QSslConfiguration sslConfiguration;

            QSslCertificate certificate(&crtFile, QSsl::Pem);
            QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);

            sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
            sslConfiguration.setLocalCertificate(certificate);
            sslConfiguration.setPrivateKey(sslKey);

            m_BGMRPCServer->setSslConfiguration(sslConfiguration);
        }

        crtFile.close();
        keyFile.close();
    }

    if (!ssl)
        m_BGMRPCServer =
            new QWebSocketServer(QString(), QWebSocketServer::NonSecureMode);
}

/*void
BGMRPC::ctrl_registerObject(const QString& name) {
    qInfo().noquote() << "BGMRPC,Register Object," << name;
    m_objects[name] = qobject_cast<ObjectCtrl*>(sender());
    emit test_addedObject(name);
}

void
BGMRPC::ctrl_checkObject(const QString& name) {
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    if (m_objects.contains(name))
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x1", 1));
    else
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x0", 1));
}

void
BGMRPC::ctrl_getConfig(quint8 cnf) {
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    QByteArray rootPath =
        m_settings->value("path/root")
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();

    switch (cnf) {
    case CNF_PATH_ROOT:
        theObjCtrl->sendCtrlData(rootPath);
        break;
    case CNF_PATH_BIN:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/bin", rootPath + "/bin")
                .toString()
                .replace(QRegularExpression("^~"), QDir::homePath())
                .toUtf8());
        break;
    case CNF_PATH_INTERFACES:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/interfaces", rootPath + "/interfaces")
                .toString()
                .replace(QRegularExpression("^~"), QDir::homePath())
                .toUtf8());
        break;
    case CNF_PATH_LOGS:
        theObjCtrl->sendCtrlData(
            m_settings->value("path/logs", rootPath + "/logs")
                .toString()
                .replace(QRegularExpression("^~"), QDir::homePath())
                .toUtf8());
        break;
    }
}

void
BGMRPC::ctrl_getSetting(const QByteArray& key) {
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());
    theObjCtrl->sendCtrlData(m_settings->value(key, "").toByteArray());
}

void
BGMRPC::ctrl_detachObject(const QString& name) {
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

void
BGMRPC::ctrl_listObjects() {
    ObjectCtrl* theObjCtrl = qobject_cast<ObjectCtrl*>(sender());

    QByteArray listData;
    bool begin = true;
    foreach (QString objName, m_objects.keys()) {
        if (begin)
            begin = false;
        else
            listData += ',';

        listData += objName.toLatin1();
    }
    if (begin)
        theObjCtrl->sendCtrlData(QByteArray::fromRawData("\x0", 1));
    else
        theObjCtrl->sendCtrlData(listData);
}*/

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

void
BGMRPC::newClient() {
    qInfo().noquote() << "BGMRPC,newClient,New client connected";

    new Client(this, m_BGMRPCServer->nextPendingConnection());
}

QByteArray
BGMRPC::listObjects() const {
    QByteArray listData;
    bool begin = true;
    foreach (QString objName, m_objects.keys()) {
        if (begin)
            begin = false;
        else
            listData += ',';

        listData += objName.toLatin1();
    }

    return listData;
}

void
BGMRPC::stopServer() {
    qInfo() << "BGMRPC,stopServer,Stoped";
    qApp->quit();
}

bool
BGMRPC::detachObject(const QByteArray& name) {
    if (m_objects.contains(name)) {
        m_objects[name]->closeCtrlSocket();
        return true;
    } else
        return false;
}

void
BGMRPC::removeObject(const QByteArray& name) {
    // FINISHED 实现BGMRPC::detachObject, ObjectCtrl
    m_objects.remove(name);
}

bool
BGMRPC::registerObject(ObjectCtrl* ctrl, const QByteArray& name) {
    qInfo().noquote() << "BGMRPC,Register Object," << name;
    // m_objects[name] = ctrl;
    if (m_objects.contains(name))
        return false;
    else {
        m_objects[name] = ctrl;
        return true;
    }
}

QByteArray
BGMRPC::getConfig(quint8 cnf) {
    QByteArray rootPath =
        m_settings
            ->value("path/root",
                    m_defaultSettings.value("path/root", QDir::homePath()))
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();

    QByteArray optPath =
#ifdef REMOTEPATH
        REMOTEPATH;
#else
        QDir::homePath().toUtf8();
#endif

    QByteArray configData;
    switch (cnf) {
    case CNF_PATH_ROOT:
        return rootPath;
    case CNF_PATH_BIN:
        return m_settings
            ->value("path/bin",
                    m_defaultSettings.value("path/bin", optPath + "/bin"))
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();
    case CNF_PATH_ETC:
        return m_settings
            ->value("path/etc",
                    m_defaultSettings.value("path/etc", defaultEtcDir))
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();
    case CNF_PATH_INTERFACES:
        return m_settings
            ->value("path/interfaces",
                    m_defaultSettings.value("path/interfaces",
                                            optPath + "/interfaces"))
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();
    case CNF_PATH_LOGS:
        return m_settings
            ->value("path/logs",
                    m_defaultSettings.value("path/log", rootPath + "/logs"))
            .toString()
            .replace(QRegularExpression("^~"), QDir::homePath())
            .toUtf8();
    }
    return QByteArray(1, '\x0');
}

QByteArray
BGMRPC::getSetting(const QByteArray& key) {
    return m_settings->value(key, m_defaultSettings.value(key, ""))
        .toByteArray();
}
