#include "doodlestorage.h"
#include <QDir>
#include <QJsonObject>
#include <QMutexLocker>
#include <QtMath>

using namespace NS_BGMRPCObjectInterface;

DoodleStorage::DoodleStorage(QObject* paretnt) : ObjectInterface(paretnt)
{
    registerMethods();

    m_dataFile.setFileName("py/Handwritten/Doodle/DoodelStorage");

    /*m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("py/Handwritten/HWStorage/handwritten.db");
    m_database.open();*/
}

/*rawData += toRaw(_data_.type << 6 | _data_.color << 4 | _data_.shade, 1);
rawData += toRaw(parseInt(_data_.preSize * 100), 2);
rawData += toRaw(parseInt(_data_.size * 100), 2);
rawData += toRaw(parseInt(_data_.prePos.x * 10), 2);
rawData += toRaw(parseInt(_data_.prePos.y * 10), 2);
rawData += toRaw(parseInt(_data_.pos.x * 10), 2);
rawData += toRaw(parseInt(_data_.pos.y * 10), 2);*/

QVariant DoodleStorage::storageDoodleStrokes(QPointer<Caller> /*caller*/,
                                             const QVariantList& args)
{
    QMutexLocker locker(&m_mutex);

    // m_buffer += args[0].toByteArray();
    //
    QJsonObject strokeJson = args[0].toJsonObject();

    QByteArray strokeData(13, '\x00');
    strokeData[0] = strokeJson["type"].toInt() << 6 |
                    strokeJson["color"].toInt() << 4 |
                    strokeJson["shade"].toInt();

    unsigned int preSize = qFloor(strokeJson["preSize"].toDouble() * 100);
    strokeData[1] = preSize >> 8;
    strokeData[2] = preSize & 0x0f;

    unsigned int size = qFloor(strokeJson["size"].toDouble() * 100);
    strokeData[3] = size >> 8;
    strokeData[4] = size & 0x0f;

    unsigned int prePosX =
        qFloor(strokeJson["prePos"].toObject()["x"].toDouble() * 10);
    strokeData[5] = prePosX >> 8;
    strokeData[6] = prePosX & 0x0f;

    unsigned int prePosY =
        qFloor(strokeJson["prePos"].toObject()["y"].toDouble() * 10);
    strokeData[7] = prePosY >> 8;
    strokeData[8] = prePosY & 0x0f;

    unsigned int posX =
        qFloor(strokeJson["pos"].toObject()["x"].toDouble() * 10);
    strokeData[9] = posX >> 8;
    strokeData[10] = posX & 0x0f;

    unsigned int posY =
        qFloor(strokeJson["pos"].toObject()["y"].toDouble() * 10);
    strokeData[11] = posY >> 8;
    strokeData[12] = posY & 0x0f;

    qDebug() << "--->" << strokeData << "<----";

    m_buffer += strokeData;

    if (m_buffer.length() >= 64 * 13) {
        if (m_dataFile.open(QIODevice::Append)) {
            m_dataFile.write(m_buffer);
            m_dataFile.close();
        } else
            qWarning().noquote() << "Can't open file";
    }

    return QVariant();
}

bool DoodleStorage::verification(QPointer<Caller> caller, const QString& method,
                                 const QVariantList& /*args*/)
{
    if (method == "storageDoodleStrokes" && caller->ID() != -1)
        return false;

    return true;
}

void DoodleStorage::registerMethods()
{
    m_methods["storageDoodleStrokes"] =
        REG_METHOD(DoodleStorage, storageDoodleStrokes);
}

ObjectInterface* create(int, char**)
{
    return new DoodleStorage;
}
