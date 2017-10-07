#include "cmssettings.h"

#include <QWebEngineSettings>
#include <QUrl>

CMSSettings::CMSSettings()
{

}

QUrl CMSSettings::CMSUrl()
{
    return QUrl (QSettings ().value ("cmsurl", "").toString ());
}

void CMSSettings::setCMSUrl(const QUrl& url)
{
    QSettings ().setValue ("cmsurl", url.toString ());
}

void CMSSettings::setCMSUrl(const QString& url)
{
    QRegExp re ("^\\s*[a-zA-Z]+://");
    if (re.indexIn (url) == -1)
        QSettings ().setValue ("cmsurl", "ws://" + url);
    else
        QSettings ().setValue ("cmsurl", url);
}

QString CMSSettings::resBaseUrl()
{
    QSettings settings;
    return settings.value ("bgres/baseurl", "127.0.0.1").toString ();
}

bool CMSSettings::setResBaseUrl(QString& baseUrl)
{
    QSettings settings;
    baseUrl.remove (QRegExp ("(^\\s*http://|/\\s*$)"));
    if (!baseUrl.isEmpty ()) {
        settings.setValue ("bgres/baseurl", baseUrl);
        return true;
    } else
        return false;
}

QString CMSSettings::previewFontFamily()
{
    QWebEngineSettings* defaultSettings
            = QWebEngineSettings::defaultSettings ();
    QString defaultFont
            = defaultSettings->fontFamily (QWebEngineSettings::StandardFont);

    return QSettings ().value ("previewfontfamily", defaultFont).toString ();
}

void CMSSettings::setPreviewFontFamily(const QString& fontFamily)
{
    QSettings ().setValue ("previewfontfamily", fontFamily);
}

QString CMSSettings::user()
{
    return QSettings ().value ("user", "guest").toString ();
}

void CMSSettings::setUser(const QString& name)
{
    QSettings ().setValue ("user", name);
}

QString CMSSettings::password()
{
    return QSettings ().value ("password", "").toString ();
}

void CMSSettings::setPassword(const QString& pwd)
{
    QSettings ().setValue ("password", pwd);
}
