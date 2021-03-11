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

#include <android/log.h>
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "CubeSocket", __VA_ARGS__)

int main(int argc, char **argv)
{
    printf("test socket \n");
	char* address = "192.168.5.111";
	int port = 2000;
	struct sockaddr_in serverAddr;
	int sock = -1;

	if (argc >= 2) {
		address = argv[0];
		port = ::atoi(argv[1]);
		printf("use custom socket %s %s \n", argv[0], address);
		printf("use custom socket %s %d \n", argv[1], port);
	}

	printf("client connect start ... \n");
	if(sock == -1)
	{
		sock = socket(AF_INET , SOCK_STREAM , 0);
		if (sock == -1)
		{
			printf("Could not create socket \n");
		}
		printf( "socket %d \n", sock);
		memset(&serverAddr, 0, sizeof(serverAddr));         // Zero out structure
		serverAddr.sin_family      = AF_INET;               // Internet address family
		serverAddr.sin_addr.s_addr = inet_addr(address);    // Server IP address
		serverAddr.sin_port        = htons(port);     		// Server port

		printf( "socket connect start \n");
		if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
			printf("connect() failed \n");
			return 0;
		}
		else {
			printf("connect ok \n");
		}		 	
	}	

	char* data = "hello server 2222!";
	int len = strlen(data);
	if (send(sock, data, len, 0) != len)
	{
		printf("send() failed \n");
		return false;
	}
    return 0;
}
