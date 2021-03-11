#include <sys/time.h>
#include <pthread.h>
#include "Condition.h"
class ConditionPrivate
{
public:
    ConditionPrivate()
        : m_mutex(PTHREAD_MUTEX_INITIALIZER)
        , m_cond(PTHREAD_COND_INITIALIZER)
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_condattr_init(&m_condattr);
        pthread_condattr_setclock(&m_condattr, CLOCK_MONOTONIC);
        pthread_cond_init(&m_cond, &m_condattr);
    }

    ~ConditionPrivate()
    {
        pthread_cond_destroy(&m_cond);
        pthread_mutex_destroy(&m_mutex);
        pthread_condattr_destroy(&m_condattr);
    }

    pthread_mutex_t    m_mutex;
    pthread_cond_t     m_cond;
    pthread_condattr_t m_condattr;
    DISABLE_COPY(ConditionPrivate)
};


Condition::Condition()
    : m_data(new ConditionPrivate)
{

}

Condition::~Condition()
{
    if (m_data) {
        delete m_data;
        m_data = NULL;
    }
}

void Condition::signal()
{
    pthread_cond_signal(&m_data->m_cond);
}

void Condition::broadcast()
{
    pthread_cond_broadcast(&m_data->m_cond);
}

void Condition::lock()
{
    pthread_mutex_lock(&m_data->m_mutex);
}

void Condition::unlock()
{
    pthread_mutex_unlock(&m_data->m_mutex);
}

bool Condition::wait()
{
    return 0 == pthread_cond_wait(&m_data->m_cond, &m_data->m_mutex);
}

bool Condition::timedWait(int timeoutMs)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    ts.tv_sec  += timeoutMs/1000;
    ts.tv_nsec += (timeoutMs%1000)*1000*1000;
    if (ts.tv_nsec >= 1000*1000*1000) {
        ts.tv_nsec -= 1000*1000*1000;
        ts.tv_sec  += 1;
    }

    return 0 == pthread_cond_timedwait(&m_data->m_cond, &m_data->m_mutex, &ts);
}
