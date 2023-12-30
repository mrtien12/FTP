#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "request.h"

using namespace std;

#define BACKLOG 20
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{   
     if (argc != 2)
    {
        cout<<"Usage<:"<<argv[0]<< "[port_number]";
        return 0;
    }
    int port = atoi(argv[1]);

    int listenfd, *connfd;
    struct sockaddr_in server;
    struct sockaddr_in *client;
    int sin_size;
    pthread_t tid;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\nError1: ");
        return 0;
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("\nError2: ");
        return 0;
    }

    if (listen(listenfd, BACKLOG) == -1)
    {
        perror("\nError3: ");
        return 0;
    }

    sin_size = sizeof(struct sockaddr_in);
    client = (sockaddr_in *)malloc(sin_size);
    while (1)
    {
        connfd = (int *)malloc(sizeof(int));
        if ((*connfd = accept(listenfd, (struct sockaddr *)client, (socklen_t*)(&sin_size))) == -1)
        {
            perror("\nError4");
        }

        cout<<"You got connection from "<< inet_ntoa(client->sin_addr)<<":"<< *connfd<<"\n";
        pthread_create(&tid, NULL, &request_handle, connfd);
    }
    close(listenfd);
    return 0;
}