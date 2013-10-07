#include <bgmrpcinterface.h>
#include <QJsonArray>
#include <QJsonObject>
#include <datatype.h>
#include <getopt.h>
#include <iostream>
#include <QJsonArray>
#include <QHostInfo>
#include <common.h>

using namespace std;
// BGMRCall meth.plus -i1 -i2

using namespace BGMircroRPCInterface;
using namespace BGMircroRPC;
BGMRPCInterface theClient;


void printReturned (const QJsonValue& retVal)
{
    if (retVal.isBool ())
        cout << retVal.toBool () << endl;
    else if (retVal.isDouble ())
        cout << retVal.toDouble () << endl;
    else if (retVal.isString ())
        cout << retVal.toString ().toStdString ()<< endl;
    else if (retVal.isArray ()) {
        foreach (QJsonValue val, retVal.toArray ())
            printReturned (val);
    } else if (retVal.isObject ()) {
        QJsonObject retObj = retVal.toObject ();
        QJsonObject::iterator it;
        for (it = retObj.begin (); it != retObj.end (); ++it) {
            cout << it.key ().toStdString () << ": ";
            printReturned (it.value ());
        }
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    QJsonArray args;

    QHostAddress addr;
    quint16 port = 8000;
    QString host;
    QString Usage = QObject::tr ("Usage: BGMRCall obj.method"
                                 " [-d DoubleArg | -b BoolArg"
                                 " | -s StrArg | -o JsonObjectArg]"
                                 " [-i Address | -h host] [-p Port]"
                                 " [-l loop] [-m bookmark] [-M bookmark] [L]"
                                 " [-R bookmark] [-P] [-D] [-T]");

    int opt;
    bool loop = false;

    bool setBookMark = false;
    bool useBookMark = false;
    bool removeBookMark = false;
    bool listBookMarks = false;
    QString setBookMarkName;
    QString useBookMarkName;
    QString removeBookMarkName;

    bool setProxy = false;
    bool disableProxy = false;
    int proxyType = QNetworkProxy::Socks5Proxy;

    while ((opt = getopt (argc, argv, "d:b:s:o:i:h:p:l:m:M:R:LPDT:")) != -1) {
        QString vstr (optarg);

        switch (opt) {
        case 'd':
            args.append (vstr.toDouble ());
            break;
        case 'b':
            if  (vstr.contains (QRegExp ("^true$", Qt::CaseInsensitive)))
                args.append (true);
            else
                args.append (false);
            break;
        case 's':
            args.append (vstr);
            break;
        case 'o':
        {
            QJsonDocument json = QJsonDocument::fromJson (vstr.toUtf8 ());
            if (json.isArray ())
                args.append (json.array ());
            else if (json.isObject ())
                args.append (json.object ());
        }
            break;
        case 'p':
            port = vstr.toUInt ();
            break;
        case 'i':
            addr.setAddress (vstr);
            break;
        case 'h':
            host = vstr;
            break;
        case 'l':
            loop = true;
            break;
        case 'M':
            setBookMark = true;
            setBookMarkName = vstr;
            break;
        case 'm':
            useBookMark = true;
            useBookMarkName = vstr;
            break;
        case 'R':
            removeBookMark = true;
            removeBookMarkName = vstr;
            break;
        case 'L':
            listBookMarks = true;
            break;
        case 'P':
            setProxy = true;
            break;
        case 'D':
            disableProxy = true;
            break;
        case 'T':
            proxyType = vstr.toInt ();
            if (proxyType > 5)
                proxyType = QNetworkProxy::Socks5Proxy;
            break;
        default:
            cerr << "Invalid option." << endl;
            cerr << Usage.toStdString () << endl;
            exit (0);
        }
    }

    QSettings cliSetting ("BG", "BGMRCli");
    if (listBookMarks) {
        cliSetting.beginGroup ("bookmarks");
        foreach (QString b, cliSetting.childGroups ()) {
            cliSetting.beginGroup (b);
            cout << b.toStdString () << " - "
                 << "host: " << cliSetting.value ("host").toString ().toStdString () << " | "
                 << "ip: " << cliSetting.value ("ip").toString ().toStdString ()  << " | "
                 << "port: " << cliSetting.value ("port").toInt () << endl;
            cliSetting.endGroup ();
        }

        cliSetting.endGroup ();
        return 0;
    }
    if (removeBookMark) {
        cliSetting.beginGroup ("bookmarks");
        cliSetting.remove (removeBookMarkName);
        cliSetting.endGroup ();
        return 0;
    }
    if (setBookMark) {
        cliSetting.beginGroup ("bookmarks");
        cliSetting.beginGroup (setBookMarkName);
        cliSetting.setValue ("ip", addr.toString ());
        cliSetting.setValue ("port", QString::number (port));
        cliSetting.setValue ("host", host);
        cliSetting.endGroup ();
        cliSetting.endGroup ();
        return 0;
    }

    if (useBookMark) {
        bookMake (cliSetting, useBookMarkName,
                  addr, port, host);
    }

    if (setProxy && !setBookMark) {
        QString proxyHost;
        if (!host.isEmpty ())
            proxyHost = host;
        else if (!addr.isNull ())
            proxyHost = addr.toString ();

        if (!proxyHost.isEmpty ()) {
            cliSetting.beginGroup ("proxy");
            cliSetting.setValue ("host", proxyHost);
            cliSetting.setValue ("port", QString::number (port));
            cliSetting.setValue ("type", proxyType);
            cliSetting.endGroup ();
        } else
            cliSetting.remove ("proxy");

        return 0;
    }

    QString obj;
    QString method;

    if (!disableProxy)
        theClient.setProxy (proxy (cliSetting));

    if (optind < argc) {
        QString call (argv[optind]);
        obj = call.section ('.', 0, 0);
        method = call.section ('.', 1);

    } else {
        cerr << Usage.toStdString () << endl;
        exit (0);
    }

    QCoreApplication a(argc, argv);

    getAddrHost (addr, host);
    theClient.setHost (host);
    theClient.setServerAddress (addr, port);
    returnedValue_t retVals = theClient.callMethod (obj, method, args, loop);
    for (int i = 0; i < retVals.count (); ++i)
        printReturned (retVals[i]);

    if (!loop)
        QTimer::singleShot(0, &a, SLOT(quit()));

    return a.exec();
}
