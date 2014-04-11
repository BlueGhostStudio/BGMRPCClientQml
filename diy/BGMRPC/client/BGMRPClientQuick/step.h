#ifndef STEP_H
#define STEP_H

#include <QObject>
#include <QtQuick/QQuickItem>
#include <qqmlengine.h>

class BGMRPC;
extern QQmlEngine* engine;

class step : public QQuickItem
{
    Q_OBJECT
public:
    explicit step (QQuickItem *parent = 0);

    virtual void call (const QJSValue &) = 0;
    void doneCallBack (const QJSValue& rets);
    Q_INVOKABLE step* done (const QJSValue& d);

private:
    QJSValue JSOnDone;
};

class _RStep : public step
{
    Q_OBJECT

    Q_PROPERTY(QString object READ object WRITE setObject NOTIFY objectChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
public:
    explicit _RStep(QQuickItem* parent = 0);
    explicit _RStep(const QString& o, const QString& m, QQuickItem* parent = 0);

    QString object () const;
    void setObject (const QString& o);
    QString method () const;
    void setMethod (const QString& m);

    void setRPC (BGMRPC* R);
    Q_INVOKABLE void call (const QJSValue& args);

//    void doneCallBack (const QJSValue& rets);

signals:
    void objectChanged ();
    void methodChanged ();

public slots:

protected:
    BGMRPC* ownRPC ();

    QString Object;
    QString Method;
//    QJSValue JSOnDone;
};

class _JSStep : public _RStep
{
    Q_OBJECT
public:
    explicit _JSStep (QQuickItem* parent = 0);
    explicit _JSStep (const QString& o, const QString& m, QQuickItem* parent = 0);
    Q_INVOKABLE void call (const QJSValue& args);
};

class _LStep : public step
{
    Q_OBJECT

    Q_PROPERTY(QJSValue flow READ flow WRITE setFlow NOTIFY flowChanged)
public:
    explicit _LStep (QQuickItem* parent = 0);

    QJSValue flow () const;
    void setFlow (const QJSValue& f);

    Q_INVOKABLE void call (const QJSValue& args);

signals:
    void flowChanged ();

private:
    QJSValue Fun;
};

#endif // STEP_H
