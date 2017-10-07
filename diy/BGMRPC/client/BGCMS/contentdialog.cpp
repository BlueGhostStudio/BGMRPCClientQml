#include <qtextbrowser.h>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <resbrowerdialog.h>
#include <QWebEngineSettings>
#include "contentdialog.h"

void Document::setText(const QString& text)
{
    ContentText = text;
    emit textChanged (ContentText);
}

ContentDialog::ContentDialog(QWidget* parent)
    : QDialog(parent), PreviewPage (this), PreviewChannel (this)
{
    setupUi(this);
    cbTag->setModel (&TagsModel);
    /*tbContent->document ()->setDefaultStyleSheet (R"css(
* {
    font-size: 11pt;
}
em{
    font-style:italic;
}
strong {
    font-weight: bold;
}
table
{
    border-collapse:collapse;
}
header {
    font-weight: bold;
    font-size: 21pt;
}
strike,del {
    text-decoration: line-through;
}
)css");*/

    wevPreview->setContextMenuPolicy(Qt::NoContextMenu);

    wevPreview->setPage (&PreviewPage);
    PreviewChannel.registerObject ("content", &PreviewDocument);
    PreviewPage.setWebChannel (&PreviewChannel);

    wevPreview->setUrl (QUrl ("qrc:/markdown/index.html"));

    QObject::connect (pbEditContent, &QPushButton::clicked,
                      [=]() {
        stwContent->setCurrentIndex (1);
    });
    QObject::connect (pbBackToPreview, &QPushButton::clicked,
                      [=]() {
        stwContent->setCurrentIndex (0);
    });
    RemoteSignalConnection
            = QObject::connect (RPC, &BGMRPCClient::remoteSignal,
                      [=](const QString& obj,const QString& sig,
                      const QJsonArray& args) {
        if (sig == "updated") {
            if (obj == CMSObj && args[0] == ContentID) {
                setPreview (args[1].toVariant ().toMap ());
                if (args[3].toInt () != RPC->property ("CliID").toInt ()
                    && stwContent->currentIndex () == 1) {
                    setEdit (args[2].toObject ().toVariantMap ());
                }
            }
        } else if (sig == "tagUpdated") {
            if (obj == CMSObj && args[0] == ContentID) {
                TagsModel.setStringList (
                            args[2].toVariant ().toStringList ());
                QString tag = args[1].toString ();
                labPreviewTag->setText (tag);
                if (stwContent->currentIndex () == 1)
                    cbTag->setCurrentIndex (cbTag->findText (tag));
            }
        }
    });

    QObject::connect (cbTag,
                      qOverload < const QString& >(&QComboBox::activated),
                      [=](const QString& text) {
        qDebug () << "activated";
        RPC->callMethod (CMSObj, "js", {
                             "updateTag", ContentID, text
                         });
    });
    QObject::connect (cbTag->lineEdit (), &QLineEdit::returnPressed,
                      [=] () {
        cbTag->activated (cbTag->currentText ());
    });
}

ContentDialog::~ContentDialog()
{
    QObject::disconnect (RemoteSignalConnection);
}

void ContentDialog::openContent(const QString& cmsObj, int cID)
{
    CMSObj = cmsObj;
    ContentID = cID;
    //tbContent->clear ();
    QVariantMap ret
            = RPC->callMethod (CMSObj, "js",
    {"content", ContentID}).toMap ()["content"].toMap ();
    setPreview (ret);
    setEdit (ret);
    stwContent->setCurrentIndex (0);
    exec ();
}

void ContentDialog::setTagSelector(const QStringList& tags)
{
    TagsModel.setStringList (tags);
}

void ContentDialog::updateTags(const QString& obj,const QJsonArray& args)
{
    if (obj == CMSObj && args[0] == ContentID) {
        QString tag = args[1].toString ();
        labPreviewTag->setText (tag);
        if (stwContent->currentIndex () == 1) {
            cbTag->setCurrentIndex (cbTag->findText (tag));
        }
    }
}

void ContentDialog::on_pbSave_clicked()
{
    RPC->callMethod (CMSObj, "js", {
                         "updateContent",
                         ContentID,
                         leTitle->text (),
                         pteContentEdit->toPlainText ()
                     });
}

void ContentDialog::on_pbResBrowser_clicked()
{
    ResBrowerDialog rbdlg (this);
    if (rbdlg.exec () == QDialog::Accepted)
        pteContentEdit->insertPlainText (rbdlg.resource ());
}

void ContentDialog::setPreview(const QVariantMap& data)
{
    PreviewDocument.setText ("");
    QString html = data["content"].toString ()
            .replace ("<table>", "<table border=\"1\">");
    labPreviewTitle->setText (data["title"].toString ());
    labPreviewTag->setText (data["tag"].toString ());
    setWindowTitle (QString ("Preview/Edit - %1 | %2 | %3")
                    .arg (data["title"].toString ())
                    .arg (data["date"].toString ())
                    .arg (data["author"].toString ()));
    PreviewDocument.setText (html);
}

void ContentDialog::setEdit(const QVariantMap& contentData)
{
    leTitle->setText (contentData["title"].toString ());
    //cbTag->setCurrentText (contentData["tag"].toString ());
    cbTag->setCurrentIndex (cbTag->findText (contentData["tag"].toString ()));
    pteContentEdit->setPlainText (contentData["content"].toString ());
}
