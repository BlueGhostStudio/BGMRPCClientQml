#ifndef PROC_H
#define PROC_H

#include <QObject>
#include <QtQuick/QQuickItem>

class step;
class _RStep;
class _JSStep;
class _LStep;

extern QJSValue callStepJs;

class proc : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QJSValue call READ call WRITE setCall NOTIFY callChanged)
    Q_PROPERTY(QQmlListProperty < step > steps READ steps)
    Q_PROPERTY(QJSValue callStep READ callStep)

public:
    explicit proc(QQuickItem *parent = 0);

    QJSValue call () const;
    void setCall (const QJSValue& c);
    QQmlListProperty<step> steps();
    QJSValue callStep () const;

    Q_INVOKABLE _RStep* newRStep (const QString& o, const QString& m);
    Q_INVOKABLE _JSStep* newJSStep (const QString& o, const QString& m);
    Q_INVOKABLE _LStep* newLStep ();

signals:
    void callChanged ();

public slots:

private:
    QJSValue Call;

    QList < step* > Steps;

    static void appendStep (QQmlListProperty < step >* list, step* s);
    static int stepCount(QQmlListProperty < step >* list);
    static step* atSteps (QQmlListProperty < step >* list, int index);
};

#endif // PROC_H
