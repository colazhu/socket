#include "SocketClient.h"

#define MAX_BUFFER (1024 * 64)

SocketClient::SocketClient(const std::string& name, const std::string& ip, int port) :
    SocketThread(name),
    m_positive(true),
    m_ip(ip),
    m_port(port),
    m_clientsockfd(-1),
    m_cb(0)
{
    start();
}

SocketClient::SocketClient(int clientid) :
    m_positive(false),
    m_ip(""),
    m_port(-1),
    m_clientsockfd(clientid),
    m_cb(0)
{
    char name[32] = {0};
    snprintf(name, 32, "SvrClient_%d", clientid);
    m_name = name;
}

SocketClient::~SocketClient()
{
    closeSocket();
    stop();
}

void SocketClient::onRun()
{
    if (!connectSocket()) {
        closeSocket();
    }

    while (!checkQuit()) {
        char buffer[MAX_BUFFER] = {0};
        int size  = read(m_clientsockfd, buffer, MAX_BUFFER);
        if (size > 0)        {
            LOG_BASE( "[%s]socket:%d :%p read:%s", m_name.c_str(), size, m_cb, buffer);
            if (m_cb) {
                std::string data(buffer, size);
                m_cb->onReceiveData(data, m_clientsockfd);
            }
        }
        else {
            break;
        }
    }

    closeSocket();
}

void SocketClient::sendData(const std::string& data)
{
    if (!isConnected()) {
        return;
    }
    CondAutoLock _autocon(m_condition);
    size_t len = data.size();
    if (m_positive) {
        // LOG_BASE("[%s]send:%s", m_name.c_str(), data.c_str());
        int sent = send(m_clientsockfd, data.data(), len, 0);
        if (sent == len) {
            // LOG_BASE("send success:%d -> %d", len, sent);
        }
        else {
            LOG_BASE("send failed :%d -> %d",len, sent);
            // closeSocket();
        }
    }
    else {
        int written = write(m_clientsockfd, data.data(), len);
        // LOG_BASE("[%s]write:%s", m_name.c_str(), data.c_str());
        if (written > 0) {
            // LOG_BASE("write success:%d -> %d", len, written);
        }
        else {
            LOG_BASE("write failed :%d -> %d",len, written);
            // closeSocket();
        }
    }
}

bool SocketClient::connectSocket()
{
    if(!m_positive) {
        return true;
    }

    LOG_BASE_TRACE("connectSocket:%s", m_name.c_str());
    m_clientsockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (m_clientsockfd == -1) {
        LOG_BASE("could not create socket");
        return false;
    }

    LOG_BASE( "socket create sucess: id %d ", m_clientsockfd);

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));             // Zero out structure
    serverAddr.sin_family      = AF_INET;                   // Internet address family
    serverAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());   // Server IP address
    serverAddr.sin_port        = htons(m_port);             // Server port

    LOG_BASE( "socket connect start : %s : %d", m_ip.c_str(), m_port);
    if (connect(m_clientsockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        LOG_BASE("connect() failed ");
        return false;
    }
    return true;
}

void SocketClient::closeSocket()
{
    LOG_BASE( "closeSocket:%d", m_clientsockfd);
    if (m_clientsockfd != -1) {
        close(m_clientsockfd);
        m_clientsockfd = -1;
        if (m_cb) {
            m_cb->onDisconnected(m_port, m_clientsockfd);
        }
    }
}

