#ifndef CLIFILE_H
#define CLIFILE_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

class cliFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName)
    Q_PROPERTY(QString filePath READ filePath)
    Q_PROPERTY(bool atEnd READ atEnd)

public:
    explicit cliFile(QObject *parent = 0);

    Q_INVOKABLE void __destroy ();
    Q_INVOKABLE bool open (const QString& file);
    Q_INVOKABLE void close();
    Q_INVOKABLE QByteArray readAll ();
    Q_INVOKABLE QByteArray read (qint64 maxLen);
    Q_INVOKABLE qint64 write (const QByteArray& data);
    Q_INVOKABLE qint64 fileSize ();

    Q_INVOKABLE QString fileName () const;
    Q_INVOKABLE QString filePath () const;
    Q_INVOKABLE bool atEnd ();

    static QFileInfo fileInfo (const QString& file);

signals:

public slots:

private:
    QFile File;
    QString FileName;
    QString FilePath;
};

#endif // CLIFILE_H
