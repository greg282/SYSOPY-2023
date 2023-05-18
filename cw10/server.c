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
#include <sys/epoll.h>
#include <signal.h>
#define MAX_EVENTS 10
#define MAX_CLIENTS 10
#include "com.h"

int num_clients = 0;

int tcp_port, unix_socket;
int tcp_sock, unix_sock;



struct Client{
    int client_socket;
    int client_id;
    char name[512];
};
struct Client client_sockets[MAX_CLIENTS];

void close_all(int sig){
    for (int i = 0; i < num_clients; i++) {
        struct Message stop;
        stop.command_type = STOP;
        stop.client_id = client_sockets[i].client_id;
        if (write(client_sockets[i].client_socket, &stop, sizeof(struct Message)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < num_clients; i++) {
        close(client_sockets[i].client_socket);
    }


    shutdown(unix_socket, SHUT_RDWR);
    shutdown(tcp_sock, SHUT_RDWR);
    close(unix_socket);
    close(tcp_sock);
    exit(0);
}
struct sockaddr_un unix_addr;

void rm_socket(){
    unlink(unix_addr.sun_path);
}



int main(int argc, char *argv[]) {
    atexit(rm_socket);
    struct sockaddr_in tcp_addr;
    int epoll_fd;
    struct epoll_event events[MAX_EVENTS];
    int nfds;
    int i;
    char buf[1024];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tcp_port> <unix_socket>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    tcp_port = atoi(argv[1]);
    unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&unix_addr, 0, sizeof(struct sockaddr_un));
    unix_addr.sun_family = AF_UNIX;
    strncpy(unix_addr.sun_path, argv[2], sizeof(unix_addr.sun_path) - 1);

    if (bind(unix_socket, (struct sockaddr *) &unix_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(unix_socket, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&tcp_addr, 0, sizeof(struct sockaddr_in));
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_port = htons(tcp_port);
    tcp_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(tcp_sock, (struct sockaddr *) &tcp_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(tcp_sock, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = unix_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, unix_socket, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    event.events = EPOLLIN;
    event.data.fd = tcp_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, tcp_sock, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_handler = close_all;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    while (1) {
        //clear buf
        memset(buf, 0, sizeof(buf));
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < nfds; i++) {
            if (events[i].data.fd == unix_socket || events[i].data.fd == tcp_sock) {
                int client_sock = accept(events[i].data.fd, NULL, NULL);
                if (client_sock == -1) {
                    perror("accept");
                    continue;
                }

                
                char buf[sizeof(struct Message)];
                ssize_t readed= recv(client_sock, buf, sizeof(struct Message), 0);
                if(readed < 0){
                    printf("Receive failed\n");
                    return 1;
                }
                struct Message request;
                memcpy(&request, buf, sizeof(struct Message));
                printf("Received message: %s\n", request.msg);
                
     
                if (num_clients >= MAX_CLIENTS) {
                    fprintf(stderr, "Too many clients\n");
                    close(client_sock);
                    continue;
                }

                client_sockets[num_clients++].client_socket = client_sock;
                client_sockets[num_clients-1].client_id=num_clients-1;
                strcpy(client_sockets[num_clients-1].name, request.username);
                struct Message response_id;
                response_id.client_id = num_clients-1;
                if (write(client_sock, &response_id, sizeof(struct Message)) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }

                event.events = EPOLLIN;
                event.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event) == -1) {
                    perror("epoll_ctl");
                    exit(EXIT_FAILURE);
                }
            } else {
                int client_fd = events[i].data.fd;
                ssize_t n = read(client_fd, buf, sizeof(buf));
                if (n == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                } else if (n == 0) {
                    printf("Client disconnected\n");
                    close(client_fd);
                    for (int j = 0; j < num_clients; j++) {
                        if (client_sockets[j].client_socket == client_fd) {
                            client_sockets[j] = client_sockets[num_clients - 1];
                            num_clients--;
                            break;
                        }
                    }
                } else { 
                    struct Message received;
                    memcpy(&received, buf, sizeof(struct Message));
                    printf("Received message: %s\n", received.msg);

                    switch (received.command_type)
                    {
                    case LIST:
                        struct Message response_list;
                        response_list.command_type = LIST;
                        response_list.client_id = received.client_id;
                        memset(response_list.msg, 0, sizeof(response_list.msg));
                        char temp[1024];
                        for (int j = 0; j < num_clients; j++) {
                            sprintf(temp, "Client id: %d, name: %s\n", client_sockets[j].client_id, client_sockets[j].name);
                            strcat(response_list.msg, temp);
                        }
                        
                        if (write(client_fd, &response_list, sizeof(struct Message)) == -1) {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }
                        for (int j = 0; j < num_clients; j++) {
                            printf("Client id: %d, name: %s\n", client_sockets[j].client_id, client_sockets[j].name);
                        }
                        break;
                    case _2ONE:
                        struct Message response_2one;
                        response_2one.command_type = _2ONE;
                        response_2one.client_id = received.client_id;
                        
                        memset(response_2one.msg, 0, sizeof(response_2one.msg));
                        sprintf(response_2one.msg, "%s", received.msg);
                        for (int j = 0; j < num_clients; j++) {
                            if (client_sockets[j].client_id == received.idTo) {
                                if (write(client_sockets[j].client_socket, &response_2one, sizeof(struct Message)) == -1) {
                                    perror("write");
                                    exit(EXIT_FAILURE);
                                }
                                break;
                            }
                        }
                        break;
                    case _2ALL:
                        struct Message response_2all;
                        response_2all.command_type = _2ALL;
                        response_2all.client_id = received.client_id;
                        memset(response_2all.msg, 0, sizeof(response_2all.msg));
                        sprintf(response_2all.msg, "%s", received.msg);
                        for (int j = 0; j < num_clients; j++) {
                            if (client_sockets[j].client_id != received.client_id) {
                                if (write(client_sockets[j].client_socket, &response_2all, sizeof(struct Message)) == -1) {
                                    perror("write");
                                    exit(EXIT_FAILURE);
                                }
                            }
                        }
                        break;
                    default:
                        break;
                    }
                    
                }
            }
        }
    }

    return 0;
}
