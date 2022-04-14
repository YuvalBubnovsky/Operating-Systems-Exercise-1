#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#define PORT 6090
#define ADDRESS "127.0.0.1"
#define MTU 1500

int main()
{
    // Setting up variables for later use
    int conn_status;
    int sock, sock_recv;
    struct sockaddr_in server_addr;

    char buffer[MTU];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
    }

    // Setting server_addr memory to 0 in order to make sure we open a fresh port without overlapping
    // any existing data
    memset(&server_addr, 0, sizeof(server_addr));

    // Setting up socket data
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    conn_status = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (conn_status < 0)
    {
        perror("bind");
        printf("\n");
        close(sock);
        exit(1);
    }

    int e;
    printf("Ready To Serve!\n");
    e = listen(sock, 10);
    if (e < 0)
    {
        perror("listen");
    }
    sock_recv = accept(sock, NULL, NULL);
    if (sock_recv < 0)
    {
        perror("accept");
        exit(1);
    }

    int n;
    while (n = recv(sock_recv, &buffer, sizeof(buffer), 0) > 0)
    {
        if (n == -1)
        {
            perror("recv");
        }
        if (strncmp("dir", buffer, 3) == 0)
        {
            DIR *dir_p = opendir("."); // returns NULL on error
            if (dir_p == NULL)
            {
                printf("Error opening directory pointer\n");
            }
            else
            {
                struct dirent *file_p;
                while ((file_p = readdir(dir_p)) != NULL)
                {
                    printf("%s | ", file_p->d_name);
                }
                printf("\n");
                closedir(dir_p);
            }
        }
        printf("%s\n", buffer);
        memset(buffer, '\0', MTU); // clean buffer
    }

    bzero(buffer, MTU);
    close(sock_recv);

    close(sock);
    return 0;
}