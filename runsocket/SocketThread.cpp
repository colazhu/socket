#include "SocketThread.h"
#include <pthread.h>

static void* threadRoutine(void* pArguments)
{
    if (SocketThread *thread = reinterpret_cast<SocketThread*>(pArguments)) {
        thread->run();
    }
    return 0;
}


SocketThread::SocketThread(const std::string& name):
    m_tid(0),
    m_name(name),
    m_quit(false),
    m_start(false)
{

}

SocketThread::~SocketThread()
{

}

bool SocketThread::start()
{    
    if (m_start) {
        return true;
    }

    int ret = pthread_create(reinterpret_cast<pthread_t*>(&m_tid), NULL, threadRoutine, this);
    if (0 == ret) {
        m_start = true;
        m_quit = false;
        return true;
    }
    return false;
}

void SocketThread::stop()
{
    m_quit = true;
    notify();
    if ((unsigned long)pthread_self() != m_tid) {
        pthread_join((pthread_t)m_tid, NULL);
    }
}

void SocketThread::wait(int ms)
{
    if (ms < 0 ) {
        m_condition.wait();
    }
    else {
        m_condition.timedWait(ms);
    }
}

void SocketThread::notify()
{
    m_condition.signal();
}

void SocketThread::run()
{
    LOG_BASE_TRACE("======== onRun(%s %p) ========", m_name.c_str(), this);
    onRun();
    m_start = false;
}
