#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include "Base.h"
#include "SocketThread.h"

class SocketCallback
{
public:
    virtual ~SocketCallback() {}
    virtual void onConnected(int port, int clientid) {}
    virtual void onDisconnected(int port, int clientid) {}
    virtual void onReceiveData(const std::string& data, int clientid = 0) {}
};

class SocketClient : public SocketThread
{
public:
    SocketClient(const std::string& name, const std::string& ip, int port);
    SocketClient(int clientid);
    ~SocketClient();

    void setListener(SocketCallback* cb) { m_cb = cb; }
    bool isConnected() {return m_clientsockfd != -1; }
    bool connectSocket();
    void closeSocket();

    void sendData(const std::string& data);

protected:

    virtual void onRun();

    bool m_positive;
    std::string m_ip;
    int m_port;
    int m_clientsockfd;

    SocketCallback* m_cb;
    // std::list<std::string> m_dataRecieved;

};

#endif // SOCKETCLIENT_H
