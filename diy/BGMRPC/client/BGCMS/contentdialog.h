#ifndef CONTENTDIALOG_H
#define CONTENTDIALOG_H

#include <QWebEnginePage>
#include <QWebChannel>
#include <bgmrpcclient.h>
#include "ui_contentdialog.h"

extern BGMRPCClient* RPC;

class Document : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER ContentText NOTIFY textChanged FINAL)

public:
    explicit Document(QObject *parent = nullptr) : QObject(parent) {}

    void setText(const QString &text);

signals:
    void textChanged(const QString &text);

private:
    QString ContentText;
};

class ContentDialog : public QDialog, private Ui::ContentDialog
{
    Q_OBJECT

public:
    explicit ContentDialog (QWidget *parent = 0);
    ~ContentDialog ();

    void openContent (const QString& cmsObj, int cID);
    void setTagSelector (const QStringList& tags);

public slots:
    void updateTags (const QString& obj, const QJsonArray& args);

private slots:
    void on_pbSave_clicked();

    void on_pbResBrowser_clicked();

private:
    void setPreview (const QVariantMap& data);
    void setEdit (const QVariantMap& contentData);

    QWebEnginePage PreviewPage;
    QWebChannel PreviewChannel;
    Document PreviewDocument;
    QString CMSObj;
    int ContentID;
    QStringListModel TagsModel;

    QMetaObject::Connection RemoteSignalConnection;
};

#endif // CONTENTDIALOG_H
