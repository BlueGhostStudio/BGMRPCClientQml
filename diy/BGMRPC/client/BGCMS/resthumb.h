#ifndef RESTHUMB_H
#define RESTHUMB_H

#include <QtWidgets>
#include <QtNetwork>

class ResThumb : public QLabel
{
    Q_OBJECT

public:
    ResThumb(const QString& name, QWidget* parent=nullptr);
    ~ResThumb ();

    void setResUrl (const QUrl& url);

protected:
    void mousePressEvent (QMouseEvent*);

signals:
    void clicked (const QString& url);

private:
    QNetworkAccessManager ResNAM;
    QNetworkReply* Reply;
    QString ResName;
    QString ResUrl;
};

#endif // RESTHUMB_H
