#include "resbrowerdialog.h"
#include "resthumb.h"
#include <QInputDialog>
#include <QFileDialog>
#include "cmssettings.h"

ResBrowerDialog::ResBrowerDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    ResBaseUrl = CMSSettings::resBaseUrl ();
    resourceBaseUrlLineEdit->setText (ResBaseUrl);

    QObject::connect (buttonBox->addButton ("Property",
                                            QDialogButtonBox::ActionRole),
                      &QPushButton::clicked, [this] () {
        QString baseUrl = QInputDialog::getText (this, "Base Url",
                                                 "Url", QLineEdit::Normal,
                                                 ResBaseUrl);
        if (CMSSettings::setResBaseUrl (baseUrl)) {
            ResBaseUrl = baseUrl;
            resourceBaseUrlLineEdit->setText (ResBaseUrl);
            refreshImgs ();
        }
        /*baseUrl.remove (QRegExp ("(^\\s*http://|/\\s*$)"));
        if (!baseUrl.isEmpty ()) {
            ResBaseUrl = baseUrl;
            Settings.setValue ("bgres/baseurl", ResBaseUrl);
            resourceBaseUrlLineEdit->setText (ResBaseUrl);
            refreshImgs ();
        }*/
    });
    QObject::connect (buttonBox->addButton ("Add image",
                                            QDialogButtonBox::ActionRole),
                      &QPushButton::clicked, [this] () {
        QUrl fileName
                = QFileDialog::getOpenFileName (
                      this, "Add Image",
                      QStandardPaths::standardLocations (
                          QStandardPaths::PicturesLocation)[0],
                "Images (*.png *.xpm *.jpg)");

        QFile imgFile (fileName.toString ());
        imgFile.open (QIODevice::ReadOnly);
        qDebug () << RPC->callMethod ("bgres", "addRes", {
                                          fileName.fileName (),
                                          QString (imgFile.readAll ().toBase64 ())
                                      });
    });


    lvResBrower->setModel (&RessModel);

    QStringList resList
            = RPC->callMethod ("bgres", "listRes", {})
              .toStringList ();

    foreach (QString resName, resList) {
        addRes (resName);
    }

    /*
     * Remote signals
     */
    RemoteSignalConnection
            = QObject::connect (RPC, &BGMRPCClient::remoteSignal,
                      [=] (const QString& obj,const QString& sig,
                           const QJsonArray& args) {
        if (obj == "bgres") {
            if (sig == "added")
                addRes (args[0].toString ());
        }
    });
}

ResBrowerDialog::~ResBrowerDialog()
{
    QObject::disconnect (RemoteSignalConnection);
}

QString ResBrowerDialog::getResUrl() const
{
    return imgUrlLineEdit->text ();
}

QString ResBrowerDialog::imgToMarkdown() const
{
    QUrl resUrl (getResUrl ());
    return QString ("![%1](%2)")
            .arg (resUrl.fileName ()).arg (resUrl.toString ());
}

QString ResBrowerDialog::imgHtmlTag() const
{
    QUrl resUrl (getResUrl ());
    return QString (R"html(<img alt="%1" src="%2" width="" height=""/>)html")
            .arg (resUrl.fileName ()).arg (resUrl.toString ());
}

QString ResBrowerDialog::resource() const
{
    if (imgSrcCheckBox->checkState () == Qt::Checked)
        return imgHtmlTag ();
    else
        return imgToMarkdown ();
}

void ResBrowerDialog::refreshImgs()
{
    for (int row = 0; row < RessModel.rowCount (); ++row) {
        QModelIndex index = RessModel.index (row, 0);
        QString resName = index.data (Qt::UserRole + 1).toString ();
        ResThumb* img = qobject_cast < ResThumb* > (
                            lvResBrower->indexWidget (
                                RessModel.index (row, 0)));
        img->setResUrl (QUrl ("http://" + ResBaseUrl + "/" + resName));
    }
}

void ResBrowerDialog::addRes(const QString& resName)
{
    QStandardItem* item = new QStandardItem (resName);
    item->setSizeHint (QSize (64, 64));
    item->setData (resName, Qt::UserRole + 1);
    RessModel.appendRow (item);

    ResThumb* img = new ResThumb (resName);
    img->setResUrl (QUrl ("http://" + ResBaseUrl + "/" + resName));
    lvResBrower->setIndexWidget (item->index (), img);
    QObject::connect (img, &ResThumb::clicked, [=] (const QString& url) {
        imgUrlLineEdit->setText (url);
    });
}
