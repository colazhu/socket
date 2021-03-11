#include "SocketServer.h"
#include "SocketClient.h"
#define CLIENT_CONNECT_MAX 1

//class ConnectedClientThread : public SocketThread
//{
//public:
//    ConnectedClientThread(ServerSocket* server)
//        :m_server(server) {
//        start();
//    }

//    virtual void onRun() {
////        while (checkQuit()) {


//    }

//    ServerSocket* m_server;
//};

SocketServer::SocketServer(const std::string& name, int port):
    SocketThread(name),
    m_port(port),
    m_serversockfd(-1),
    m_cb(0)
{

}

SocketServer::~SocketServer()
{
    closeSocket();
    stop();
}

void SocketServer::onRun()
{
wait_server:    
    while (!checkQuit()) {
        if (startSocket()) {
            break;
        }
        sleep(1);
    }

wait_client:
    int clientId = -1;
    while (!checkQuit()) {
        clientId = acceptClient();
        if (clientId != -1) {
            SocketClient* client = new SocketClient(clientId);
            m_setclientfd[clientId] = client;
            client->setListener(this);
            client->start();
            break;
        }
        else {
            closeSocket();
            goto wait_server;
        }
    }

    while (!checkQuit()) {
        wait();
        for (std::map<int, SocketClient*>::iterator it = m_setclientfd_del.begin(); it != m_setclientfd_del.end(); ) {
            delete it->second;
            it = m_setclientfd_del.erase(it);
        }

        if (m_setclientfd.empty()) {
            goto wait_client;
        }

//        LOG_BASE("Server get data");
//        std::list<std::string> datas;
//        {
//            CondAutoLock _auto(m_condition);
//            datas.swap(m_dataRecieved);
//        }

//        if (m_cb) {
//            for (std::list<std::string>::const_iterator it = datas.begin(); it != datas.end(); ++it) {
//                m_cb->onReceiveData(*it);
//            }
//        }
    }
}

void SocketServer::sendData(const std::string& data)
{
    CondAutoLock _auto(m_condition);
    std::map<int, SocketClient*>::iterator it = m_setclientfd.begin();
    if (it == m_setclientfd.end()) {
        return;
    }

    if (SocketClient* client = it->second) {
        client->sendData(data);
    }
}

bool SocketServer::startSocket()
{
    LOG_BASE_TRACE("startSocket:port:%d", m_port);
    struct sockaddr_in server_addr;
    bool ret = false;
    do {
        if (m_serversockfd != -1) {
            ret = true;
            break;
        }
        if ((m_serversockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            LOG_BASE("Socket Created Failed!");
            break;
        }
        LOG_BASE("Socket Create Success %d!", m_serversockfd);

        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(m_port);
        bzero(&(server_addr.sin_zero), 8);

        int opt = 1;
        int res = setsockopt(m_serversockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (res < 0)
        {
            LOG_BASE("Server reuse setsockopt failed!");
            break;
        }

        if (bind(m_serversockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
        {
            LOG_BASE("Socket Bind Failed!");
            break;
        }
        LOG_BASE("Socket Bind Success! port %d", m_port);

        if (listen(m_serversockfd, CLIENT_CONNECT_MAX) == -1)
        {
            LOG_BASE("Listened Failed!");
        }
        ret = true;
    } while (0);

    if (!ret) {
        closeSocket();
    }

    return ret;
}

int SocketServer::acceptClient()
{
    LOG_BASE_TRACE("accept....");
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_sockfd = -1;

    LOG_BASE("waiting connection...");
    if ((client_sockfd = accept(m_serversockfd, (struct sockaddr *)&client_addr, &len)) == -1)
    {
        LOG_BASE("accepted Failed!");
        return -1;
    }
    LOG_BASE("accepted success:%d", client_sockfd);
    return client_sockfd;
}

void SocketServer::closeSocket()
{
    LOG_BASE_TRACE("closeServer");
    for (std::map<int, SocketClient*>::iterator it = m_setclientfd.begin();  it != m_setclientfd.end(); ) {
        if (SocketClient* client = it->second) {
            LOG_BASE("  closeServerClient:%d %p", it->first, client);
            client->setListener(0);
            client->closeSocket();
        }
        it = m_setclientfd.erase(it);
    }

    LOG_BASE("  closeServer:%d %d", m_port, m_serversockfd);
    if (m_serversockfd) {
        close(m_serversockfd);
        m_serversockfd = -1;
    }
}

void SocketServer::onConnected(int port, int clientid)
{

}

void SocketServer::onDisconnected(int port, int clientid)
{
    m_setclientfd_del.swap(m_setclientfd);
    notify();
}

void SocketServer::onReceiveData(const std::string& data, int clientid)
{
//    {
//        CondAutoLock _autocon(m_condition);
//        LOG_BASE("Server get Data:%s", data.c_str());
//        m_dataRecieved.push_back(data);
//    }
//    notify();

//    LOG_BASE("Server get Data:%s", data.c_str());
    if (m_cb) {
       m_cb->onReceiveData(data, clientid);
    }

//    sendData("Pong");
}
