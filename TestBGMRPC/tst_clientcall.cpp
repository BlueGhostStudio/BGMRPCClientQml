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
    void test_client();
    void test_relatedCaller();
    void test_thread();

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
    qSetMessagePattern(
        "%{if-debug}[D]%{function}:%{line} - "
        "%{endif}%{if-warning}[W]%{function}:%{line} - "
        "%{endif}%{if-critical}[C]%{function}:%{line} - "
        "%{endif}%{if-fatal}[F]%{function}:%{line} - %{endif}%{message}");
    /*QLocalServer::removeServer(NS_BGMRPC::BGMRPCCtrlSocket);

    RPC = new NS_BGMRPC::BGMRPC;
    RPC->start();

    QObject::connect(RPC, &NS_BGMRPC::BGMRPC::test_addedObject,
                     [&](const QString& name) {
                         qDebug() << "已加入对象，对象名为：" << name;
                         currentAddObject = name;
                     });*/
}

void ClientCall::cleanupTestCase()
{ /*delete RPC;*/
}

void ClientCall::test_client()
{
    QSKIP("skip test_client");
    NS_BGMRPCClient::BGMRPCClient testClient1;
    testClient1.connectToHost(QUrl("ws://127.0.0.1:8000"));
    QObject::connect(
        &testClient1, &NS_BGMRPCClient::BGMRPCClient::connected, [&]() {
            qDebug() << "connected";
            (new CallChain([&](CallChain* cc) {
                testClient1.callMethod(cc, "testRemoteObject", "plus",
                                       {100, 200});
            }))
                ->then(
                    [&](CallChain* cc, const QVariant& data) {
                        int d = data.toList()[0].toInt();
                        qDebug() << d;

                        testClient1.callMethod(cc, "testRemoteObject", "plus",
                                               {d, 200});
                    },
                    nullptr)
                ->then(
                    [](CallChain* cc, const QVariant& data) {
                        qDebug() << data.toList()[0].toInt();
                        cc->final(nullptr);
                    },
                    nullptr)
                ->exec();
        });

    QTest::qWait(500);
}

void ClientCall::test_relatedCaller()
{
    //    QSKIP("skip test_relatedCaller");
    NS_BGMRPCClient::BGMRPCClient testClient1;
    NS_BGMRPCClient::BGMRPCClient testClient2;
    testClient1.connectToHost(QUrl("ws://127.0.0.1:8000"));
    testClient2.connectToHost(QUrl("ws://127.0.0.1:8000"));
    bool testClient1Connected = false;
    bool testClient2Connected = false;

    QObject::connect(&testClient1, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { testClient1Connected = true; });
    QObject::connect(&testClient2, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { testClient2Connected = true; });

    QObject::connect(
        &testClient1, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal,
        [](const QString& obj, const QString& sig, const QJsonArray& args) {
            qDebug() << "testClient1"
                     << "onRemoteSignal" << obj << sig << args[0].toString();
        });
    QObject::connect(
        &testClient2, &NS_BGMRPCClient::BGMRPCClient::onRemoteSignal,
        [](const QString& obj, const QString& sig, const QJsonArray& args) {
            qDebug() << "testClient2"
                     << "onRemoteSignal" << obj << sig << args[0].toString();
        });

    QTest::qWaitFor(
        [&]() -> bool { return testClient1Connected && testClient2Connected; },
        500);
    QVERIFY2(testClient1Connected && testClient2Connected,
             "一个或全部客户端未连接");
    qDebug() << testClient1Connected << testClient2Connected;

    (new CallChain([&](CallChain* cc) {
        testClient1.callMethod(cc, "testRemoteObject", "join", {"usr1"});
    }))
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << "client1 join - " << data.toList()[0].toBool();
                testClient2.callMethod(cc, "testRemoteObject", "join",
                                       {"usr2"});
            },
            nullptr)
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << "client2 join - " << data.toList()[0].toBool();
                testClient2.callMethod(cc, "testRemoteObject",
                                       "foreachRelatedCaller", {});
            },
            nullptr)
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << "client2 foreachRelatedCaller return - "
                         << data.toList()[0].toBool();
                testClient1.callMethod(cc, "testRemoteObject",
                                       "broadcastSignal", {});
            },
            nullptr)
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << data.toList()[0];
                cc->final(nullptr);
            },
            nullptr)
        ->exec();

    QTest::qWait(500);
}

void ClientCall::test_thread()
{
    NS_BGMRPCClient::BGMRPCClient testClient1;
    NS_BGMRPCClient::BGMRPCClient testClient2;
    testClient1.connectToHost(QUrl("ws://127.0.0.1:8000"));
    testClient2.connectToHost(QUrl("ws://127.0.0.1:8000"));
    bool testClient1Connected = false;
    bool testClient2Connected = false;

    QObject::connect(&testClient1, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { testClient1Connected = true; });
    QObject::connect(&testClient2, &NS_BGMRPCClient::BGMRPCClient::connected,
                     [&]() { testClient2Connected = true; });

    QTest::qWaitFor(
        [&]() -> bool { return testClient1Connected && testClient2Connected; },
        500);
    QVERIFY2(testClient1Connected && testClient2Connected,
             "一个或全部客户端未连接");
    qDebug() << testClient1Connected << testClient2Connected;

    (new CallChain([&](CallChain* cc) {
        testClient1.callMethod(cc, "testRemoteObject", "testThread",
                               {"testClient1"});
    }))
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << data;
                cc->final(nullptr);
            },
            nullptr)
        ->exec();

    (new CallChain([&](CallChain* cc) {
        testClient2.callMethod(cc, "testRemoteObject", "testThread",
                               {"testClient2"});
    }))
        ->then(
            [&](CallChain* cc, const QVariant& data) {
                qDebug() << data;
                cc->final(nullptr);
            },
            nullptr)
        ->exec();

    QTest::qWait(5000);
}

QTEST_MAIN(ClientCall)

#include "tst_clientcall.moc"
