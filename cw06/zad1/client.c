#include "com.h"
int qidServer, qidClient;
key_t server_key, client_key;
int client_id_server = -1;


void clean()
{
    msgctl(qidClient, IPC_RMID, NULL);
}
void stop_handler()
{
    struct MsgBuf request;
    request.mtype = STOP;
    request.mtext.idFrom = qidClient;
    request.mtext.client_id = client_id_server;
    snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "%s", "STOP");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
    {
        perror("msgsnd");
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

    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    server_key = ftok(getenv("HOME"), SERVER_CONST);
    client_key = ftok(getenv("HOME"), getpid());
    if (server_key == -1 || client_key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    qidServer = msgget(server_key, 0);
    if (qidServer == -1)
    {
        perror("Server is not running");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", qidServer);
    qidClient = msgget(client_key, IPC_CREAT | 0600);
    if (qidClient == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    printf("Client is running\n");

    struct MsgBuf request;
    request.mtype = INIT;
    request.mtext.idFrom = qidClient;
    request.mtext.client_id = 0;
    snprintf(request.mtext.msg, MAX_MESSAGE_LEN, "init");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
    {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    struct MsgBuf response;
    if (msgrcv(qidClient, &response, MSG_BUF_SIZE, INIT, 0) == -1)
    {
        perror("msgrcv");
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
            if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
            {
                perror("msgsnd");
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
            if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
            {
                perror("msgsnd");
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
            if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
            {
                perror("msgsnd");
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
        if (msgrcv(qidClient, &response, MSG_BUF_SIZE, 0, 0) == -1)
        {
            printf("Program has been stopped\n");
            exit(EXIT_FAILURE);
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