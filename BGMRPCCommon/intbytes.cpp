#ifdef COMMON_H

template < typename T >
T bytes2int(const QByteArray& data)
{
    dataUnion< T > __union;

    memcpy(__union._bytes, data.constData(), sizeof(T));

    return __union._rawData;
}

template < typename T >
QByteArray int2bytes(T data)
{
    dataUnion< T > __union;

    __union._rawData = data;

    return QByteArray(__union._bytes, sizeof(T));
}
#endif
