#include "jshttpproto.h"

httpWork::httpWork(QObject* parent)
    : QNetworkAccessManager (parent)
{
    connect (this, SIGNAL(startGet(QString,QScriptValue)),
             this, SLOT(_GET_(QString,QScriptValue)));
    connect (this, SIGNAL(startPost(QString,QString,QScriptValue)),
             this, SLOT(_POST_(QString,QString,QScriptValue)));
}


void httpWork::_GET_(const QString& url, const QScriptValue& headers)
{
    QNetworkRequest request = makeRequest (url, headers);

    get(request);
}

void httpWork::_POST_(const QString& url, const QString& data, const QScriptValue& headers)
{
    QNetworkRequest request = makeRequest (url, headers);

    post (request, data.toLatin1 ());
}

QNetworkRequest httpWork::makeRequest(const QString& url,
                                      const QScriptValue& headers)
{
    QNetworkRequest request;
    request.setUrl (QUrl (url));

    if (headers.isObject ()) {
        QScriptValueIterator it (headers);
        while (it.hasNext ()) {
            it.next ();
            request.setRawHeader (it.name ().toLatin1 (),
                                  it.value ().toString ().toLatin1 ());
        }
    }

    return request;
}

/*jsHttp::jsHttp(QObject* parent)
    : QObject (parent), HttpWork (new httpWork (this))
{
    connect (this, SIGNAL (startGet (QString, QScriptValue)),
             HttpWork, SLOT(_GET_(QString, QScriptValue)));
    connect (this, SIGNAL(startPost(QString,QString,QScriptValue)),
             HttpWork, SLOT(_POST_(QString,QString,QScriptValue)));
    connect (HttpWork, SIGNAL(finished(QNetworkReply*)),
             SLOT(relpyFinished(QNetworkReply*)));
}

jsHttp::~jsHttp()
{
    qDebug () << "destructor";
    HttpWork->deleteLater ();
}

void jsHttp::relpyFinished(QNetworkReply* reply)
{
    qDebug () << "replyFinished";
    QScriptValue thisObj
            = SuccessCallback.engine ()->toScriptValue < jsHttp* > (this);
    qDebug () << thisObj.isQObject ();
    if (reply->error ()) {
        if (ErrorCallback.isFunction ()) {
            QScriptValueList args;
            args << reply->error () << reply->errorString ();
            ErrorCallback.call (thisObj, args);
        }
    } else if (SuccessCallback.isFunction ()) {
        QScriptValueList args;
        args << QString (reply->readAll ()) << thisObj;
        SuccessCallback.call (thisObj, args);
    }
}

void jsHttp::get (const QString& url, const QScriptValue& headers)
{
    startGet (url, headers);
}

void jsHttp::post(const QString& url, const QString& data,
                  const QScriptValue& headers)
{
    qDebug () << "post in jsHttp";
    startPost (url, data, headers);
}

void jsHttp::onSuccess(const QScriptValue& callback)
{
    SuccessCallback = callback;
}

void jsHttp::onError(const QScriptValue& callback)
{
    ErrorCallback = callback;
}

void jsHttp::setProxy(quint16 type, const QString& hostName,
                      quint16 port, const QString& user,
                      const QString& password)
{
    qDebug () << type << hostName;
    QNetworkProxy proxy;
    proxy.setType ((QNetworkProxy::ProxyType)type);
    proxy.setHostName (hostName);
    proxy.setPort (port);
    if (!password.isEmpty ()) {
        proxy.setUser (user);
        proxy.setPassword (password);
    }

    HttpWork->setProxy (proxy);
}*/

jsHttpProto::jsHttpProto(QObject* parent)
    :QObject (parent)
{
/*    connect (this, SIGNAL(startGet(QString,QScriptValue)),
             thisHttpObject (), SLOT(_GET_(QString, QScriptValue)));
    connect (this, SIGNAL(startPost(QString,QString,QScriptValue)),
             thisHttpObject (), SLOT(_POST_(QString, QString, QScriptValue)));
    connect (thisHttpObject (), SIGNAL(finished(QNetworkReply*)),
             SLOT(relpyFinished(QNetworkReply*)));*/
}

jsHttpProto::~jsHttpProto()
{
//    delete thisHttpObject ();
    thisHttpObject ()->deleteLater ();
}

void jsHttpProto::get(const QString& url, const QScriptValue& headers,
                      const QScriptValue& callback)
{
//    thisHttpObject ()->get (url, headers);
    if (callback.isValid ()) {
        onSuccess (callback.property ("success"));
        onError (callback.property ("error"));
    }
    //startGet (url, headers);
    thisHttpObject ()->startGet (url, headers);
}

void jsHttpProto::post(const QString& url, const QString& data,
                       const QScriptValue& headers,
                       const QScriptValue& callback)
{
//    thisHttpObject ()->post (url, data, headers);
    if (callback.isValid ()) {
        onSuccess (callback.property ("success"));
        onError (callback.property ("error"));
    }
    // startPost (url, data, headers);
    thisHttpObject ()->startPost (url, data, headers);
}

void jsHttpProto::onSuccess(const QScriptValue& callback)
{
//    thisHttpObject ()->onSuccess (callback);
    SuccessCallback = callback;
    SuccessCallback.setProperty ("http", thisObject ());
}

void jsHttpProto::onError(const QScriptValue& callback)
{
//    thisHttpObject ()->onError (callback);
    ErrorCallback = callback;
    ErrorCallback.setProperty ("http", thisObject ());
}

void jsHttpProto::setProxy(quint16 type, const QString& hostName, quint16 port,
                           const QString& user, const QString& password)
{
    QNetworkProxy proxy;
    proxy.setType ((QNetworkProxy::ProxyType)type);
    proxy.setHostName (hostName);
    proxy.setPort (port);
    if (!password.isEmpty ()) {
        proxy.setUser (user);
        proxy.setPassword (password);
    }

    thisHttpObject ()->setProxy (proxy);
}

void jsHttpProto::initial()
{
    connect (thisHttpObject (), SIGNAL(finished(QNetworkReply*)),
             SLOT(relpyFinished(QNetworkReply*)));
}

void jsHttpProto::relpyFinished(QNetworkReply* reply)
{
    if (reply->error ()) {
        if (ErrorCallback.isFunction ()) {
            QScriptValueList args;
            args << reply->error () << reply->errorString ();
            ErrorCallback.call (ErrorCallback, args);
        }
    } else if (SuccessCallback.isFunction ()) {
        QScriptValueList args;
        args << QString (reply->readAll ());
        SuccessCallback.call (SuccessCallback, args);
    }
}

jsHttpPtr jsHttpProto::thisHttpObject() const
{
    return qvariant_cast < jsHttpPtr > (thisObject ().data ().toVariant ());
}
