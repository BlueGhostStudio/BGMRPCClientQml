#ifndef MTHDADAPTIF_H
#define MTHDADAPTIF_H
// #include <caller.h>

#include <QDebug>
#include <QPointer>
#include <QVariant>
#include <functional>
#include <tuple>
#include <QRegularExpression>

#define UNCHECK 0
#define CHECKTYPE_WEAK 1
#define CHECKTYPE_STRONG 2

namespace NS_BGMRPCObjectInterface {

class Caller;

template <typename T = QVariant>
struct ARG {
    ARG() {}
    ARG(const QString& name) : m_name(name) {}
    ARG(const QString& name, const QVariant& defaultValue)
        : m_name(name),
          m_hasDefaultValue(true),
          m_defaultValue(defaultValue) {}

    int isNumber(QMetaType metaType) const {
        int number;

        switch (metaType.id()) {
        case QMetaType::UShort:
        case QMetaType::UInt:
        case QMetaType::ULong:
        case QMetaType::ULongLong:
            number = 1;
            break;
        case QMetaType::Short:
        case QMetaType::Int:
        case QMetaType::Long:
        case QMetaType::LongLong:
            number = 2;
            break;
        case QMetaType::Float:
        case QMetaType::Double:
            number = 3;
            break;
        default:
            number = 0;
        }

        return number;
    }

    T to(const QVariant& data) const {
        bool ok = false;

        QMetaType metaType = QMetaType::fromType<T>();
        QMetaType dataMetaType = data.metaType();

        if (m_checkLevel == 0)
            ok = true;
        else if (m_checkLevel == 1) {
            if (data.canConvert<T>()) {
                ok = true;

                if (data.typeId() == QMetaType::QString) {
                    const QString strData = data.toString();

                    if (metaType.id() == QMetaType::Bool)
                        ok = QRegularExpression("^true|false$")
                                 .match(strData.toLower())
                                 .hasMatch();
                    else if (isNumber(metaType))
                        ok = QRegularExpression("^[+-]?[0-9]*\\.?[0-9]*$")
                                 .match(strData)
                                 .hasMatch();
                }
            }
        } else if (m_checkLevel == 2) {
            int numType = isNumber(metaType);
            int dataNumType = isNumber(dataMetaType);
            if (metaType == dataMetaType || (numType == 3 && dataNumType) ||
                (numType == 2 && dataNumType <= 2) ||
                (numType == 1 && dataNumType <= 2 && data.value<int>() >= 0))
                ok = true;
        }

        if (ok)
            return data.value<T>();
        else {
            QString mess = QString(
                               "Mismatched parameter types; actual type is %1, "
                               "expected type is %2")
                               .arg(data.metaType().name())
                               .arg(QMetaType::fromType<T>().name());
            throw std::invalid_argument(mess.toStdString());
        }
    }
    ARG V(QVariant value) {
        m_hasDefaultValue = true;
        m_defaultValue = value;
        return *this;
    }
    ARG CH(unsigned short checkLevel) {
        m_checkLevel = checkLevel;
        return *this;
    }

    QString paramDecl(int index) const {
        QString decl;
        QMetaType metaType = QMetaType::fromType<T>();
        decl += metaType.name();
        decl += " ";
        decl += m_name.isEmpty() ? "arg" + QString::number(index) : m_name;

        if (m_hasDefaultValue) {
            switch (metaType.id()) {
            case QMetaType::QString:
                decl += QString("=\"%1\"").arg(m_defaultValue.toString());
                break;
            case QMetaType::QVariantList:
                decl += "=[...]";
                break;
            case QMetaType::QVariantMap:
                decl += "={...}";
                break;
            default:
                decl += QString("=%1").arg(m_defaultValue.toDouble());
                break;
            }
        }
        return decl;
    }

    QVariant outOfRange() const {
        if (m_hasDefaultValue)
            return m_defaultValue;
        else
            throw std::out_of_range(
                "Too few actual arguments for the given formal parameters.");
    }
    using ArgType = T;

public:
    unsigned short m_checkLevel = UNCHECK;
    QVariant m_defaultValue;
    bool m_hasDefaultValue = false;
    QString m_name;
};

template <typename End>
std::tuple<typename End::ArgType>
unpackArgs(const QVariantList& datas, int index, const End& end) {
    QVariant data = index < datas.size() ? datas[index] : end.outOfRange();
    // if (index < datas.size()) data = datas[index];

    return std::make_tuple(end.to(data));
}

template <typename First, typename... Rest>
std::tuple<typename First::ArgType, typename Rest::ArgType...>
unpackArgs(const QVariantList& datas, int index, const First& first,
           const Rest&... rest) {
    QVariant data = index < datas.size() ? datas[index] : first.outOfRange();

    return std::tuple_cat(std::make_tuple(first.to(data)),
                          unpackArgs(datas, index + 1, rest...));
}

/*std::tuple<>
unpackArgs(const QVariantList&, int) {
    return std::tuple<>();
}*/

template <typename T, typename... Args, typename... Rest>
std::function<QVariant(QPointer<Caller>, const QVariantList)>
AdapIF(T* obj, QVariant (T::*funPtr)(Args...), const Rest&... rest) {
    return [=](QPointer<Caller> caller, const QVariantList& args) -> QVariant {
        std::tuple tpl = std::tuple_cat(std::make_tuple(caller),
                                        unpackArgs(args, 0, rest...));
        return std::apply(
            [obj, funPtr](auto... args) -> QVariant {
                return (obj->*funPtr)(args...);
            },
            tpl);
    };
}

template <typename T>
std::function<QVariant(QPointer<Caller>, const QVariantList)>
AdapIF(T* obj, QVariant (T::*funPtr)(QPointer<Caller>)) {
    return [=](QPointer<Caller> caller, const QVariantList& args) -> QVariant {
        return (obj->*funPtr)(caller);
    };
}

template <typename End>
void
genParamInfo(QStringList& params, const End& end) {
    params.append(end.paramDecl(params.length()));
}
template <typename First, typename... Rest>
void
genParamInfo(QStringList& params, const First first, Rest... rest) {
    params.append(first.paramDecl(params.length()));
    genParamInfo(params, rest...);
}
/*void
genParamInfo(QStringList& param) {}*/

};  // namespace NS_BGMRPCObjectInterface

#endif  // MTHDADAPTIF_H
