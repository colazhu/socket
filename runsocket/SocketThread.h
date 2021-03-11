#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include "Base.h"
#include "Log.h"
#include "Condition.h"

class SocketThread
{
public:
    SocketThread(const std::string& name = "");
    virtual ~SocketThread();

    bool start();
    void stop();
    void run();

    void wait(int ms = -1);
    void notify();

    virtual void onRun() {}
    bool checkQuit() { return m_quit; }
protected:
    Condition m_condition;
    unsigned long m_tid;
    std::string m_name;
    bool m_quit;
    bool m_start;
};

#endif // SOCKETTHREAD_H
