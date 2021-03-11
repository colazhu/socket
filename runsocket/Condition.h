#ifndef CONDITION_H
#define CONDITION_H

#include "Base.h"
class ConditionPrivate;
class Condition
{
public:
    Condition();
    ~Condition();

    void signal();
    void broadcast();

    void lock();
    void unlock();

    bool wait();
    bool timedWait(int timeoutMs);

private:
    ConditionPrivate* m_data;
    pthread_mutex_t    m_mutex;
    pthread_cond_t     m_cond;
    pthread_condattr_t m_condattr;

private:
    DISABLE_COPY(Condition)
};

struct CondAutoLock
{
    CondAutoLock(Condition& cond): m_cond(cond)
    {
        // m_cond.lock();
    }

    ~CondAutoLock()
    {
        // m_cond.unlock();
    }
private:
    Condition& m_cond;
};

#endif // CONDITION_H
