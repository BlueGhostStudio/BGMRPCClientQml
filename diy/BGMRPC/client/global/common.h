#ifndef COMMON_H
#define COMMON_H
#include <QSettings>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QHostInfo>

void bookMake (QSettings& setting, const QString& bmName,
               QHostAddress& addr, quint16& port, QString& host)
{
    setting.beginGroup ("bookmarks");
    setting.beginGroup (bmName);
    addr.setAddress (setting.value ("ip").toString ());
    port = setting.value ("port").toInt ();
    host = setting.value ("host").toString ();
    setting.endGroup ();
    setting.endGroup ();
}
QNetworkProxy proxy (QSettings& setting)
{
    setting.beginGroup ("proxy");

    QString host = setting.value ("host").toString ();
    int port = setting.value ("port").toInt ();

    int t = setting.value ("type", QNetworkProxy::Socks5Proxy).toInt ();
    QNetworkProxy::ProxyType type = QNetworkProxy::NoProxy;
    if (t >= 0 && t <= 5)
        type = (QNetworkProxy::ProxyType)t;

    setting.endGroup ();

    if (host.isEmpty ())
        return QNetworkProxy (QNetworkProxy::NoProxy);
    else
        return QNetworkProxy (type, host, port);
}
void getAddrHost (QHostAddress& addr, QString& host)
{
    if (host.isEmpty ()) {
        if (addr.isNull ())
            addr.setAddress ("127.0.0.1");
        host = addr.toString ();
    } else {
        if (addr.isNull ())
            addr = QHostInfo::fromName (host).addresses ().first ();
    }
}

#endif // COMMON_H
