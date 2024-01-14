#ifndef LOGIC_H
#define LOGIC_H

int signup(char* usename, char* password);
int login(char* usename, char* password, int* login_state, char* user);
int list(char *usename, int* login_state, int connfd);
int store(int conn_sock, char *filepath, int filesize);
int deletefile(char *usename, char *filename, int* login_state,int connfd);
int renamefile(char *usename, char *filename,char* new_filename, int* login_state,int connfd);
int mkadir(char *usename, char *filename, int* login_state,int connfd);
int copy(char *usename, char *filename, char* new_filename, int* login_state,int connfd);
int move(char *usename, char *filename, char* new_filename, int* login_state,int connfd);
int download(char *usename, char *filename, int* login_state,int connfd);
int quit(int *login_state);
#endif