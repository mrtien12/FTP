#include <iostream>
#include <cstring>
#include <stdio.h>
#include "logic_handle.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#define BUFF_SIZE 1024
using namespace std;
void send_file(int client_sock, char *filepath)
{
    printf("Sending\n");
    FILE *fp = fopen(filepath, "rb");
    if (fp != NULL)
    {
        char send_buff[BUFF_SIZE];
        while (1)
        {
            int read_bytes = fread(send_buff, 1, sizeof(send_buff), fp);
            printf("Sending\n");
            if (read_bytes == 0)
            {
                break;
            }
            int sent = send(client_sock, send_buff, sizeof(send_buff), 0);
            if (sent < 0)
            {
                perror("\nError: ");
            }
        }
    }
    fclose(fp);
}
int signup(char *usename, char *password)
{
    FILE *fr = fopen("login_details.txt", "r");
    char line[1024], check_name[30];
    while (fgets(line, 1024, fr) != NULL)
    {
        sscanf(line, "%s", check_name);
        if (!strcmp(usename, check_name))
        {
            return 1;
        }
    }
    fclose(fr);

    FILE *fa = fopen("login_details.txt", "a");
    fprintf(fa, "\n%s %s", usename, password);
    fclose(fa);
    mkdir(usename, 0777);

    FILE *db = fopen("database.txt", "a");
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    char directory_path[PATH_MAX];
    strcpy(directory_path, path);
    strcat(directory_path, "/");
    strcat(directory_path, usename);
    fprintf(db, "%s %s %s\n", directory_path, usename, "ACD");
    fclose(db);

    return 0;
}

/**/
int login(char *usename, char *password, int *login_state, char *user)
{
    if (*login_state)
        return 3;
    printf("hello\n");
    FILE *fr = fopen("login_details.txt", "r");
    printf("hello\n");
    char line[1024], check_name[30], check_pass[30];
    memset(line, '\0', 1024);
    memset(check_name, '\0', 30);
    memset(check_name, '\0', 30);
    while (fgets(line, 1024, fr) != NULL)
    {
        sscanf(line, "%s %s", check_name, check_pass);
        if (!strcmp(usename, check_name))
        {
            cout << password << "|" << check_pass << "\n";
            if (!strcmp(password, check_pass))
            {
                fclose(fr);
                char cmd[1000];
                *login_state = 1;
                memset(user, '\0', 30);
                strcpy(user, usename);
                char path[PATH_MAX];
                getcwd(path, PATH_MAX);
                char directory_path[PATH_MAX];
                strcpy(directory_path, path);
                strcat(directory_path, "/");
                strcat(directory_path, usename);
                sprintf(cmd, "cd %s\0", usename);
                // strcat(cmd,directory_path);
                getcwd(path, PATH_MAX);
                system(cmd);
                // sprintf(cmd,"%s/%s", getcwd(path, PATH_MAX), usename);
                cout << cmd << "\n";
                return 0;
            }
            fclose(fr);
            return 1;
        }
    }
    fclose(fr);
    return 2;
}

int list(char *usename, int *login_state, int connfd)
{

    if (!*login_state)
    {
        return 0;
    }
    else
    {
        FILE *fp;
        char path[4096];

        // Creating the command to execute "ls" in the user's directory
        snprintf(path, sizeof(path), "ls", usename);

        // Open the process to execute the command
        fp = popen(path, "r");
        if (fp == NULL)
        {
            perror("Error opening pipe for ls command");
            return -1; // You can handle this error appropriately
        }

        char buffer[1024];
        size_t bytesRead;

        // Read the output of the command and send it to the client
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0)
        {
            if (send(connfd, buffer, bytesRead, 0) < 0)
            {
                perror("Error sending ls output to client");
                pclose(fp);
                return -1; // You can handle this error appropriately
            }
        }

        // Close the process
        pclose(fp);

        return 1;
    }
}

int store(int conn_sock, char *filepath, int filesize)
{
    FILE *fp = fopen(filepath, "wb");
    if (fp == NULL)
    {
        perror("\nError");
    }
    else
    {
        char recv_buff[1024];
        unsigned long received_bytes = 0;
        while (received_bytes < filesize)
        {
            if (filesize - received_bytes > 1024)
            {
                int ret = recv(conn_sock, recv_buff, 1024, 0);
                if (ret < 0)
                {
                    perror("\nLost data:");
                    return 1;
                }
                received_bytes += ret;
                fwrite(recv_buff, 1, ret, fp);
            }
            else
            {
                int ret = recv(conn_sock, recv_buff, filesize - received_bytes, 0);
                if (ret < 0)
                {
                    perror("\nLost data:");
                    return 1;
                }
                received_bytes += ret;
                fwrite(recv_buff, 1, ret, fp);
            }
        }
    }
    fclose(fp);
    return 0;
}

int deletefile(char *usename, char *filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        char cmd[1000];
        sprintf(cmd, "rm -r %s", filename);
        int ret = system(cmd);
        if (ret)
        {
            return 1;
        }
        else
            return 2;
    }
}

int renamefile(char *usename, char *filename, char *new_filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        char cmd[1000];
        sprintf(cmd, "mv %s %s", filename, new_filename);
        int ret = system(cmd);
        if (ret)
        {
            return 1;
        }
        else
            return 2;
    }
}

int mkadir(char *usename, char *filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        char cmd[1000];
        sprintf(cmd, "mkdir %s", filename);
        int ret = system(cmd);
        if (ret)
        {
            return 1;
        }
        else
            return 2;
    }
}

int copy(char *usename, char *filename, char *new_filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        char cmd[1000];
        sprintf(cmd, "cp %s %s", filename, new_filename);
        int ret = system(cmd);
        if (ret)
        {
            return 1;
        }
        else
            return 2;
    }
}

int move(char *usename, char *filename, char *new_filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        char cmd[1000];
        sprintf(cmd, "mv %s %s", filename, new_filename);
        int ret = system(cmd);
        if (ret)
        {
            return 1;
        }
        else
            return 2;
    }
}

int download(char *usename, char *filename, int *login_state, int connfd)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        FILE *fp = fopen(filename, "rb");
        if (fp == NULL)
        {
            perror("\nError");
            return 1;
        }
        int sent_bytes;
        char buff[1024];
        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        sprintf(buff, "RETR %s %ld", basename(filename), size);
        sent_bytes = send(connfd, buff, sizeof(buff), 0);
        send_file(connfd, filename);
        return 2;
    }
}

int quit(int *login_state)
{
    if (!*login_state)
    {
        return 0;
    }
    else
    {
        *login_state = 0;
        return 1;
    }
}
