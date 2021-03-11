#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "SocketServer.h"
#if OS_ANDROID
class SocketListener : public SocketCallback
{
public:
    SocketListener(const std::string& name, SocketServer* dest) : m_name(name), m_dest(dest) {}
    virtual ~SocketListener() {}

    // void setDst(SocketServer* dest) { m_dest = dest; }
    virtual void onReceiveData(const std::string& data, int clientid = 0) {
        LOG_BASE("[%s]onRceiveData:%s", m_name.c_str(), data.c_str());
        if (m_dest) {
            m_dest->sendData(data);
        }
    }
    std::string m_name;
    SocketServer* m_dest;
};

int main(int argc, char **argv)
{
    // char* address = "192.168.5.111";

    SocketServer serverImx8("ServerImx8", 2000);
    SocketServer serverCube("ServerCube", 2001);

    serverImx8.setListener(new SocketListener("ServerImx8",  &serverCube));
    serverImx8.start();

    serverCube.setListener(new SocketListener("ServerCube", &serverImx8));
    serverCube.start();

    while (true) {
        if (!serverCube.isConnected() || !serverImx8.isConnected()) {
            system("ifconfig eth0 192.168.5.112 \n");
        }
        sleep(1);
    }

    return 0;
}
#else

class SocketListener : public SocketCallback
{
public:
    SocketListener(const std::string& name) : m_name(name){}
    virtual ~SocketListener() {}

    // void setDst(SocketServer* dest) { m_dest = dest; }
    virtual void onReceiveData(const std::string& data, int clientid = 0) {
        LOG_BASE("[%s]onRceiveData:%s", m_name.c_str(), data.c_str());
    }

    std::string m_name;
};

int main(int argc, char **argv)
{
    char* address = "192.168.5.112";
    SocketClient client("PCClient", address, 2000);
    client.setListener(new SocketListener("PCClient"));
    client.start();
    while (true) {
        if (!client.isConnected()) {
            // system("sudo ifconfig eth0 192.168.5.113 \n");
        }
        sleep(1);
        client.sendData("Ping");
    }
    return 0;
}

#endif
