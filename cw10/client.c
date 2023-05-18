#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "com.h"
#define MAX_NAME_LEN 20
int sock;
int my_id;

void catcher(){
    while (1)
    {
        struct Message response;
        memset(&response, 0, sizeof(struct Message));
        if (read(sock, &response, sizeof(struct Message)) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        switch (response.command_type)
        {
        case _2ALL:
            printf("%s\n", response.msg);
            break;
        case _2ONE:
            printf("%s\n", response.msg);
            break;
        case LIST:
            printf("%s\n", response.msg);
            break;
        case STOP:
            printf("STOP\n");
            //kill parent
            kill(getppid(), SIGINT);
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }
    }
    
}

int main(int argc, char *argv[]) {
    char name[MAX_NAME_LEN];
    int is_network;
    char *server_addr;
    int server_port;
    char *unix_socket_path;
    struct sockaddr_in addr;
    struct sockaddr_un unix_addr;
    char buf[1024];

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <name> <network/local> <server_addr> <server_port/unix_socket_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strncpy(name, argv[1], MAX_NAME_LEN - 1);
    is_network = strcmp(argv[2], "network") == 0;
    server_addr = argv[3];

    if (is_network) {
        char *port_str = strtok(server_addr, ":");
        char *addr_str = strtok(NULL, ":");
        server_port = atoi(port_str);

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(server_port);
        if (inet_pton(AF_INET, addr_str, &addr.sin_addr) != 1) {
            fprintf(stderr, "Invalid address: %s\n", addr_str);
            exit(EXIT_FAILURE);
        }

        if (connect(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1) {
            perror("connect");
            exit(EXIT_FAILURE);
        }
    } else {
        unix_socket_path = server_addr;

        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        memset(&unix_addr, 0, sizeof(struct sockaddr_un));
        unix_addr.sun_family = AF_UNIX;
        strncpy(unix_addr.sun_path, unix_socket_path, sizeof(unix_addr.sun_path) - 1);

        if (connect(sock, (struct sockaddr *) &unix_addr, sizeof(struct sockaddr_un)) == -1) {
            perror("connect");
            exit(EXIT_FAILURE);
        }
    }

    struct Message request;
    request.client_id = -1;
    request.command_type = INIT;
    strncpy(request.username, name, sizeof(request.username) - 1);
    snprintf(request.msg ,sizeof(request.msg), "%s has joined the chat\n", name);
    if (write(sock, &request, sizeof(struct Message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    struct Message response_id;
    if (read(sock, &response_id, sizeof(struct Message)) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    printf("Your id is %d\n", response_id.client_id);
    my_id = response_id.client_id;

    void sigint_handler(int sig){
        request.command_type = STOP;
        request.client_id = -1;

        snprintf(request.msg ,sizeof(request.msg), "%s has left the chat\n", name);
        if (write(sock, &request, sizeof(struct Message)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        close(sock);
        exit(EXIT_SUCCESS);
    }

    pid_t pid = fork();
    if(pid == 0){
        catcher();
    }
    else if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);

    char input[MAX_MESSAGE_LEN];
    char msg[MAX_MESSAGE_LEN];
    while (1) {
        memset(&request, 0, sizeof(struct Message));
        fgets(input, MAX_MESSAGE_LEN, stdin);
        if(strncmp(input,"LIST",4)==0)
        {   
            
            request.command_type = LIST;
            request.client_id = my_id;
            snprintf(request.msg ,sizeof(request.msg), "LIST\n");
            
            
            if (write(sock, &request, sizeof(struct Message)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
         else if (strncmp(input, "2ONE", 4) == 0){
            request.command_type = _2ONE;
            request.client_id = my_id;

            sscanf(input, "%*s %d %[^\n]", &request.idTo, request.msg);
            if(write(sock, &request, sizeof(struct Message)) == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }
         }
        else if (strncmp(input, "2ALL", 4) == 0){
            request.command_type = _2ALL;
                        request.client_id = my_id;

            sscanf(input, "%*s %[^\n]", request.msg);
            if(write(sock, &request, sizeof(struct Message)) == -1){
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
         else if (strncmp(input, "STOP", 4) == 0){
            request.command_type = STOP;
                        request.client_id = my_id;

            snprintf(request.msg ,sizeof(request.msg), "STOP\n");
            if (write(sock, &request, sizeof(struct Message)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
         }
         else{
            printf("Wrong command\n");
         }
    }

    snprintf(buf, sizeof(buf), "%s has left the chat\n", name);
    if (write(sock, buf, strlen(buf)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(sock);

    return 0;
}
