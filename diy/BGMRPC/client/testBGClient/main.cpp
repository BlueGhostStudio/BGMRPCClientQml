#include <QCoreApplication>
#include <bgmrpcclient.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BGMRPCClient client;
    QObject::connect(&client, &BGMRPCClient::remoteSignal,
                     [=] (const QString& obj,
                     const QString& signal,
                     const QJsonArray& args) {
        qDebug () << obj << signal << args;
    });
    client.setUrl("ws://127.0.0.1", 8000);
    client.connectToHost();
    qDebug () << client.callMethod("CMS", "js", QJsonArray ({
                                                                "login",
                                                                "bgstudio",
                                                                "123456"
                                                            }));
//    qDebug () << client.callMethod ("CMS", "js", QJsonArray ({
//                                                                 "createCollection",
//                                                                 "blog",
//                                                                 "blog"
//                                                             }));
//    qDebug () << client.callMethod ("cmsblog", "js", QJsonArray ({
//                                                                     "test"
//                                                                 }));

//    qDebug () << client.callMethod ("cmsblog", "js", QJsonArray ({
//                                                                     "content",
//                                                                     "1"
//                                                                 }));
//    qDebug () << client.callMethod ("CMS", "js", QJsonArray ({
//                                                                 "removeCollection",
//                                                                 "blog"
//                                                             }));
    qDebug () << client.callMethod ("CMS", "js", QJsonArray ({
                                                                 "listCollections",
                                                             }));
    qDebug () << "ok";

    return a.exec();
}
