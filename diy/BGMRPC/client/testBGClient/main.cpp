#include <QCoreApplication>
#include <bgmrpcclient.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BGMRPCClient client;
    qDebug () << "test";
    QObject::connect(&client, &BGMRPCClient::remoteSignal,
                     [=] (const QString& obj,
                     const QString& signal,
                     const QJsonArray& args) {
        qDebug () << "remoteSignal" << obj << signal << args;
    });
    client.setUrl("ws://127.0.0.1", 8000);
    client.connectToHost();

    client.callMethod("BGMessageDemo", "js", QJsonArray ({"join"}));
    qDebug () << client.callMethod("BGMessageDemo", "js", QJsonArray ({
                                                                          "message",
                                                                          "group",
                                                                          "bg",
                                                                          "blueghost"
                                                                      }));

    return a.exec();
}
