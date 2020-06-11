#include "bgmrpc.h"
#include "client.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QSignalSpy>
#include <QWebSocket>
#include <QtTest>
#include <bgmrpcclient.h>
#include <bgmrpccommon.h>
#include <objectinterface.h>

#include <QRegularExpression>
// add necessary includes here

class ClientCall : public QObject
{
    Q_OBJECT

public:
    ClientCall();
    ~ClientCall();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_HW_doodle();

private:
    //    NS_BGMRPC::BGMRPC* RPC;
    QString currentAddObject;
};

ClientCall::ClientCall()
{
}

ClientCall::~ClientCall()
{
}

void ClientCall::initTestCase()
{
}

void ClientCall::cleanupTestCase()
{ /*delete RPC;*/
}

void ClientCall::test_HW_doodle()
{
    qDebug() << "test_HW_doodle";
    NS_BGMRPCClient::BGMRPCClient testClient1;
    NS_BGMRPCClient::BGMRPCClient testClient2;
    testClient1.connectToHost(QUrl("ws://127.0.0.1:8000"));
    testClient2.connectToHost(QUrl("ws://127.0.0.1:8000"));

    QObject::connect(&testClient1, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { qDebug() << "testClient1 connected"; });
    QObject::connect(&testClient2, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { qDebug() << "testClient2 connected"; });
    QObject::connect(
        &testClient1, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal,
        [&](const QString& obj, const QString& signal, const QJsonArray& args) {
            qDebug() << "client1 on RemoteSignal" << obj << signal << args;
        });
    QObject::connect(
        &testClient2, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal,
        [&](const QString& obj, const QString& signal, const QJsonArray& args) {
            qDebug() << "client2 on RemoteSignal" << obj << signal << args;
        });

    QTest::qWait(500);

    (new CallChain([&](CallChain* cc) {
        testClient1.callMethod(cc, "account", "login", {"blueghost", "123"});
    }))
        ->then(
            [&](CallChain* cc, const QVariant&) {
                testClient1.callMethod(cc, "Doodle", "join", {});
            },
            nullptr)
        ->then(
            [&](CallChain* cc, const QVariant&) {
                testClient1.callMethod(
                    cc, "Doodle", "doodle",
                    {QVariantMap({{"name", "bg"}, {"value", 123}})});
            },
            nullptr)
        ->then(
            [&](CallChain* cc, const QVariant&) {
                testClient1.callMethod(cc, "Doodle", "doodle", {"abc"});
            },
            nullptr)
        ->then([&](CallChain* cc, const QVariant&) { cc->final(nullptr); },
               nullptr)
        ->exec();

    (new CallChain([&](CallChain* cc) {
        testClient2.callMethod(cc, "account", "login", {"test", 123});
    }))
        ->then(
            [&](CallChain* cc, const QVariant&) {
                testClient2.callMethod(cc, "Doodle", "join", {});
            },
            nullptr)
        ->then([&](CallChain* cc, const QVariant&) { cc->final(nullptr); },
               nullptr)
        ->exec();

    QTest::qWait(500);
}

QTEST_MAIN(ClientCall)

#include "tst_clientcall.moc"
