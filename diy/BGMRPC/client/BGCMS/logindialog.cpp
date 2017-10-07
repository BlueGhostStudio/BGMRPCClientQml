#include "logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

QString LoginDialog::user() const
{
    return userLineEdit->text ();
}

QString LoginDialog::password() const
{
    return passwordLineEdit->text ();
}
