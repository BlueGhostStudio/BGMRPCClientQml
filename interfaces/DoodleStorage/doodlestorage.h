#ifndef HWSTORAGE_H
#define HWSTORAGE_H

#include <QFile>
#include <QMutex>
#include <QObject>
#include <objectinterface.h>

namespace NS_BGMRPCObjectInterface
{

class DoodleStorage : public ObjectInterface
{
    Q_OBJECT

public:
    DoodleStorage(QObject* parent = nullptr);

    QVariant storageDoodleStrokes(QPointer<Caller>, const QVariantList& args);

protected:
    bool verification(QPointer<Caller> caller, const QString& method,
                      const QVariantList& args) override;
    void registerMethods() override;

private:
    QMutex m_mutex;

    QFile m_dataFile;
    QByteArray m_buffer;
};

} // namespace NS_BGMRPCObjectInterface

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(int, char**);
}

#endif // HWSTORAGE_H
