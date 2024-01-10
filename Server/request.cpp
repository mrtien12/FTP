#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "request.h"
#include "logic_handle.h"

using namespace std;

#define BUFF_SIZE 1024

int command_handle(char *request, int connfd, int *login_state, char *user);
int send_msg(int connfd, char *msg);

void *request_handle(void *conn_sock)
{
    int login_state = 0;
    char *user;
    user = (char *)malloc(30);
    int connfd = *reinterpret_cast<int *>(conn_sock);
    int sent_bytes, received_bytes;
    char buff[BUFF_SIZE + 1];
    free(conn_sock); // Assuming conn_sock was allocated using malloc
    pthread_detach(pthread_self());
    std::cout << connfd << std::endl;

    while (1)
    {
        received_bytes = recv(connfd, buff, BUFF_SIZE, 0);
        if (received_bytes < 0)
            perror("\nError5:");
        else if (received_bytes == 0)
        {
            std::cout << "Connection closed" << std::endl;
            break;
        }
        else
        {
            buff[received_bytes] = '\0';
            if (command_handle(buff, connfd, &login_state, user) <= 0)
            {
                break;
            }
        }
    }
    close(connfd);
}

int command_handle(char *request, int connfd, int *login_state, char *user)
{
    if (*login_state)
        cout << "Command received from user " << connfd << ": " << request;
    else
        cout << "Command received from user: " << request;

    cout << "Processing......\n";

    char cmd[10];
    sscanf(request, "%s", cmd);

    if (strcmp("QUIT", cmd) == 0)
    {
        if (*login_state)
            cout << connfd << " has off\n";
        else
            cout << "User has quit\n";
        close(connfd);
        exit(0);
    }
    else if (strcmp("ABORT", cmd) == 0)
    {
        // cout<<"\nComing in abort";
        // USERNAME = 0;
        // AUTHORIZED = 0;
        // send(connfd, "200 Command Okay\nSuccessfully logged out!\n", MAXLINE, 0);
        return send_msg(connfd, "200 Command Okay\nSuccessfully logged out!\n");
    }

    else if (strcmp("SIGN", cmd) == 0)
    {
        char usename[30], password[30];
        if (sscanf(request, "SIGN %s %s", usename, password) < 2)
        {
            return send_msg(connfd, "Not enough params\n");
        }
        else
        {
            if (signup(usename, password))
            {
                return send_msg(connfd, "Account already exist\n");
            }
            return send_msg(connfd, "200, Command Okay\n");
        }
    }
    else if (strcmp("USER", cmd) == 0)
    {
        char usename[30], password[30];
        if (sscanf(request, "USER %s %s", usename, password) < 2)
        {
            return send_msg(connfd, "Not enough params\n");
        }
        else
        {
            switch (login(usename, password, login_state, user))
            {
            case 0:
                return send_msg(connfd, "Login success\n");
            case 1:
                return send_msg(connfd, "Password wrong\n");
            case 2:
                return send_msg(connfd, "Account not found\n");
            case 3:
                return send_msg(connfd, "Already login\n");
            }
        }
    }
    else if (strcmp("LIST", cmd) == 0)
    {
        
        switch(list(user, login_state,connfd)){
            case 0: 
                return send_msg(connfd, "Not login\n");
            case 1:
                return send_msg(connfd, "200, Command Okay\n");
        }
        
    }

    else if (strcmp("PWD\n", request) == 0)
    {
        char curr_dir[BUFF_SIZE];
        char response[BUFF_SIZE] = "200, Command Okay\n";
        getcwd(curr_dir, BUFF_SIZE - 20);
        strcat(response, curr_dir);
        return send_msg(connfd, response);
    }

    else if (strcmp("CWD", cmd) == 0)
    {
        char dir[100];
        sscanf(request, "CWD %s", dir);
        if (chdir(dir) < 0)
        {
            return send_msg(connfd, "0");
        }
        else
        {
            return send_msg(connfd, "200, Command Okay\n Successfully updated server current directory.\n");
        }
    }

    else if (strcmp("MKD", cmd) == 0)
    {
        char dir[100];
        sscanf(request, "MKD %s", dir);
        strcat(cmd, "mkdir -p ");
        strcat(cmd, dir);
        // cout<<"command is  :"<<cmd<<endl;
        int i = system(cmd);
        // cout<<"i value is:  "<<i<<endl;
        if (i)
            return send_msg(connfd, "\n550, Failed to recreate directory\n");
        else
            return send_msg(connfd, "\n250, Directory successfully created\n");
    }

    else if (strcmp("RMD", cmd) == 0)
    {
        char dir[100];
        sscanf(request, "RMD %s", dir);
        cout << dir << endl;
        strcat(cmd, "rm -r ");
        strcat(cmd, dir);
        // cout<<"command is  :"<<cmd<<endl;
        int i = system(cmd);
        // cout<<"i value is:  "<<i<<endl;
        if (i)
            return send_msg(connfd, "\n550, Failed to remove directory\n");
        else
            return send_msg(connfd, "\n250, Directory successfully removed\n");
    }

    else if (strcmp("STOR", cmd) == 0)
    {
        
                // Receive the filename from the client
            char filename[1000];
            recv(connfd, filename, sizeof(filename),0);
            // Receive the file size from the client
            int size;
            recv(connfd, &size, sizeof(int), 0);

            // Open a file on the server for writing
            FILE *file = fopen(filename, "wb");
            printf(filename+'\n');
            if (!file)
            {
                send_msg(connfd, "550 Failed to create file on server\n");
                return 0;
            }

            // Receive and write the file content in chunks
            char buffer[BUFF_SIZE];
            size_t totalReceived = 0;
            while (totalReceived < size)
            {
                ssize_t bytesRead = recv(connfd, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0)
                {
                    perror("Error receiving file\n");
                    fclose(file);
                    return 0;
                }

                fwrite(buffer, 1, bytesRead, file);
                totalReceived += bytesRead;
            }

            fclose(file);
            send_msg(connfd, "200 Command Okay\nFile received successfully\n");
                
            
    }

    else if (strcmp("RETR", cmd) == 0)
    {

        //     filename = strtok(NULL, " \n");
        //     // cout<<"filename : "<<filename<<endl;
        //     stat(filename, &obj);
        //     filehandle = open(filename, O_RDONLY);
        //     // cout<<"file handle is :"<<filehandle<<endl;
        //     if (filehandle == -1)
        //         sprintf(filename + strlen(filename), "%d", i); // needed only if same directory is used for both server and client

        //     else
        //     {
        //         size = obj.st_size;
        //         sprintf(str, "%d", size);
        //         // cout<<str;
        //         send(connfd, str, MAXLINE, 0);
        //         if (size)
        //             sendfile(connfd, filehandle, NULL, size);
        //         close(filehandle);
        //     }
        // }
    }
    else
    {
        // send(connfd, "\n502, Command not implemented", MAXLINE, 0);
        return send_msg(connfd, "\n502, Command not implemented");
    }
    cout << "\nDone with request\n";
}

int send_msg(int sockfd, char *msg)
{
    char buff[BUFF_SIZE];
    strcpy(buff, msg);
    strcat(buff, "\0");
    int sent_bytes = send(sockfd, &buff, strlen(buff), 0);
    if (sent_bytes < 0)
    {
        perror("\nError6:");
        return 0;
    }
    printf("Sent: %s\n", buff);
    return 1;
}