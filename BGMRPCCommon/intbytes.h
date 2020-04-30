#ifndef COMMON_H
#define COMMON_H
#include <QtCore>

template < typename T >
union dataUnion {
    T _rawData;
    char _bytes[sizeof(T)];
};

template < typename T >
T bytes2int(const QByteArray& data);

template < typename T >
QByteArray int2bytes(T data);

#include "intbytes.cpp"

#endif // COMMON_H
