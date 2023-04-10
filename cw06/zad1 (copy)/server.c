#include "com.h"
#include <time.h>

mqd_t qidServer;
int qidClients[MAX_CLIENTS];  
key_t server_key;
int clients[MAX_CLIENTS];
int client_count = 0;


struct mq_attr attr;
    char serverQueueName[64];


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
    clients[received.mtext.client_id] = -1;
    printf("Client %d disconnected\n", received.mtext.client_id);
}


void sigint_handler(int signo)
{
    int tmp_client_count=0;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != -1)
        {
            struct MsgBuf request;
            request.mtype = STOP;
            request.mtext.idFrom = qidServer;
            request.mtext.client_id = i;
            snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "%s", "STOP");
            if (mq_send(qidClients[i], (const char *)&request, sizeof(request.mtext), 0) == -1)
            {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }
            clients[i]=-1;
            client_count--;
            qidClients[i]=-1;
            tmp_client_count++;
        }
    }
    struct MsgBuf received;
    qidServer = mq_open(serverQueueName,O_RDONLY, 0600, &attr);
    if (qidServer == -1)
    {
        perror("Could not create server queue");
        exit(EXIT_FAILURE);
    }
    if (tmp_client_count == 0)
    {
        printf("No clients connected, Server stopped\n");
        exit(0);
    }
    
    do
    {
        
        if (mq_receive(qidServer, (char *)&received, sizeof(struct MsgBuf), NULL) == -1)
        {
                perror("mq_receive STOP");
                exit(EXIT_FAILURE);   
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
  
    mq_close(qidServer);
    mq_unlink(serverQueueName);
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
        char clientQueueName[64];
        snprintf(clientQueueName, 64, "/%d",received.mtext.idFrom);
        int tmp=mq_open(clientQueueName,O_WRONLY, 0600, NULL);
        if (tmp == -1)
        {
        perror("mq_open");
        exit(EXIT_FAILURE);
        }
        qidClients[idClient]=tmp;


        printf("Client %d connected,generated ID: %d\n", received.mtext.idFrom, idClient);
    
        struct MsgBuf response;
        response.mtype = INIT;
        response.mtext.idFrom = qidServer;
        response.mtext.client_id = idClient;

        
        client_count++;
        printf("Sent ID: %d to client %d \n", idClient, received.mtext.idFrom);
         if (mq_send(qidClients[idClient], (const char *)&response, sizeof(struct MsgBuf), 0) == -1)
        {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
    }
}

void list_handler(struct MsgBuf received)
{   

    char msg[MAX_MESSAGE_LEN] = "";
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
    if (mq_send(qidClients[received.mtext.client_id], (char*)&response, sizeof(struct MsgBuf), 0) == -1)
    {
    perror("mq_send");
    exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent online client list to client %d \n", received.mtext.client_id);
    }
}

void to_one_handler(struct MsgBuf received)
{
    if (clients[received.mtext.idTo] == -1)
    {
        printf("Client %d is not registered\n", received.mtext.client_id);
        return;
    }

    struct MsgBuf response;
    response.mtype = _2ONE;
    response.mtext.idFrom = received.mtext.client_id;
    response.mtext.client_id = received.mtext.client_id;
    strcpy(response.mtext.msg, received.mtext.msg);
    if (mq_send(qidClients[received.mtext.idTo],(const char*)&response, sizeof(struct MsgBuf), 0) == -1)
    {
        perror("msgsnd to_one");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Sent message to client %d from client %d\n", received.mtext.idTo, received.mtext.client_id);
    }
}

void to_all_handler(struct MsgBuf received)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] != -1 && i != received.mtext.client_id)
        {
            struct MsgBuf response;
            response.mtype = _2ALL;
            response.mtext.idFrom = received.mtext.client_id;
            response.mtext.client_id = received.mtext.client_id;
            strcpy(response.mtext.msg, received.mtext.msg);
            if (mq_send(qidClients[i], (const char *)&response, sizeof(struct MsgBuf), 0) == -1)
            {
            perror("mq_send");
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
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i] = -1;
    }

    setbuf(stdout, NULL);
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    mqd_t qidServer;
    snprintf(serverQueueName, 64, "/server%d",SERVER_CONST);

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct MsgBuf);
    attr.mq_flags = 0;

    qidServer = mq_open(serverQueueName, O_CREAT | O_EXCL | O_RDONLY, 0600, &attr);
    if (qidServer == -1)
    {
        perror("Could not create server queue");
        exit(EXIT_FAILURE);
    }

    struct MsgBuf received;
    printf("%d\n", qidServer);

    while (1)
    {
        if (mq_receive(qidServer, (char *)&received, sizeof(struct MsgBuf), NULL) == -1)
        {
        if (errno != EAGAIN)
        {
            perror("mq_receive failed");
            exit(EXIT_FAILURE);
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