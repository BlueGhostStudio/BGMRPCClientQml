#ifndef CLIFILES_H
#define CLIFILES_H

#include <QtCore>
#include <QtQuick>
#include <QDir>

class cliFile;

class cliFiles : public QObject
{
    Q_OBJECT
public:
    explicit cliFiles(QJSEngine* e, QObject *parent = 0);

    Q_INVOKABLE QJSValue file ();
    Q_INVOKABLE QJSValue fileInfo (const QString& fileName) const;
    Q_INVOKABLE QJSValue fileInfo (const QFileInfo& info) const;

    Q_INVOKABLE bool cd (const QString& path);
    Q_INVOKABLE QString currentPath () const;
    Q_INVOKABLE QJSValue dir ();

signals:

public slots:

private:
    QJSEngine* Engine;
    QDir Dir;
};

#endif // CLIFILES_H
