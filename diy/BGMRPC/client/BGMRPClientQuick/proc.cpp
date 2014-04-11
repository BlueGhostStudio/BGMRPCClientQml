#include "proc.h"
#include "bgmrpc.h"
#include "step.h"

proc::proc(QQuickItem* parent) :
    QQuickItem(parent)
{
}

QJSValue proc::call() const
{
    return Call;
}

void proc::setCall(const QJSValue& c)
{
    Call = c;
}

QQmlListProperty<step> proc::steps()
{
    return QQmlListProperty < step > (this, 0, &proc::appendStep,
                                      &proc::stepCount,
                                      &proc::atSteps, 0);
}

QJSValue proc::callStep() const
{
    return callStepJs;
}

_RStep* proc::newRStep(const QString& o, const QString& m)
{
    return new _RStep (o, m, this);
}

_JSStep* proc::newJSStep(const QString& o, const QString& m)
{
    return new _JSStep (o, m, this);
}

_LStep*proc::newLStep()
{
    return new _LStep (this);
}

void proc::appendStep (QQmlListProperty<step>* list, step* s)
{
    proc* theProc = qobject_cast < proc* > (list->object);
    if (theProc) {
        s->setParentItem (theProc);
        theProc->Steps.append (s);
    }
}

int proc::stepCount(QQmlListProperty<step>* list)
{
    proc* theProc = qobject_cast < proc* > (list->object);
    if (theProc)
        return theProc->Steps.count ();
    else
        return 0;
}

step* proc::atSteps(QQmlListProperty<step>* list, int index)
{
    proc* theProc = qobject_cast < proc* > (list->object);
    if (theProc)
        return theProc->Steps.at (index);
    else
        return NULL;
}
