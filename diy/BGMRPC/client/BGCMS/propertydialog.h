#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QtWidgets>
#include "ui_propertydialog.h"

class PropertyDialog : public QDialog, private Ui::PropertyDialog
{
    Q_OBJECT

public:
    explicit PropertyDialog(QWidget *parent = 0);

private slots:
    void on_PropertyDialog_accepted();
};

#endif // PROPERTYDIALOG_H
