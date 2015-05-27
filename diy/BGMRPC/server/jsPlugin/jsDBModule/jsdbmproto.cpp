#include "jsdbmproto.h"
#include <bgmrobject.h>
#include <dataset.h>
#include <misfunctions.h>

jsDBModuleProto::jsDBModuleProto(QObject* parent)
    : QObject (parent)
{

}

void jsDBModuleProto::setRPCObj(const QScriptValue& obj)
{
    thisDBM ()->setRPCObj (
                engine ()->fromScriptValue < BGMRObjectInterface* > (obj));
}


bool jsDBModuleProto::setDBConnection(const QString& type,
                                      const QString& connName)
{
    return thisDBM ()->setDBConnection (type, connName);
}

bool jsDBModuleProto::switchDatabase(const QString& DB)
{
    return thisDBM ()->setDatabase (DB);
}

void jsDBModuleProto::active (const QScriptValue& _proc)
{
    thisDBM ()->active (engine ()->fromScriptValue < BGMRProcedure* >(_proc));
}

void jsDBModuleProto::setDBUser(const QString& user)
{
    thisDBM ()->database ()->setUserName (user);
}

QString jsDBModuleProto::DBUser() const
{
    return thisDBM ()->database ()->userName ();
}

void jsDBModuleProto::setDBPassword(const QString& pwd)
{
    thisDBM ()->database ()->setPassword (pwd);
}

QString jsDBModuleProto::DBPassword() const
{
    return thisDBM ()->database ()->password ();
}

void jsDBModuleProto::setDBConnectOpt(const QString& opt)
{
    thisDBM ()->database ()->setConnectOptions (opt);
}

QString jsDBModuleProto::DBConnectOpt() const
{
    return thisDBM ()->database ()->connectOptions ();
}

void jsDBModuleProto::setDBPort(int port)
{
    thisDBM ()->database ()->setPort (port);
}

int jsDBModuleProto::DBPort() const
{
    thisDBM ()->database ()->port ();
}

void jsDBModuleProto::setDBHost (const QString& host)
{
    thisDBM ()->database ()->setHostName (host);
}

QString jsDBModuleProto::DBHost () const
{
    return thisDBM ()->database ()->hostName ();
}

bool jsDBModuleProto::addDataSet (const QString &DS,
                                  const QString &source)
{
    BGMRPCDBModule* theDBM = thisDBM ();

    bool ok = false;
    if (!theDBM->containsDataSet (DS)) {
        new DBMDataSet (theDBM, theDBM->getSource (source));
        ok = true;
    }

    return ok;
}

bool jsDBModuleProto::addRTable (const QString& mDS, const QString& source,
                                 const QString& mKey, const QString& rKey)
{
    bool ok = false;
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (mDS);
    if (theDataSet)
        ok = theDataSet->addRelationTable (thisDBM ()->getSource (source),
                                           mKey, rKey);

    return ok;
}

void jsDBModuleProto::addRTable (const QString& mDS, const QString& DSName,
                                 const QString& source, const QString& mKey,
                                 const QString& rKey)
{
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (mDS);
    if (theDataSet)
        theDataSet->addRelationTable (DSName, source, mKey, rKey);
}

void jsDBModuleProto::update (const QScriptValue& _proc, const QString& SRC,
                              const QString& cursor, const QString& field,
                              const QScriptValue& value)
{
    DBMDataSource* theSource = thisDBM ()->getSource (SRC);
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    QVariantMap data;
    data[field] = value.toVariant ();
    theSource->commitRowData (proc, base64ToMap (cursor.toLatin1 ()),
                              data);
}

void jsDBModuleProto::update (const QScriptValue& _proc, const QString& DS,
                              const QString& field, const QScriptValue& value)
{
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (DS);
    if (theDataSet) {
        BGMRProcedure* proc
                = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
        QVariantMap data;
        data[field] = value.toVariant ();
        theDataSet->commitRowData (prooc, data);
    }
}

void jsDBModuleProto::updateRow (const QScriptValue& _proc, const QString& SRC,
                                 const QString& cursor,
                                 const QScriptValue& rowData)
{
    DBMDataSource* theSource = thisDBM ()->getSource (SRC);
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    theSource->commitRowData (proc, base64ToMap (cursor.toLatin1 ()),
                              rowData);
}

void jsDBModuleProto::updateRow (const QScriptValue& _proc, const QString& DS,
                                 const QVariantMap& rowData)
{
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (DS);
    if (theDataSet) {
        BGMRProcedure* proc
                = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
        theDataSet->commitRowData (proc, rowData);
    }
}

QScriptValue jsDBModuleProto::newRowRlShData (const QScriptValue& _proc,
                                      const QString& table)
{
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (table);
    QScriptValue theNewRowRlShData;
    if (theDataSet) {
        BGMRProcedure* proc
                = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
        RlShValue theRlShValue
                = theDataSet->getStatus (proc).getRlShValue (theDataSet);
        theNewRowRlShData = engine ()->newObject ();
        theNewRowRlShData.setProperty ("field", theRlShValue.RKey);
        theNewRowRlShData.setProperty ("value", theRlShValue.Value);
    }

    return theNewRowRlShData;
}

void jsDBModuleProto::setCursor(const QScriptValue& _proc, const QString& DS,
                                const QString &cursor)
{
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (DS);
    if (theDataSet)
        theDataSet->setCursor (proc, base64ToMap (cursor.toLatin1 ()));
}

void jsDBModuleProto::CRNext (const QScriptValue& _proc, const QString& DS)
{
    moveCursor (_proc, DS, AFTER_CURSOR);
}

void jsDBModuleProto::CRBack(const QScriptValue& _proc, const QString &DS)
{
    moveCursor (_proc, DS, BEFORE_CURSOR);
}

void jsDBModuleProto::CRFirst(const QScriptValue& _proc, const QString &DS)
{
    moveCursor (_proc, DS, FIRST_RECORD);
}

void jsDBModuleProto::CRLast(const QScriptValue& _proc, const QString &DS)
{
    moveCursor (_proc, DS, LAST_RECORD);
}

BGMRPCDBModule* jsDBModuleProto::thisDBM() const
{
    return qscriptvalue_cast < DBMPtr > (thisObject ().data ());
}

void jsDBModuleProto::moveCursor(const QScriptValue &_proc,
                                 const QString &DS, SELRECOPT opt)
{
    BGMRProcedure* proc = engine ()->fromScriptValue < BGMRProcedure* > (_proc);
    DBMDataSet* theDataSet = thisDBM ()->getDataSet (DS);
    if (theDataSet)
        theDataSet->moveCursor (proc, opt);
}
