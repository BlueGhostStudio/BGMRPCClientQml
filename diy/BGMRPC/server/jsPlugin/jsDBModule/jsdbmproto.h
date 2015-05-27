#ifndef JSDBMPROTO_H
#define JSDBMPROTO_H

#include <QObject>
#include <bgmrpc.h>
#include <jsobjectclass.h>
#include <bgmrpcdbmodule.h>
#include <datasource.h>

using namespace BGMircroRPCServer;


typedef BGMRPCDBModule* DBMPtr;
class jsDBModuleProto;
template <>
class protoTypeInfo < jsDBModuleProto >
{
public:
    typedef DBMPtr dataType;
    static QString className () { return "BGMRPCDBModule"; }
    static bool isNull (DBMPtr d) { return !d; }
    static DBMPtr nullData () { return NULL; }
    static dataType newObject () { return new BGMRPCDBModule; }
};

class jsDBModuleProto : public QObject, public QScriptable
{
    Q_PROPERTY(QScriptValue RPCObj READ RPCObj WRITE setRPCObj)
    Q_PROPERTY(QString DBUser READ DBUser WRITE setDBUser)
    Q_PROPERTY(QString DBPassword READ DBPassword WRITE setDBPassword)
    Q_PROPERTY(QString DBConnectOpt READ DBConnectOpt WRITE setDBConnectOpt)
    Q_PROPERTY(int DBPort READ DBPort WRITE setDBPort)
    Q_PROPERTY(QString DBHost READ DBHost WRITE setDBHost)

public:
    jsDBModuleProto (QObject* parent = 0);

    void setRPCObj (const QScriptValue& obj);

    Q_INVOKABLE bool setDBConnection (const QString& type,
                                      const QString& connName);
    Q_INVOKABLE bool switchDatabase (const QString& DB);
    Q_INVOKABLE void active (const QScriptValue& _proc);

    Q_INVOKABLE void setDBUser (const QString& DBUser);
    Q_INVOKABLE QString DBUser () const;
    Q_INVOKABLE void setDBPassword (const QString& pwd);
    Q_INVOKABLE QString DBPassword () const;
    Q_INVOKABLE void setDBConnectOpt (const QString& opt);
    Q_INVOKABLE QString DBConnectOpt () const;
    Q_INVOKABLE void setDBPort (int port);
    Q_INVOKABLE int DBPort () const;
    Q_INVOKABLE void setDBHost (const QString& host);
    Q_INVOKABLE QString DBHost () const;

    Q_INVOKABLE bool addDataSet(const QString& DS,
                                         const QString& source);
    Q_INVOKABLE bool addRTable (const QString& mDS, const QString& source,
                                const QString& mKey, const QString& rKey);
    Q_INVOKABLE void addRTable (const QString& mDS, const QString& DSName,
                                const QString& source, const QString& mKey,
                                const QString& rKey);

    Q_INVOKABLE void update (const QScriptValue& _proc, const QString& SRC,
                             const QString& cursor, const QString& field,
                             const QScriptValue& value);
    Q_INVOKABLE void update (const QScriptValue& _proc, const QString& DS,
                             const QString& field, const QScriptValue& value);
    Q_INVOKABLE void updateRow (const QScriptValue& _proc, const QString& SRC,
                                const QString& cursor,
                                const QScriptValue& rowData);
    Q_INVOKABLE void updateRow (const QScriptValue& _proc, const QString& DS,
                                const QVariantMap& rowData);
    Q_INVOKABLE QScriptValue newRowRlShData (const QScriptValue& _proc,
                                     const QString& table);

    Q_INVOKABLE void setCursor (const QScriptValue& _proc,
                                const QString& DS, const QString& cursor);
    Q_INVOKABLE void CRNext (const QScriptValue& _proc, const QString& DS);
    Q_INVOKABLE void CRBack (const QScriptValue& _proc, const QString& DS);
    Q_INVOKABLE void CRFirst (const QScriptValue& _proc, const QString& DS);
    Q_INVOKABLE void CRLast (const QScriptValue& _proc, const QString& DS);

private:
    BGMRPCDBModule* thisDBM () const;
    void moveCursor (const QScriptValue& _proc, const QString& DS,
                     SELRECOPT opt);
};

typedef jsObjectClass < jsDBModuleProto > jsDBModuleClass;
Q_DECLARE_METATYPE (const BGMRPCDBModule*)
Q_DECLARE_METATYPE (BGMRPCDBModule*)
Q_DECLARE_METATYPE (jsDBModuleClass*)

#endif // JSDBMPROTO_H
