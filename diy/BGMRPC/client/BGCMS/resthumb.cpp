#include "resthumb.h"
#include <QDebug>

ResThumb::ResThumb(const QString& name, QWidget* parent)
    : QLabel (parent)
{
    //setScaledContents (true);
    setMinimumSize (64, 64);
    setMaximumSize (64, 64);
    setAutoFillBackground (true);
    setAlignment (Qt::AlignHCenter | Qt::AlignVCenter);

    ResName = name;
}

ResThumb::~ResThumb()
{
    qDebug () << "deleted";
}

void ResThumb::setResUrl(const QUrl& url)
{
    ResUrl = url.toString ();
    Reply = ResNAM.get (QNetworkRequest (url));

    QObject::connect (Reply, &QNetworkReply::finished,
                      [=] () {
        Reply->deleteLater ();
    });
    QObject::connect (Reply, &QNetworkReply::readyRead,
                      [=]() {
        QByteArray resData = Reply->readAll ();
        QPixmap img;
        img.loadFromData (resData);
        setPixmap (img.width () > img.height ()
                   ? img.scaledToWidth (64, Qt::SmoothTransformation)
                   : img.scaledToHeight (64, Qt::SmoothTransformation));
    });
}

void ResThumb::mousePressEvent (QMouseEvent*)
{
    emit clicked (ResUrl);
}
