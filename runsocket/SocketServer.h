#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "Base.h"
#include "SocketThread.h"
#include "SocketClient.h"

class SocketClient;
class SocketServer : public SocketThread, public SocketCallback
{
public:
    SocketServer(const std::string& name, int port);
    ~SocketServer();

    void setListener(SocketCallback* cb) { m_cb = cb; }

    bool isConnected() {return m_serversockfd != -1; }
    bool isConnect2Client() { return m_serversockfd != -1 && !m_setclientfd.empty(); }
    bool startSocket();
    void closeSocket();

    void sendData(const std::string& data);

protected:

    virtual void onRun();
    virtual void onConnected(int port, int clientid);
    virtual void onDisconnected(int port, int clientid);
    virtual void onReceiveData(const std::string& data, int clientid = 0);

    int acceptClient();

    int m_port;
    int m_serversockfd;
    std::map<int, SocketClient*> m_setclientfd;
    std::map<int, SocketClient*> m_setclientfd_del;

    SocketCallback* m_cb;
    std::list<std::string> m_dataRecieved;
};

#endif // SOCKETSERVER_H
