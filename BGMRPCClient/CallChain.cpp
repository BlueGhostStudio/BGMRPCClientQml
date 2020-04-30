#include "CallChain.h"
#include <QDebug>

CallChain::CallChain(CallChain* begin) {
    if (!begin)
        m_begin = this;
    else
        m_begin = begin;
}

CallChain::CallChain(std::function<void(CallChain*)> f) {
    m_begin = this;
    m_exec = f;
}

CallChain::~CallChain() { qDebug() << "destructor"; }

CallChain*
CallChain::then(std::function<void(CallChain*, const QVariant&)> resolve,
                std::function<void(CallChain*, const QVariant&)> reject) {
    CallChain* _next_ = m_next;
    m_next = new CallChain(m_begin);
    m_next->m_next = _next_;
    m_next->m_resolve = resolve;
    m_next->m_reject = reject;
    return m_next;
}

void CallChain::exec() {
    if (m_begin)
        m_begin->m_exec(m_begin);
}

void CallChain::resolve(const QVariant& data) {
    if (m_next && m_next->m_resolve)
        m_next->m_resolve(m_next, data);
}

void CallChain::reject(const QVariant& data) {
    if (m_next && m_next->m_reject)
        m_next->m_reject(m_next, data);
}

void CallChain::final(std::function<void()> f) {
    if (f)
        f();
    if (m_begin)
        m_begin->free();
}

void CallChain::free() {
    if (m_next)
        m_next->free();
    delete this;
}
