#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStandardItemModel>
#include <bgmrpcclient.h>
#include <QMenu>
#include "ui_mainwindow.h"
#include "contentdialog.h"

extern BGMRPCClient* RPC;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    void loadCollections ();
    void loadContents (const QString& cmsObj,
                       const QJsonArray& tags = QJsonArray (),
                       bool resetTagsList = true);

private slots:
    void on_lvContents_doubleClicked(const QModelIndex &index);

    void on_actionAdd_Content_triggered();

    void on_actionAdd_Image_triggered();

    void on_actionRemove_Content_triggered();

    void on_actionRearrange_triggered();

    void on_actionAdd_Collection_triggered();

    void on_actionRemove_Collection_triggered();

    void on_actionRename_Collection_triggered();

signals:
    void cmsObjChanged ();

private:
    void initialRemoteSignal ();
    QList < QStandardItem* > collectionRow (const QVariantMap& rowData) const;
    QList < QStandardItem* > contentRow (const QVariantMap& rowData) const;
    void setCurrentCmsObj (const QString& cmsObj);

    QString CurrentCmsObj;
    QStandardItemModel CollectionsModel;
    QStandardItemModel ContentsModel;
    QItemSelection ContentsSelection;
    //ContentDialog dlgContent;

    QItemSelection TagFilterSelection;

    QStringListModel CollTagsModel;
};

#endif // MAINWINDOW_H
