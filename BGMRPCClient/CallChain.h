#ifndef CALLCHAIN_H
#define CALLCHAIN_H

#include <QObject>
#include <QWebSocket>
#include <functional>

class CallChain {
public:
    CallChain(CallChain* begin = nullptr);
    CallChain(std::function<void(CallChain*)> f);
    ~CallChain();
    //    CallChain* begin(void (*f)(CallChain*));
    CallChain* then(std::function<void(CallChain*, const QVariant&)> resolve,
                    std::function<void(CallChain*, const QVariant&)> reject);
    void exec();

    void resolve(const QVariant& data);
    void reject(const QVariant& data);
    void final(std::function<void()> f);

protected:
    void free();

protected:
    CallChain* m_begin;
    CallChain* m_next;

    /*void (*m_exec)(CallChain*);
    void (*m_resolve)(CallChain*, const QByteArray&);
    void (*m_reject)(CallChain*, const QByteArray&);*/
    std::function<void(CallChain*)> m_exec;
    std::function<void(CallChain*, const QVariant&)> m_resolve;
    std::function<void(CallChain*, const QVariant&)> m_reject;
};

#endif // CALLCHAIN_H
