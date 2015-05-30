#ifndef PARSEJSON
#define PARSEJSON
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QtCore>

void nextUnspaceChar (const QByteArray& src, int& pos);
QString takeQuoString (const QByteArray& src, int& pos, bool& error);
QJsonValue parse (const QByteArray& src, int& pos, bool& error);
QJsonValue parse (const QByteArray& src);

#endif // PARSEJSON

