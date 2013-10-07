#include <bgmrpcinterface.h>
#include <QJsonArray>
#include <QJsonObject>
#include <datatype.h>
#include <getopt.h>
#include <iostream>
#include <QJsonArray>
#include <common.h>

using namespace std;
using namespace BGMircroRPCInterface;
using namespace BGMircroRPC;

BGMRPCInterface theClient;

void sendFile (const QString& op,  QString& src, const QString& target,
               const QString& pathID)
{
    src.replace (QRegExp ("^\\s~"), QDir::homePath ());
    QFile file(src);
    if (!file.exists ())
        cerr << "no file exist";
    else {
        cliRawDataTrans* fileTrans
                = theClient.callMethodRaw ("file", "open",
                                           args_t () << target << op << pathID);
        char rt = fileTrans->getRawData () [0];
        if (rt == 1) {
            if (!file.open (QIODevice::ReadOnly))
                cerr << "open error";
            else {
                QByteArray data = file.readAll();
                fileTrans->sendRawData (data);
                file.close ();
            }
        } else if (rt == -1)
            cerr << "Remote file has exist" << endl;
        else if (rt == -2)
            cerr << "Remote file open fail" << endl;
    }
}

void downloadFile (const QString& op, const QString& src, const QString& target,
               bool overlay, const QString& pathID)
{
    QFile file(target);
    if (file.exists () && !overlay)
        cerr << "file has exist";
    else {
        cliRawDataTrans* fileTrans
                = theClient.callMethodRaw ("file", "open",
                                           args_t () << src << op << pathID);
        char rt = fileTrans->getRawData (1) [0];
        if (rt == 1) {
            if (!file.open (QIODevice::WriteOnly | QIODevice::Truncate))
                cerr << "open error";
            else {
                QByteArray data;
                QByteArray part;
                while (!(part = fileTrans->getRawData ()).isEmpty ())
                    data.append (part);

                file.write (data);
                file.close ();
            }
        }  else if (rt == -1)
            cerr << "Remote file no exist" << endl;
        else if (rt == -2)
            cerr << "Remote file open fail" << endl;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString errMsg ("Invalid option.");
    QHostAddress addr;
    quint16 port = 8000;
    QString host;
    bool overlay = false;

    int opt;
    bool loop = false;
    bool useBookMark = false;
    QString bookMarkName;
    bool disableProxy = false;
    QString pathID;
    while ((opt = getopt (argc, argv, "i:h:p:o:ld:m:D")) != -1) {
        QString vstr (optarg);

        switch (opt) {
        case 'i':
            addr.setAddress (vstr);
            break;
        case 'h':
            host = vstr;
            break;
        case 'p':
            port = vstr.toUInt ();
            break;
        case 'o':
            if (vstr == "true")
                overlay = true;
            break;
        case 'l':
            loop = true;
            break;
        case 'd':
            pathID = vstr;
            break;
        case 'm':
            useBookMark = true;
            bookMarkName = vstr;
            break;
        case 'D':
            disableProxy = true;
            break;
        default:
            cerr << errMsg.toStdString () << "1" << endl;
            exit (0);
        }
    }

    QSettings cliSetting ("BG", "BGMRCli");
    if (useBookMark)
        bookMake (cliSetting, bookMarkName, addr, port, host);
    if (!disableProxy)
        theClient.setProxy (proxy (cliSetting));

    QString fileOp;
    QString src;
    QString target;
    if (argc - optind >= 2) {
        for (int i = 0; optind < argc; ++optind, ++i) {
            QString opt (argv [optind]);
            if (i == 0) {
                if (opt == "w" || opt == "wx" || opt == "r")
                    fileOp = opt;
                else {
                    cerr << errMsg.toStdString () << "2"<< endl;
                    exit (0);
                }
            } else if (i == 1)
                src = opt;
            else if (i == 2)
                target = opt;
            else
                break;
        }
        getAddrHost (addr, host);
        theClient.setHost (host);
        theClient.setServerAddress (addr, port);
        if (target.isEmpty ())
            target = src.section (QRegExp ("[/\\\\]"), -1);
        if (fileOp == "r")
            downloadFile (fileOp, src, target, overlay, pathID);
        else
            sendFile (fileOp, src, target, pathID);
    } else {
        cerr << errMsg.toStdString () << "3" << endl;
        exit (0);
    }

    if (!loop)
        QTimer::singleShot(0, &a, SLOT(quit()));
    return a.exec();
}
