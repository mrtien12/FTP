#ifndef LOGIC_H
#define LOGIC_H

int signup(char* usename, char* password);
int login(char* usename, char* password, int* login_state, char* user);
int list(char *usename, int* login_state, int connfd);
int store(int conn_sock, char *filepath, int filesize);

#endif