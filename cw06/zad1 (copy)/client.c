#include "com.h"
key_t server_key, client_key;
int client_id_server = -1;
mqd_t qidServer, qidClient;
int id_uniq;
pid_t parentpid;
void clean()
{
    char clientQueueName[64];
    snprintf(clientQueueName, 64, "/%d",id_uniq);
    mq_close(qidClient);
    mq_unlink(clientQueueName);
}
void stop_handler()
{
    if(parentpid!=getpid())
    {
        exit(0);
    }
    struct MsgBuf request;
    request.mtype = STOP;
    request.mtext.idFrom = qidClient;
    request.mtext.client_id = client_id_server;
    snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "%s", "STOP");

    if (mq_send(qidServer, (char *)&request, sizeof(struct MsgBuf), 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }
    exit(0);
}

void sigint_handler(int signo)
{
    stop_handler();
}

void init()
{
    atexit(clean);
    id_uniq=getpid();
    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);
    char serverQueueName[64];
    snprintf(serverQueueName, 64, "/server%d",SERVER_CONST);
    qidServer=mq_open(serverQueueName, O_WRONLY,0600,NULL);
    if (qidServer == -1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    char clientQueueName[64];
    snprintf(clientQueueName, 64, "/%d",id_uniq);
    printf("%s\n", clientQueueName);
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct MsgBuf);
    attr.mq_flags = 0;

    
    qidClient=mq_open(clientQueueName, O_CREAT | O_EXCL |O_RDONLY, 0600, &attr);
    if (qidClient == -1)
    {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    printf("Client is running\n");

    struct MsgBuf request;
    request.mtype = INIT;
    request.mtext.idFrom = id_uniq;
    request.mtext.client_id = 0;
    snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "init");
    if (mq_send(qidServer, (const char*)&request, sizeof(struct MsgBuf), 0) == -1) {
        perror("mq_send");
        exit(EXIT_FAILURE);
    }

    struct MsgBuf response;
    ssize_t msgSize = mq_receive(qidClient, (char*)&response, sizeof(struct MsgBuf), NULL);
    if (msgSize == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }
    client_id_server = response.mtext.client_id;
    if (client_id_server == -1)
    {
        printf("server is full\n");
        exit(0);
    }
    printf("Client ID on server is: %d\n", client_id_server);
}

void sender()
{

    char input[MAX_MESSAGE_LEN];
    while (1)
    {
        fgets(input, MAX_MESSAGE_LEN, stdin);
        if (strncmp(input, "LIST", 4) == 0)
        {
            struct MsgBuf request;
            request.mtype = LIST;
            request.mtext.idFrom = qidClient;
            request.mtext.client_id = client_id_server;
            snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "%s", input);
            if (mq_send(qidServer, (char*)&request, sizeof(request), 0) == -1)
            {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(input, "2ONE", 4) == 0)
        {
            struct MsgBuf request;
            request.mtype = _2ONE;
            request.mtext.idFrom = qidClient;

            sscanf(input, "%*s %d %[^\n]", &request.mtext.idTo, request.mtext.msg);
            request.mtext.client_id = client_id_server;
              if (mq_send(qidServer, (char*)&request, sizeof(request), 0) == -1)
            {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(input, "2ALL", 4) == 0)
        {
            struct MsgBuf request;
            request.mtype = _2ALL;

            sscanf(input, "%*s %[^\n]", request.mtext.msg);
            request.mtext.idFrom = qidClient;
            request.mtext.client_id = client_id_server;
      
            if (mq_send(qidServer, (char*)&request, sizeof(request), 0) == -1)
            {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }
        }
        else if (strncmp(input, "STOP", 4) == 0)
        {
            stop_handler();
        }
        else
        {
            printf("Wrong command\n");
        }
    }
}

void catcher()
{
    struct MsgBuf response;
    while (1)
    {
         if (mq_receive(qidClient, (char*) &response, sizeof(struct MsgBuf), NULL) == -1)
        {
           
        }
        else
        {
      
            switch (response.mtype)
            {
            case LIST:
                printf("\nClients online: \n%s", response.mtext.msg);
                break;
            case _2ONE:
                printf("Received message from clientid %d:\n%s\n", response.mtext.idFrom, response.mtext.msg);
                break;
            case _2ALL:
                printf("Received message from clientid %d:\n%s\n", response.mtext.idFrom, response.mtext.msg);
                break;
            case STOP:
                printf("Server is closing\n");
                kill(getppid(), SIGINT);
                exit(0);
                break;
            default:
                break;
            }
        }
    }
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    init();
    parentpid=getpid();
    if (fork() == 0)
    {
        catcher();
    }
    else
    {
        sender();
    }
    return 0;
}