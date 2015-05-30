#include "parsejson.h"

void nextUnspaceChar (const QByteArray& src, int& pos)
{
    while (pos < src.size ()) {
        char ch = src[pos];
        if (ch == ' ' || ch == '\t' || ch == '\n')
            pos++;
        else
            break;
    }
}

QString takeQuoString (const QByteArray& src, int& pos, bool& error)
{
    pos++;
    QString theStr;
    QByteArray tmp;
    while (pos < src.size ()) {
        char ch = src[pos];
        if (ch == '"') {
            pos++;
            break;
        } else {
            tmp += ch;
            if (ch == '\\') {
                pos++;
                tmp += src[pos];
            }
        }

        pos++;
    }

    if (pos == src.size ()) {
        error = true;
        return QString ();
    } else {
        nextUnspaceChar (src, pos);
        theStr = QString::fromUtf8 (tmp);
        return theStr;
    }
}

QJsonValue parse (const QByteArray& src, int& pos, bool& error)
{
    error = false;
    nextUnspaceChar (src, pos);
    char firstChar = src[pos];
    if (firstChar == '{') {
        pos++;
        QJsonObject objValue;
        while (pos < src.size ()) {
            QString name = takeQuoString (src, pos, error);
            if (name.isEmpty () || error || src[pos] != ':') {
                error = true;
                return QJsonValue (QJsonValue::Undefined);
            }

            pos++;
            QJsonValue value = parse (src, pos, error);
            if (value.isNull () && error)
                return QJsonValue ();
            objValue [name] = value;

            char tailChar = src[pos];
            if (tailChar == '}' || tailChar == ',') {
                pos++;
                nextUnspaceChar (src,pos);
                if (tailChar == '}')
                    return objValue;
            } else {
                error = true;
                return QJsonValue ();
            }
        }

        error = true;
        return QJsonValue ();
    } else if (firstChar == '[') {
        pos++;
        QJsonArray arrayValue;
        while (pos < src.size ()) {
            QJsonValue value = parse (src, pos, error);
            if (value.isNull () && error)
                return QJsonValue ();
            arrayValue.append (value);

            char tailChar = src[pos];
            if (tailChar == ']' || tailChar == ',') {
                pos++;
                nextUnspaceChar (src, pos);
                if (tailChar == ']')
                    return arrayValue;
            } else if (tailChar != ',') {
                error = true;
                return QJsonValue ();
            }
        }

        error = true;
        return QJsonValue ();
    } else if (firstChar == '"') {
        QString value = takeQuoString (src, pos, error);
        if (error)
            return QJsonValue ();
        return QJsonValue (value);
    } else {
        QString valStr;
        char ch = firstChar;
        while (pos < src.size ()
               && ch != ','
               && ch != '}'
               && ch != ']') {
            valStr += ch;
            pos++;
            ch = src[pos];
        }
        valStr.trimmed ();
        if (valStr == "true")
            return QJsonValue (true);
        else if (valStr == "false")
            return QJsonValue (false);
        else {
            bool ok;
            double num = valStr.toDouble (&ok);
            if (!ok) {
                error = true;
                return QJsonValue ();
            } else
                return QJsonValue (num);
        }
    }

    error = true;
    return QJsonValue ();
}

QJsonValue parse (const QByteArray& src)
{
    int pos = 0;
    bool error = false;
    return parse (src, pos, error);
}

