#ifndef DLGRESBROWER_H
#define DLGRESBROWER_H

#include "ui_resbrowerdialog.h"
#include <bgmrpcclient.h>
#include <QtWidgets>
#include <QListView>
#include <QStringListModel>

extern BGMRPCClient* RPC;

class ResBrowerDialog : public QDialog, private Ui::ResBrowerDialog
{
    Q_OBJECT

public:
    explicit ResBrowerDialog(QWidget *parent = 0);
    ~ResBrowerDialog ();

    QString getResUrl () const;
    QString imgToMarkdown () const;
    QString imgHtmlTag () const;
    QString resource () const;

private:
    void refreshImgs ();
    void addRes (const QString& resName);
    QStandardItemModel RessModel;
    QString ResBaseUrl;
    QSettings Settings;

    QMetaObject::Connection RemoteSignalConnection;
};

#endif // DLGRESBROWER_H
