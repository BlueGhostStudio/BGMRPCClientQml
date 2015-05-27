#ifndef JSHTTPPROTO_H
#define JSHTTPPROTO_H

#include "jshttp_global.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QtScript>
#include <bgmrpc.h>
#include <jsobjectclass.h>

using namespace BGMircroRPCServer;

class httpWork:public QNetworkAccessManager {
    Q_OBJECT
public:
    httpWork (QObject* parent = 0);
    ~httpWork () { qDebug () << "destruct in http work"; }

public slots:
    void _GET_ (const QString& url, const QScriptValue& headers);
    void _POST_ (const QString& url, const QString& data,
                 const QScriptValue& headers);

signals:
    void startGet (const QString&, const QScriptValue&);
    void startPost (const QString&, const QString&,
                    const QScriptValue& headers);

private:
    QNetworkRequest makeRequest (const QString& url,
                                 const QScriptValue& headers);
};

/*class jsHttp:public QObject
{
    Q_OBJECT
public:
    jsHttp (QObject* parent = 0);
    ~jsHttp ();

    Q_INVOKABLE void get (const QString& url,
                          const QScriptValue& headers = QScriptValue ());
    Q_INVOKABLE void post (const QString& url, const QString& data,
                           const QScriptValue& headers = QScriptValue ());
    Q_INVOKABLE void onSuccess (const QScriptValue& callback);
    Q_INVOKABLE void onError (const QScriptValue& callback);
    Q_INVOKABLE void setProxy (quint16 type, const QString& hostName,
                               quint16 port,
                               const QString& user = QString (),
                               const QString& password = QString ());
    Q_INVOKABLE void test () { qDebug () << "ok"; }

signals:
    void startGet (QString, QScriptValue);
    void startPost (QString, QString, QScriptValue);

public slots:
    void relpyFinished (QNetworkReply* reply);

private:
    httpWork* HttpWork;
    QScriptValue SuccessCallback;
    QScriptValue ErrorCallback;
};*/


// ===============================

class jsHttpProto;
typedef httpWork* jsHttpPtr;

template <>
class protoTypeInfo < jsHttpProto >
{
public:
    typedef jsHttpPtr dataType;
    static QString className () { return "JSHttp"; }
    static bool isNull (dataType d) { return !d; }
    static dataType nullData () { return NULL; }
    static dataType newObject () { return new httpWork; }
};

class jsHttpProto:public QObject, public QScriptable
{
    Q_OBJECT
public:
    jsHttpProto (QObject* parent = 0);
    ~jsHttpProto ();

    Q_INVOKABLE void get (const QString& url,
                          const QScriptValue& headers = QScriptValue (),
                          const QScriptValue& callback = QScriptValue ());

    Q_INVOKABLE void post (const QString& url, const QString& data,
                           const QScriptValue& headers = QScriptValue (),
                           const QScriptValue& callback = QScriptValue ());

    Q_INVOKABLE void onSuccess (const QScriptValue& callback);
    Q_INVOKABLE void onError (const QScriptValue& callback);
    Q_INVOKABLE void setProxy (quint16 type, const QString& hostName,
                               quint16 port, const QString& user = QString (),
                               const QString& password = QString ());
    Q_INVOKABLE void initial ();

signals:
    void startGet (QString, QScriptValue);
    void startPost (QString, QString, QScriptValue);

private slots:
    void relpyFinished (QNetworkReply* reply);

private:
    jsHttpPtr thisHttpObject () const;
    QScriptValue SuccessCallback;
    QScriptValue ErrorCallback;
};

typedef jsObjectClass < jsHttpProto > jsHttpClass;

Q_DECLARE_METATYPE (const httpWork*)
Q_DECLARE_METATYPE (httpWork*)
Q_DECLARE_METATYPE (jsHttpClass*)

#endif // JSHTTPPROTO_H
