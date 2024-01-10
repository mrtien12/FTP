#include <iostream>
#include <cstring>
#include <stdio.h>
#include "logic_handle.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>

using namespace std;


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
    cout<<"dmm ok chua\n";
    FILE *fa = fopen("login_details.txt", "a");
    fprintf(fa, "%s %s\n", usename, password);
    fclose(fa);
    mkdir(usename, 0777);
    
    FILE *db = fopen("database.txt", "a");
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    char directory_path[PATH_MAX];
    strcpy(directory_path, path);
    strcat(directory_path, "/");
    strcat(directory_path, usename);
    fprintf(db, "%s %s %s\n",  directory_path,usename,"ACD");
    fclose(db);


    return 0;
}

/**/
int login(char* usename, char* password, int* login_state, char *user){
    if(*login_state)
        return 3;
    FILE *fr = fopen("login_details.txt", "r");
    char line[1024], check_name[30], check_pass[30];
    while (fgets(line, 1024, fr) != NULL)
    {
        sscanf(line, "%s %s", check_name, check_pass);
        if (!strcmp(usename, check_name))
        {   
            cout<<password<<"|"<<check_pass<<"\n";
            if(!strcmp(password, check_pass)){
                fclose(fr);
                *login_state = 1; 
                memset(user, '\0', 30);
                strcpy(user, usename);
                char path[PATH_MAX];
                getcwd(path, PATH_MAX);
                char directory_path[PATH_MAX];
                strcpy(directory_path, path);
                strcat(directory_path, "/");
                strcat(directory_path, usename);
                chdir(directory_path);
                getcwd(path, PATH_MAX);
                // sprintf(,"%s/%s", getcwd(path, PATH_MAX), usename);
                cout<<path<<"\n";
                return 0;
            }
            fclose(fr);
            return 1;
        }
    }
    fclose(fr);


    return 2;
}


int list(char *usename, int* login_state, int connfd){
    
    if (!*login_state) {
        return 0;
    } else {
        FILE *fp;
        char path[4096];

        // Creating the command to execute "ls" in the user's directory
        snprintf(path, sizeof(path), "ls", usename);

        // Open the process to execute the command
        fp = popen(path, "r");
        if (fp == NULL) {
            perror("Error opening pipe for ls command");
            return -1;  // You can handle this error appropriately
        }

        char buffer[1024];
        size_t bytesRead;

        // Read the output of the command and send it to the client
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            if (send(connfd, buffer, bytesRead, 0) < 0) {
                perror("Error sending ls output to client");
                pclose(fp);
                return -1;  // You can handle this error appropriately
            }
        }

        // Close the process
        pclose(fp);

        return 1;
    }
}
