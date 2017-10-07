#ifndef CMSSETTINGS_H
#define CMSSETTINGS_H
#include <QSettings>

class CMSSettings
{
public:
    CMSSettings();

    static QUrl CMSUrl ();
    static void setCMSUrl (const QUrl& url);
    static void setCMSUrl (const QString& url);
    static QString resBaseUrl ();
    static bool setResBaseUrl (QString& baseUrl);
    static QString previewFontFamily ();
    static void setPreviewFontFamily (const QString& fontFamily);
    static QString user ();
    static void setUser (const QString& name);
    static QString password ();
    static void setPassword (const QString& pwd);
};


#endif // CMSSETTINGS_H
