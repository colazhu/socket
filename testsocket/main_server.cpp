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
#define MAX_BUFFER 1024
#define PORT 2000 
static void *startReceive(void* data)
{
    struct sockaddr_in server_addr, client_addr;
    int server_sockfd, client_sockfd;
    int size, write_size;
    char buffer[MAX_BUFFER];

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)    
    {
        perror("Socket Created Failed!\n");
        exit(1);
    }
    printf("Socket Create Success!\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    bzero(&(server_addr.sin_zero), 8);

    int opt = 1;
    int res = setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    
    if (res < 0)
    {
        perror("Server reuse address failed!\n");
        exit(1);
    }

    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)  
    {
        perror("Socket Bind Failed!\n");
        exit(1);
    }
    printf("Socket Bind Success! port %d\n", PORT);

    if (listen(server_sockfd, 5) == -1)                 
    {
        perror("Listened Failed!\n");
        exit(1);
    }
    printf("Listening ....\n");

    socklen_t len = sizeof(client_addr);

    printf("waiting connection...\n");
    if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &len)) == -1)  
    {
        perror("Accepted Failed!\n");
        exit(1);
    }
    printf("connection established!\n");
    printf("waiting message...\n");

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));                             

        if ((size = read(client_sockfd, buffer, MAX_BUFFER)) == -1)    
        {
            perror("Recv Failed!\n");
            exit(1);
        }
 
        if (size != 0)                                               
        {
            buffer[size] = '\0';
            printf("Recv msg from client: %s\n", buffer);
            if ((write_size = write(client_sockfd, buffer, MAX_BUFFER)) > 0)   
            {
                printf("Sent msg to client successfully!\n");
            }

        }
    }

    close(client_sockfd);  
    close(server_sockfd);

    return NULL;
}

int main(int argc, char **argv)
{
    printf("test socketserver \n");
	startReceive(nullptr);
    return 0;
}
