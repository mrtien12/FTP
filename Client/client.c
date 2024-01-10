/*TCP Echo Client*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024

void send_msg(int sockfd, const char *msg)
{
    size_t msg_len = strlen(msg);
    ssize_t sent_bytes = send(sockfd, msg, msg_len, 0);
    if (sent_bytes < 0)
    {
        perror("\nError: ");
        exit(EXIT_FAILURE);
    }
}

void send_file(int client_sock, char *filepath)
{
    FILE *fp = fopen(filepath, "rb");
    if (fp != NULL)
    {
        char send_buff[BUFF_SIZE];
        while (1)
        {
            int read_bytes = fread(send_buff, 1, sizeof(send_buff), fp);
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <server_ip> <port_number>\n", argv[0]);
        return 0;
    }

    int port_num = atoi(argv[2]);
    if (port_num == 0)
    {
        printf("Wrong port number\n");
        return 0;
    }

    char *ip_adress = argv[1];
    if (inet_addr(ip_adress) == -1)
    {
        printf("Wrong ip_address\n");
        return 0;
    }

    int client_sock;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in server_addr; /* server's address information */
    int msg_len, sent_bytes, received_bytes;
    // Step 1: Construct socket
    if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("\nError: ");
        exit(EXIT_FAILURE);
    }
    // Step 2: Specify server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    server_addr.sin_addr.s_addr = inet_addr(ip_adress);
    // Step 3: Request to connect server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        perror("\nError: ");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Send row 2
        printf("\nEnter string to send: ");
        memset(buff, '\0', 1024);
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);

        if (strncmp(buff, "STOR", 4) == 0)
        {
            // STOR command handling
            char filename[100];
            sscanf(buff, "STOR %s", filename);
            // Open the file for reading
            FILE *file = fopen(filename, "rb");
            if (!file)
            {
                printf("Failed to open file: %s\n", filename);
                continue; // Continue to the next iteration
            }

            // Get the file size
            fseek(file, 0, SEEK_END);
            int size = ftell(file);

            sprintf(buff, "STOR %s %ld", basename(filename), size);
            fclose(file);
            sent_bytes = send(client_sock, buff, strlen(buff), 0);
            send_file(client_sock, filename);
        }
        else
        {
            sent_bytes = send(client_sock, buff, strlen(buff), 0);
            if (sent_bytes < 0)
                perror("\nError: ");
        }
        // receive echo reply
        received_bytes = recv(client_sock, buff, BUFF_SIZE, 0);
        if (received_bytes < 0)
            perror("\nError: ");
        else if (received_bytes == 0)
        {
            printf("Connection closed.\n");
            break;
        }
        else
        {

            buff[received_bytes] = '\0';
            printf("Reply from server: %s", buff);
        }
    }
    // Step 4: Close socket
    close(client_sock);
    return 0;
}
