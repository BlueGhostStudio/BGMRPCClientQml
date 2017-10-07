#include "propertydialog.h"
#include <QFontDatabase>
#include <QDebug>
#include <QSettings>
#include <QWebEngineSettings>
#include <cmssettings.h>

PropertyDialog::PropertyDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    CMSUrlLineEdit->setText (CMSSettings::CMSUrl ().toString ());

    userLineEdit->setText (CMSSettings::user ());
    passwordLineEdit->setText (CMSSettings::password ());

    fontFamilyComboBox->addItems (QFontDatabase().families ());
    fontFamilyComboBox->setCurrentText (
                CMSSettings::previewFontFamily ());

    resBaseUrlLineEdit->setText (CMSSettings::resBaseUrl ());
}

void PropertyDialog::on_PropertyDialog_accepted()
{
    CMSSettings::setCMSUrl (CMSUrlLineEdit->text ());
    CMSSettings::setUser (userLineEdit->text ());
    CMSSettings::setPassword (passwordLineEdit->text ());

    QString baseUrl = resBaseUrlLineEdit->text ();
    CMSSettings::setResBaseUrl (baseUrl);

    QString previewfontfamily =  fontFamilyComboBox->currentText ();
    CMSSettings::setPreviewFontFamily (previewfontfamily);
    QWebEngineSettings::defaultSettings ()->setFontFamily (
                QWebEngineSettings::StandardFont, previewfontfamily);
}
