#include "com.h"
#include <time.h>

key_t server_key;
int qidServer;
int clients[MAX_CLIENTS];

void log_time_to_file(const char *filename,const char*msg, int qid) {
  time_t current_time = time(NULL);
  struct tm *tm = localtime(&current_time);
  char time_str[30];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm);
  FILE *file = fopen(filename, "a");
  if (file != NULL) {
    fprintf(file, "Date: %s | ClientQID: %d | Message:%s \n", time_str,qid,msg);
    fclose(file);
  }
}

void stop_handler(struct MsgBuf received)
{
    // remove client from clients array
    clients[received.mtext.client_id] = -1;
    printf("Client %d disconnected\n", received.mtext.client_id);
}


void sigint_handler(int signo)
{
    int tmp_client_count=0;
    //set all users to -1 and send them STOP SIGNAL
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != -1)
        {
            struct MsgBuf request;
            request.mtype = STOP;
            request.mtext.idFrom = qidServer;
            request.mtext.client_id = i;
            snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "%s", "STOP");
            if (msgsnd(clients[i], &request, MSG_BUF_SIZE, 0) == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            clients[i]=-1;
            tmp_client_count++;
        }
    }
    struct MsgBuf received;
    do
    {
        if (msgrcv(qidServer, &received, MSG_BUF_SIZE, STOP, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
            {
                perror("msgrcv");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
           stop_handler(received);
           tmp_client_count--;
        }
    } while (tmp_client_count > 0); 

    printf("All clients disconnected, Server stopped\n");
    exit(0);
}
void clean()
{
    msgctl(qidServer, IPC_RMID, NULL);
}

void init_handler(struct MsgBuf received)
{
    int idClient = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == -1)
        {
            idClient = i;
            break;
        }
    }

    if (idClient == -1)
    {
        printf("Server is full\n");
        return;
    }
    else
    {
        clients[idClient] = received.mtext.idFrom;
        printf("Client %d connected,generated ID: %d\n", received.mtext.idFrom, idClient);
        struct MsgBuf response;
        response.mtype = INIT;
        response.mtext.idFrom = qidServer;
        response.mtext.client_id = idClient;
        if (msgsnd(received.mtext.idFrom, &response, MSG_BUF_SIZE, 0) == -1)
        {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }
}

void list_handler(struct MsgBuf received)
{
    char msg[MAX_MESSAGE_LEN] = "";
    // put all registered client id in msg
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != -1)
        {
            char tmp[32];
            sprintf(tmp, "ClientID: %d \n", i);
            strcat(msg, tmp);
        }
    }

    struct MsgBuf response;
    response.mtype = LIST;
    response.mtext.idFrom = qidServer;
    response.mtext.client_id = received.mtext.client_id;
    strcpy(response.mtext.msg, msg);
    if (msgsnd(received.mtext.idFrom, &response, MSG_BUF_SIZE, 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent online client list to client %d \n", received.mtext.client_id);
    }
}

void to_one_handler(struct MsgBuf received)
{
    // check if client is registered
    if (clients[received.mtext.idTo] == -1)
    {
        printf("Client %d is not registered\n", received.mtext.client_id);
        return;
    }

    // send message to client
    struct MsgBuf response;
    response.mtype = _2ONE;
    response.mtext.idFrom = received.mtext.client_id;
    response.mtext.client_id = received.mtext.client_id;
    strcpy(response.mtext.msg, received.mtext.msg);
    if (msgsnd(clients[received.mtext.idTo], &response, MSG_BUF_SIZE, 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent message to client %d from client %d\n", received.mtext.idTo, received.mtext.client_id);
    }
}

void to_all_handler(struct MsgBuf received)
{
    // send message to all clients
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != -1 && i != received.mtext.client_id)
        {
            struct MsgBuf response;
            response.mtype = _2ALL;
            response.mtext.idFrom = received.mtext.client_id;
            response.mtext.client_id = received.mtext.client_id;
            strcpy(response.mtext.msg, received.mtext.msg);
            if (msgsnd(clients[i], &response, MSG_BUF_SIZE, 0) == -1)
            {
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }
            else
            {
                printf("Sent message to client %d from client %d\n", i, received.mtext.client_id);
            }
        }
    }
}


int main(int argc, char **argv)
{
    atexit(clean);
    // set clients array vals to -1
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
    }

    setbuf(stdout, NULL);
    // setting up handler for SIGINT
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    server_key = ftok(getenv("HOME"), SERVER_CONST);
    if (server_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    qidServer = msgget(server_key, IPC_CREAT | 0600);
    if (qidServer == -1)
    {
        perror("Could not create server queue");
        exit(EXIT_FAILURE);
    }
    struct MsgBuf received;
    printf("%d\n", qidServer);

    while (1)
    {
        if (msgrcv(qidServer, &received, MSG_BUF_SIZE, -6, IPC_NOWAIT) == -1)
        {
            // print errno value
            if (errno != ENOMSG)
            {
                printf("CHUJ");
            }
        }
        else
        {   

            printf("Received message: %s\n", received.mtext.msg);
            log_time_to_file("log.txt",received.mtext.msg, received.mtext.idFrom);
            switch (received.mtype)
            {
            case STOP:
            stop_handler(received);
            break;
            case LIST:
                list_handler(received);
                break;
            case INIT:
                init_handler(received);
                break;
            case _2ONE:
                to_one_handler(received);
                break;
            case _2ALL:
                to_all_handler(received);
                break;
            default:
                break;
            }
        }
    }
}