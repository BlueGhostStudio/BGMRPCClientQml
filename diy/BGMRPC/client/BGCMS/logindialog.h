#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ui_logindialog.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);

    QString user () const;
    QString password () const;
};

#endif // LOGINDIALOG_H
